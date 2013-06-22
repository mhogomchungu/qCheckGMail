/*
 *
 *  Copyright (c) 2013
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qcheckgmail.h"

#define DEBUG 1

qCheckGMail::qCheckGMail() : m_menu( new KMenu() ),m_timer( new QTimer() ),
	m_gotCredentials( false ),m_walletName( "qCheckGMail" )
{
	this->setStatus( KStatusNotifierItem::NeedsAttention ) ;
	this->setCategory( KStatusNotifierItem::ApplicationStatus ) ;
	this->changeIcon( QString( "qCheckGMailError" ) ) ;
}

qCheckGMail::~qCheckGMail()
{
	m_menu->deleteLater() ;
	m_timer->deleteLater() ;
}

void qCheckGMail::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void qCheckGMail::changeIcon( QString icon )
{
	this->setIconByName( icon );
	this->setAttentionIconByName( icon ) ;
}

void qCheckGMail::run()
{
	m_reportOnAllAccounts = configurationoptionsdialog::reportOnAllAccounts() ;

	connect( this,SIGNAL( activateRequested( bool,QPoint ) ),this,SLOT( trayIconClicked( bool,QPoint ) ) ) ;

	this->setLocalLanguage();

	this->setToolTip( QString( "qCheckGMailError" ),tr( "status" ),tr( "opening wallet" ) ) ;

	m_manager = new QNetworkAccessManager( this ) ;

	connect( m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( googleQueryResponce( QNetworkReply * ) ) ) ;

	QAction * ac = new QAction( m_menu ) ;

	ac->setText( tr( "check mail now" ) ) ;

	connect( ac,SIGNAL( triggered() ),this,SLOT( checkMail() ) ) ;

	m_menu->addAction( ac ) ;

	ac = new QAction( m_menu ) ;

	ac->setText( tr( "pause checking mail" ) ) ;
	ac->setCheckable( true ) ;
	ac->setChecked( false ) ;

	connect( ac,SIGNAL( toggled( bool ) ),this,SLOT( pauseCheckingMail( bool ) ) ) ;

	m_menu->addAction( ac ) ;

	ac = new QAction( m_menu ) ;

	ac->setText( tr( "configure accounts" ) ) ;

	connect( ac,SIGNAL( triggered() ),this,SLOT( configureAccounts() ) ) ;

	m_menu->addAction( ac ) ;

	ac = new QAction( m_menu ) ;

	ac->setText( tr( "configure options" ) ) ;

	connect( ac,SIGNAL( triggered() ),this,SLOT( configurationoptionWindow() ) ) ;

	m_menu->addAction( ac ) ;

	this->setContextMenu( m_menu ) ;

	this->getAccountsInformation();

	this->setTimer() ;
	this->setTimerEvents();
	this->startTimer();
}

void qCheckGMail::googleQueryResponce( QNetworkReply * r )
{
	QByteArray content = r->readAll() ;
	r->deleteLater();

	if( content.isEmpty() ){
		this->setToolTip( QString( "qCheckGMail" ),
				  tr( "failed to connect" ),
				  tr( "check mail skipped,user is not connected to the internet" ) ) ;
		this->changeIcon( QString( "qCheckGMailError" ) );
	}else{
		if( m_reportOnAllAccounts ){
			this->reportOnAllAccounts( content ) ;
		}else{
			this->reportOnlyFirstAccountWithMail( content ) ;
		}
	}
}

QString qCheckGMail::nameToDisplay()
{
	const accounts& account    = m_accounts.at( m_currentAccount ) ;
	const QString& displayName = account.displayName() ;
	const QString& accountName = account.accountName() ;
	QString label              = account.labelUrlAt( m_currentLabel ).split( "/" ).last() ;

	if( label.isEmpty() ){
		if( displayName.isEmpty() ){
			return accountName ;
		}else{
			return displayName ;
		}
	}else{
		if( displayName.isEmpty() ){
			return QString( "%1/%2" ).arg( accountName ).arg( label ) ;
		}else{
			return QString( "%1/%2" ).arg( displayName ).arg( label ) ;
		}
	}
}

QString qCheckGMail::getAtomComponent( const QByteArray& msg,QString cmp )
{
	QString x = QString( "<%1>" ).arg( cmp ) ;
	QString z = QString( "</%1>" ).arg( cmp ) ;

	int index_1 = msg.indexOf( x ) + x.size() ;
	int index_2 = msg.indexOf( z ) - index_1  ;

	return QString( msg.mid( index_1,index_2 ) ) ;
}

void qCheckGMail::wrongAccountNameOrPassword()
{
	const accounts& acc = m_accounts.at( m_currentAccount ) ;
	this->changeIcon( QString( "qCheckGMailError" ) ) ;
	this->setToolTip( QString( "qCheckGMailError" ),
			  tr( "failed to log in" ),
			  tr( "%1 account has wrong username/password combination" ).arg( acc.accountName() ) ) ;
	m_checkingMail = false ;
}

/*
 * This function goes through all accounts and give reports of all of their states
 */
void qCheckGMail::reportOnAllAccounts( const QByteArray& msg )
{
#if DEBUG
	qDebug() << "\n" << msg;
#endif
	if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) ){
		return this->wrongAccountNameOrPassword() ;
	}

	QString mailCount = this->getAtomComponent( msg,QString( "fullcount" ) ) ;

	QString z = this->nameToDisplay() ;

	if( mailCount == QString( "0" ) ){
		m_accountStatus += QString( "<tr><td>%1</td><td>%2</td></tr>" ).arg( z ).arg( mailCount ) ;
	}else{
		m_mailCount = m_mailCount + mailCount.toInt() ;
		m_accountStatus += QString( "<tr><td><b>%1</b></td><td><b>%2</b></td></tr>" ).arg( z ).arg( mailCount ) ;
	}

	m_currentLabel++ ; //we just processed a label,increment one to go to the next one if present

	if( m_currentLabel < m_numberOfLabels ){
		/*
		 * account has more labels to go through,go through the next one
		 */
		const accounts& acc = m_accounts.at( m_currentAccount ) ;
		this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
	}else{
		m_currentAccount++ ; // we are done processing one account,go to the next one if present

		if( m_currentAccount < m_numberOfAccounts ){
			/*
			 * there are more accounts,process the next one
			 */
			this->checkMail( m_accounts.at( m_currentAccount ) ) ;
		}else{
			/*
			 * done checking all labels on all accounts
			 */
			m_checkingMail = false ;

			m_accountStatus += QString( "</table>" ) ;

			if( m_mailCount > 0 ){
				 this->setStatus( KStatusNotifierItem::NeedsAttention ) ;
				 QString icon = QString( "qCheckGMail-GotMail" ) ;
				 this->changeIcon( icon ) ;
				 qDebug() << m_mailCount ;
				 if( m_mailCount == 1 ){
					this->setToolTip( icon,tr( "found 1 new email" ),m_accountStatus ) ;
				 }else{
					this->setToolTip( icon,tr( "found %2 new emails" ).arg( QString::number( m_mailCount ) ),m_accountStatus ) ;
				 }
				 this->newEmailNotify();
			}else{
				this->setStatus( KStatusNotifierItem::Passive ) ;
				QString icon = QString( "qCheckGMail" ) ;
				this->changeIcon( icon ) ;
				this->setToolTip( icon,tr( "no new email found" ),m_accountStatus ) ;
			}
		}
	}
}

/*
 * This function stops on the first account it finds with a new email.
 * This function will hence report only the first account it finds with new email
 */
void qCheckGMail::reportOnlyFirstAccountWithMail( const QByteArray& msg )
{
#if DEBUG
	qDebug() << "\n" << msg;
#endif
	if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) ){
		return this->wrongAccountNameOrPassword() ;
	}

	QString mailCount = this->getAtomComponent( msg,QString( "fullcount" ) ) ;

	int count = mailCount.toInt() ;

	if( count > 0 ){

		QString info ;

		if( count == 1 ){
			QString x = this->getAtomComponent( msg,QString( "name" ) ) ;
			info = tr( "<table><tr><td>1 email from <b>%1</b> is waiting for you</td></tr></table>" ).arg( x ) ;
		}else{
			info = tr( "%2 emails are waiting for you" ).arg( mailCount ) ;
		}

		this->setStatus( KStatusNotifierItem::NeedsAttention ) ;
		QString icon = QString( "qCheckGMail-GotMail" ) ;
		this->changeIcon( icon ) ;

		this->setToolTip( icon,this->nameToDisplay(),info ) ;

		this->newEmailNotify();

		m_checkingMail = false ;
	}else{
		m_currentLabel++ ; //we just processed a label,increment one to go to the next one if present

		if( m_currentLabel < m_numberOfLabels ){
			/*
			 * account has more labels to go through,go through the second one
			 */
			const accounts& acc = m_accounts.at( m_currentAccount ) ;
			this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
		}else{
			m_currentAccount++ ; // we are done processing one account,go to the next one if present

			if( m_currentAccount < m_numberOfAccounts ){
				/*
				 * there are more accounts,check the next account
				 */
				this->checkMail( m_accounts.at( m_currentAccount ) ) ;
			}else{
				/*
				 * there are no more accounts and new mail not found in any of them
				 */
				m_checkingMail = false ;

				this->setToolTip( QString( "qCheckGMail" ),tr( "status" ),tr( "no new email found" ) ) ;
				this->changeIcon( QString( "qCheckGMail" ) ) ;

				this->setStatus( KStatusNotifierItem::Passive ) ;
			}
		}
	}
}

void qCheckGMail::newEmailNotify()
{
	QByteArray r( "qCheckGMail" ) ;
	KNotification::event( QString( "qCheckGMail-NewMail" ),QString( "" ),QPixmap(),0,0,
	       KComponentData( r,r,KComponentData::SkipMainComponentRegistration ) ) ;
}

void qCheckGMail::pauseCheckingMail( bool b )
{
	if( b ){
		this->stopTimer();
		this->setOverlayIconByName( QString( "qCheckGMail-GotMail" ) );
	}else{
		this->setOverlayIconByName( QString( "" ) );
		this->startTimer();

		if( m_checkingMail ){
			;
		}else{
			this->checkMail();
		}
	}
}

void qCheckGMail::configureAccounts()
{
	this->stopTimer();
	kwalletmanager * cfg = new kwalletmanager( &m_wallet,m_walletName ) ;
	connect( cfg,SIGNAL( kwalletmanagerClosed() ),this,SLOT( kwalletmanagerClosed() ) ) ;
	cfg->ShowUI() ;
}

void qCheckGMail::configurationoptionWindow()
{
	configurationoptionsdialog * cg = new configurationoptionsdialog() ;
	connect( cg,SIGNAL( setTimer( int ) ),this,SLOT( setTimer( int ) ) ) ;
	connect( cg,SIGNAL( reportOnAllAccounts( bool ) ),this,SLOT( reportOnAllAccounts( bool ) ) ) ;
	cg->ShowUI() ;
}

void qCheckGMail::reportOnAllAccounts( bool b )
{
	m_reportOnAllAccounts = b ;
}

void qCheckGMail::kwalletmanagerClosed( void )
{
	if( m_wallet->isOpen() ){
		this->getAccountsInfo();
	}else{
		this->deleteKWallet();
	}
	this->startTimer();
}

void qCheckGMail::deleteKWallet()
{
	m_wallet->deleteLater();
	m_wallet = 0 ;
}

/*
 * This should be the only function that initiate email checking
 */
void qCheckGMail::checkMail()
{
	if( m_gotCredentials ){
		/*
		* check for updates on the first account
		*/
		if( m_numberOfAccounts > 0 ){
			//m_accountStatus   = QString( "<table><col width=\"%1\"><col width=\"3\">" ).arg( m_accountNameColumnWidth ) ;
			m_accountStatus   = QString( "<table>" );
			m_mailCount       = 0 ;
			m_currentAccount  = 0 ; // use to track the account we are checking
			m_checkingMail    = true ;
			this->checkMail( m_accounts.at( m_currentAccount ) );
		}else{
			qDebug() << "BUGG!!,tried to check for mails when there are no accounts configured" ;
		}
	}else{
		qDebug() << tr( "dont have credentials,(re)trying to open wallet" ) ;
		this->getAccountsInformation() ;
	}
}

/*
 * try to force tray bubble to expand to the width of longest account+label to force it to
 * not split the text to two lines
 */
void qCheckGMail::displaNameColumnWidth()
{
	int width = 0 ;
	int j = m_accounts.size() ;
	int k ;
	int z ;
	int n ;

	for( int i = 0 ; i < j ; i++ ){
		const accounts& acc = m_accounts.at( i ) ;
		const QStringList& l = acc.labelUrls() ;
		k = l.size() ;
		if( acc.displayName().isEmpty() ){
			z = acc.accountName().size() ;
			if( k > 1 ){
				for( int e = 1 ; e < k ; e++ ){
					n = l.at( e ).split( "/" ).last().size() + z + 1 ;
					if( width < n ){
						width = n ;
					}
				}
			}else{
				if( width < z ){
					width = z ;
				}
			}
		}else{
			z = acc.displayName().size() ;
			if( k > 1 ){
				for( int e = 1 ; e < k ; e++ ){
					n = l.at( e ).split( "/" ).last().size() + z + 1 ;
					if( width < n ){
						width = n ;
					}
				}
			}else{
				if( width < z ){
					width = z ;
				}
			}
		}
	}

	/*
	 * m_accountNameColumnWidth will contain the length of the longer accountName+label/accountDisplayName+label
	 */
	m_accountNameColumnWidth = QString::number( width  ) ;
}

void qCheckGMail::checkMail( const accounts& acc )
{
	m_currentLabel = 0 ;
	m_numberOfLabels = acc.numberOfLabels() ;
	this->checkMail( acc,acc.defaultLabelUrl() ) ;
}

void qCheckGMail::checkMail( const accounts& acc,const QString& UrlLabel )
{
	QUrl url( UrlLabel ) ;

	url.setUserName( acc.accountName() ) ;
	url.setPassword( acc.passWord() ) ;

	QNetworkRequest rqt( url ) ;

	m_manager->get( rqt ) ;
}

void qCheckGMail::getAccountsInformation()
{
	m_wallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;
	connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
}

void qCheckGMail::walletOpened( bool opened )
{
	if( m_wallet ){
		if( opened ){
			this->getAccountsInfo();
		}else{
			this->walletNotOPenedError();
		}
	}else{
		qDebug() << "BUGG!!,walletOpened(): m_wallet is void" ;
	}
}

void qCheckGMail::getAccountsInfo()
{
	if( m_wallet ){
		/*
		 * get accounts information from kwallet
		 */
		m_accounts = kwalletmanager::getAccounts( m_wallet ) ;

		m_numberOfAccounts = m_accounts.size() ;

		m_gotCredentials = m_numberOfAccounts > 0 ;

		if( m_gotCredentials ){
			//this->displaNameColumnWidth() ;
			this->checkMail() ;
		}else{
			/*
			 * wallet is empty,warn the user about it
			 */
			this->noAccountConfigured() ;
		}

		m_wallet->closeWallet( m_walletName,false ) ;
		this->deleteKWallet();
	}else{
		qDebug() << "BUGG!!,getAccountsInfo(): m_wallet is void" ;
		return ;
	}
}

void qCheckGMail::noAccountConfigured()
{
	QString x( "qCheckGMailError" );
	this->changeIcon( x ) ;
	this->setToolTip( x,tr( "error" ),tr( "no account appear to be configured in the wallet" ) ) ;
}

void qCheckGMail::walletNotOPenedError()
{
	qDebug() << tr( "wallet not opened" ) ;
	this->setToolTip( QString( "qCheckGMailError"),tr( "status" ),tr( "error,failed to open wallet" ) ) ;
	if( m_wallet ){
		this->deleteKWallet();
	}else{
		qDebug() << "BUGG!!,walletNotOPenedError(): m_wallet is void" ;
	}
}

QStringList qCheckGMail::getAccountNames()
{
	QStringList l ;
	int j = m_accounts.size();
	for( int i = 0 ; i < j ; i++ ){
		l.append( m_accounts.at( i ).accountName() ) ;
	}
	return l ;
}

void qCheckGMail::setLocalLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;

	QString lang     = configurationoptionsdialog::localLanguage() ;
	QString langPath = configurationoptionsdialog::localLanguagePath() ;

	QByteArray r = lang.toAscii() ;

	QByteArray e( "english_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath ) ;
		QCoreApplication::installTranslator( translator ) ;
	}
}

void qCheckGMail::objectDestroyed()
{
	qDebug() << "objectDestryoed" ;
}

void qCheckGMail::setLocalLanguage( QCoreApplication& qapp,QTranslator * translator )
{
	QString lang     = configurationoptionsdialog::localLanguage() ;
	QString langPath = configurationoptionsdialog::localLanguagePath() ;

	QByteArray r = lang.toAscii() ;

	QByteArray e( "english_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath ) ;
		qapp.installTranslator( translator ) ;
	}
}

void qCheckGMail::setTimer()
{
	m_interval = configurationoptionsdialog::getTimeFromConfigFile() ;
}

void qCheckGMail::setTimer( int time )
{
	m_interval = time ;
	this->startTimer() ;
}

void qCheckGMail::trayIconClicked( bool x,const QPoint & y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
	KToolInvocation::invokeBrowser( "https://mail.google.com/mail" ) ;
}

void qCheckGMail::startTimer()
{
	m_timer->stop() ;
	m_timer->start( m_interval ) ;
}

void qCheckGMail::stopTimer()
{
	m_timer->stop() ;
}

void qCheckGMail::setTimerEvents()
{
	connect( m_timer,SIGNAL( timeout() ),this,SLOT( checkMail() ) ) ;
}

int qCheckGMail::instanceAlreadyRunning()
{
	QString lang = configurationoptionsdialog::localLanguage() ;
	QByteArray r = lang.toAscii() ;

	QByteArray e( "english_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
		qDebug() << tr( "another instance is already running,exiting this one" ) ;
	}else{
		int argc = 1 ;
		const char * x[ 2 ] ;
		x[ 0 ] = "qCheckGMail" ;
		x[ 1 ] =  0 ;
		char ** z = ( char ** ) x ;
		QCoreApplication qapp( argc,z ) ;

		QString langPath = configurationoptionsdialog::localLanguagePath() ;
		QTranslator translator ;
		translator.load( r.constData(),langPath ) ;
		qapp.installTranslator( &translator ) ;
		qDebug() << tr( "another instance is already running,exiting this one" ) ;
	}

	return 1 ;
}

int qCheckGMail::autoStartDisabled()
{
	QString lang = configurationoptionsdialog::localLanguage() ;
	QByteArray r = lang.toAscii() ;

	QByteArray e( "english_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
		qDebug() << tr( "autostart disabled,exiting this one" ) ;
	}else{
		int argc = 1 ;
		const char * x[ 2 ] ;
		x[ 0 ] = "qCheckGMail" ;
		x[ 1 ] =  0 ;
		char ** z = ( char ** ) x ;
		QCoreApplication qapp( argc,z ) ;

		QString langPath = configurationoptionsdialog::localLanguagePath() ;
		QTranslator translator ;
		translator.load( r.constData(),langPath ) ;
		qapp.installTranslator( &translator ) ;
		qDebug() << tr( "autostart disabled,exiting this one" ) ;
	}

	return 1 ;
}

bool qCheckGMail::autoStartEnabled()
{
	return configurationoptionsdialog::autoStartEnabled() ;
}
