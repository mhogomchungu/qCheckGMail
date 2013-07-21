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

qCheckGMail::qCheckGMail() : m_menu( new KMenu() ),m_timer( new QTimer() ),
	m_gotCredentials( false ),m_walletName( "qCheckGMail" )
{
	KStatusNotifierItem::setStatus( KStatusNotifierItem::NeedsAttention ) ;
	KStatusNotifierItem::setCategory( KStatusNotifierItem::ApplicationStatus ) ;

	this->changeIcon( QString( "qCheckGMailError" ) ) ;
}

qCheckGMail::~qCheckGMail()
{
	m_manager->deleteLater() ;
	m_menu->deleteLater() ;
	m_timer->deleteLater() ;
	delete m_mutex ;
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
	this->initLogFile();

	m_enableDebug = KCmdLineArgs::allArguments().contains( "-d" ) ;

	m_reportOnAllAccounts = configurationoptionsdialog::reportOnAllAccounts() ;

	connect( this,SIGNAL( activateRequested( bool,QPoint ) ),this,SLOT( trayIconClicked( bool,QPoint ) ) ) ;

	this->setLocalLanguage();

	KStatusNotifierItem::setToolTip( QString( "qCheckGMailError" ),tr( "status" ),tr( "opening wallet" ) ) ;

	QAction * ac = new QAction( m_menu ) ;

	ac->setText( tr( "check mail now" ) ) ;

	connect( ac,SIGNAL( triggered() ),this,SLOT( checkMail() ) ) ;

	m_menu->addAction( ac ) ;

	ac = new QAction( m_menu ) ;

	ac->setText( tr( "pause checking mail" ) ) ;
	ac->setObjectName( QString( "pauseCheckingMail" ) ) ;
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

	KStatusNotifierItem::setContextMenu( m_menu ) ;

	m_mutex = new QMutex() ;

	m_checkingMail = false ;

	this->getAccountsInformation();

	m_interval = configurationoptionsdialog::getTimeFromConfigFile() ;

	connect( m_timer,SIGNAL( timeout() ),this,SLOT( checkMail() ) ) ;
	
	m_manager = new QNetworkAccessManager( this ) ;
	connect( m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( googleQueryResponce( QNetworkReply * ) ) ) ;
	
	m_timer->stop() ;
	m_timer->start( m_interval ) ;
}

void qCheckGMail::noInternet( void )
{
	KStatusNotifierItem::setToolTip( QString( "qCheckGMailError" ),
					 tr( "failed to connect" ),
					 tr( "check mail skipped,user is not connected to the internet" ) ) ;
	this->changeIcon( QString( "qCheckGMailError" ) );
	this->doneCheckingMail() ;
}

void qCheckGMail::googleQueryResponce( QNetworkReply * r )
{
	QByteArray content = r->readAll() ;

	r->deleteLater();

	if( content.isEmpty() ){
		this->noInternet() ;
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
	QString x = m_accounts.at( m_currentAccount ).accountName() ;
	QString z = QString( "qCheckGMailError" ) ;
	QString e = tr( "%1 account has wrong username/password combination" ).arg( x ) ;

	this->changeIcon( z ) ;
	KStatusNotifierItem::setToolTip( z,tr( "failed to log in" ),e ) ;
	this->doneCheckingMail() ;
}

/*
 * This function goes through all accounts and give reports of all of their states
 */
void qCheckGMail::reportOnAllAccounts( const QByteArray& msg )
{
	if( m_enableDebug ){
		qDebug() << "\n" << msg ;
	}

	if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) ){
		return this->wrongAccountNameOrPassword() ;
	}

	QString mailCount = this->getAtomComponent( msg,QString( "fullcount" ) ) ;

	int mailCount_1 = mailCount.toInt() ;

	if( mailCount_1 == 0 ){
		m_accountsStatus += QString( "<tr valign=\"middle\"><td width=\"80%\">%1</td><td>0</td></tr>" ).arg( this->nameToDisplay() ) ;
	}else{
		m_mailCount += mailCount_1 ;
		QString z = this->nameToDisplay() ;
		m_accountsStatus += QString( "<tr valign=\"middle\"><td width=\"80%\"><b>%1</b></td><td><b>%2</b></td></tr>" ).arg( z ).arg( mailCount ) ;
	}

	/*
	 * done processing a label in an account,go to the next label if present
	 */
	m_currentLabel++ ;

	if( m_currentLabel < m_numberOfLabels ){
		/*
		 * account has more labels and are we are at the next one and are about to go through it
		 */
		const accounts& acc = m_accounts.at( m_currentAccount ) ;
		this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
	}else{
		/*
		 * we are done processing an account,go to the next one if available
		 */
		m_currentAccount++ ;

		if( m_currentAccount < m_numberOfAccounts ){
			/*
			 * more accounts are configured and we are at the next one and are about to go through it
			 */
			this->checkMail( m_accounts.at( m_currentAccount ) ) ;
		}else{
			/*
			 * done checking all labels on all accounts
			 */
			m_accountsStatus += QString( "</table>" ) ;

			if( m_mailCount > 0 ){
				KStatusNotifierItem::setStatus( KStatusNotifierItem::NeedsAttention ) ;
				QString icon = QString( "qCheckGMail-GotMail" ) ;
				this->changeIcon( icon ) ;
				if( m_mailCount == 1 ){
					KStatusNotifierItem::setToolTip( icon,tr( "found 1 new email" ),m_accountsStatus ) ;
				}else{
					QString x = QString::number( m_mailCount ) ;
					KStatusNotifierItem::setToolTip( icon,tr( "found %2 new emails" ).arg( x ),m_accountsStatus ) ;
				}
				this->newEmailNotify();
			}else{
				KStatusNotifierItem::setStatus( KStatusNotifierItem::Passive ) ;
				QString icon = QString( "qCheckGMail" ) ;
				this->changeIcon( icon ) ;
				KStatusNotifierItem::setToolTip( icon,tr( "no new email found" ),m_accountsStatus ) ;
			}

			this->doneCheckingMail() ;
		}
	}
}

/*
 * This function stops on the first account it finds with a new email.
 * This function will hence report only the first account it finds with new email
 *
 * This mail checking way is visually more appealing on the tray bubble when only one
 * account is set up
 */
void qCheckGMail::reportOnlyFirstAccountWithMail( const QByteArray& msg )
{
	if( m_enableDebug ){
		qDebug() << "\n" << msg ;
	}

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

		KStatusNotifierItem::setStatus( KStatusNotifierItem::NeedsAttention ) ;
		QString icon = QString( "qCheckGMail-GotMail" ) ;
		this->changeIcon( icon ) ;

		KStatusNotifierItem::setToolTip( icon,this->nameToDisplay(),info ) ;

		this->newEmailNotify();

		this->doneCheckingMail() ;
	}else{
		/*
		 * done processing a label in an account,go to the next label if present
		 */
		m_currentLabel++ ;

		if( m_currentLabel < m_numberOfLabels ){
			/*
			 * account has more labels and are we are at the next one and are about to go through it
			 */
			const accounts& acc = m_accounts.at( m_currentAccount ) ;
			this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
		}else{
			/*
			 * we are done processing an account,go to the next one if available
			 */
			m_currentAccount++ ;

			if( m_currentAccount < m_numberOfAccounts ){
				/*
				 * more accounts are configured and we are at the next one and are about to go through it
				 */
				this->checkMail( m_accounts.at( m_currentAccount ) ) ;
			}else{
				/*
				 * there are no more accounts to go through
				 */
				KStatusNotifierItem::setToolTip( QString( "qCheckGMail" ),tr( "status" ),tr( "no new email found" ) ) ;
				this->changeIcon( QString( "qCheckGMail" ) ) ;
				KStatusNotifierItem::setStatus( KStatusNotifierItem::Passive ) ;
				this->doneCheckingMail() ;
			}
		}
	}
}

void qCheckGMail::doneCheckingMail()
{
	m_mutex->lock();
	m_checkingMail = false ;
	bool redoMailCheck = m_redoMailCheck ;
	m_mutex->unlock();
	if( redoMailCheck ){
		/*
		 * we are redoing checking mail because a user changed account properties while we
		 * were in the middle of checking mail.We are redoing the check to give a corrent
		 * account info in the tray bubble
		 */
		this->checkMail() ;
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
		KStatusNotifierItem::setOverlayIconByName( QString( "qCheckGMail-GotMail" ) );
	}else{
		KStatusNotifierItem::setOverlayIconByName( QString( "" ) );
		this->startTimer();

		m_mutex->lock();
		bool checking = m_checkingMail ;
		m_mutex->unlock();
		if( checking ){
			QString log = QString( "WARNING,manual mail check from auto pause attempted when mail checking is already in progress" ) ;
			this->writeToLogFile( log ) ;
			this->stuck();
		}else{
			this->checkMail() ;
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
	connect( cg,SIGNAL( setTimer( int ) ),this,SLOT( configurationWindowClosed( int ) ) ) ;
	connect( cg,SIGNAL( reportOnAllAccounts( bool ) ),this,SLOT( reportOnAllAccounts( bool ) ) ) ;
	cg->ShowUI() ;
}

void qCheckGMail::configurationWindowClosed( int r )
{
	if( m_interval != r ){
		this->setTimer( r ) ;
	}
}

void qCheckGMail::reportOnAllAccounts( bool b )
{
	m_reportOnAllAccounts = b ;
}

void qCheckGMail::kwalletmanagerClosed( void )
{
	if( m_wallet->isOpen() ){
		m_mutex->lock();
		m_redoMailCheck = true ;
		m_mutex->unlock();
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
			m_accountsStatus  = QString( "<table>" ) ;
			m_mailCount       = 0 ;
			m_currentAccount  = 0 ;

			bool cancheckMail = false ;

			m_mutex->lock() ;

			if( m_checkingMail ){
				QString log = QString( "WARNING!!,mail checking attempted while mail checking is already in progress" ) ;
				this->writeToLogFile( log ) ;
				this->stuck();
			}else{
				cancheckMail   = true ;
				m_checkingMail = true ;
			}

			m_redoMailCheck   = false ;

			m_mutex->unlock() ;

			if( cancheckMail ){
				this->checkMail( m_accounts.at( m_currentAccount ) ) ;
			}else{
				;
			}
		}else{
			QString log = QString( "BUGG!!,tried to check for mails when there are no accounts configured" ) ;
			this->writeToLogFile( log ) ;
		}
	}else{
		qDebug() << tr( "dont have credentials,(re)trying to open wallet" ) ;
		this->writeToLogFile( QString( "dont have credentials,(re)trying to open wallet" ) ) ;
		this->getAccountsInformation() ;
	}
}

void qCheckGMail::stuck()
{
	/*
	 * possible reason for getting stuck.
	 * 1. On my system,m_manager->get() just stops working when its called and the network is down and i have yet
	 *    to discover how to make it start working it again when the network is reestablished.
	 */
	QString err = tr( "recommending a restart as i am probably stuck" ) ;
	QString icon = QString( "qCheckGMailError" ) ;
	this->changeIcon( icon );
	KStatusNotifierItem::setToolTip( icon,tr( "error" ),err ) ;
	KStatusNotifierItem::setStatus( KStatusNotifierItem::NeedsAttention ) ;
}

void qCheckGMail::checkMail( const accounts& acc )
{
	m_currentLabel   = 0 ;
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

void qCheckGMail::objectGone( QObject * obj )
{
	if( m_enableDebug ){
		qDebug() << "destroyed object:" << obj->objectName() ;
	}
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
		QString log = QString( "BUGG!!,walletOpened(): m_wallet is void" ) ;
		this->writeToLogFile( log ) ;
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
		QString log = QString( "BUGG!!,getAccountsInfo(): m_wallet is void" ) ;
		this->writeToLogFile( log ) ;
	}
}

void qCheckGMail::noAccountConfigured()
{
	QString x( "qCheckGMailError" );
	this->changeIcon( x ) ;
	KStatusNotifierItem::setToolTip( x,tr( "error" ),tr( "no account appear to be configured in the wallet" ) ) ;
}

void qCheckGMail::walletNotOPenedError()
{
	qDebug() << tr( "wallet not opened" ) ;
	KStatusNotifierItem::setToolTip( QString( "qCheckGMailError"),tr( "status" ),tr( "error,failed to open wallet" ) ) ;
	if( m_wallet ){
		this->deleteKWallet();
	}else{
		QString log = QString( "BUGG!!,walletNotOPenedError(): m_wallet is void" ) ;
		this->writeToLogFile( log ) ;
	}
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

	if( m_numberOfAccounts > 0 ){
		QString url = m_accounts.at( 0 ).defaultLabelUrl() ;
		int index = url.size() - QString( "/feed/atom/" ).size() ;
		url.truncate( index ) ;
		KToolInvocation::invokeBrowser( url ) ;
	}else{
		KToolInvocation::invokeBrowser( "https://mail.google.com/" ) ;
	}
}

void qCheckGMail::startTimer()
{
	QList<QAction*> ac = m_menu->actions() ;
	int j = ac.size() ;
	QString pauseMenuContext = QString( "pauseCheckingMail" ) ;
	for( int i = 0 ; i < j ; i++ ){
		if( ac.at( i )->objectName() == pauseMenuContext ){
			if( ac.at( i )->isChecked() ){
				;
			}else{
				m_timer->stop() ;
				m_timer->start( m_interval ) ;
			}
		}
	}
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

void qCheckGMail::initLogFile()
{
	QString logFile = configurationoptionsdialog::logFile() ;
	QString logFile_old = logFile + QString( ".old" ) ;
	QFile f( logFile_old ) ;
	f.remove() ;
	QFile::rename( logFile,logFile_old ) ;
}

void qCheckGMail::writeToLogFile( QString log )
{
	qDebug() << log ;
	QString logFile = configurationoptionsdialog::logFile() ;
	QFile f( logFile ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Append ) ;
	f.write( log.toAscii() + "\n" ) ;
}

bool qCheckGMail::autoStartEnabled()
{
	return configurationoptionsdialog::autoStartEnabled() ;
}
