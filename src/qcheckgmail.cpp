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

qCheckGMail::qCheckGMail() : statusicon( m_accounts )
{
	m_timer = new QTimer( this ) ;

	m_numberOfAccounts = 0 ;
	m_numberOfLabels   = 0 ;

	statusicon::setCategory( statusicon::ApplicationStatus ) ;

	this->changeIcon( QString( "qCheckGMailError" ) ) ;
	this->setTrayIconToVisible( true ) ;
}

qCheckGMail::~qCheckGMail()
{
	delete m_mutex ;
}

void qCheckGMail::setTrayIconToVisible( bool showIcon )
{
	if( showIcon ){
		statusicon::setStatus( statusicon::NeedsAttention ) ;
	}else{
		statusicon::setStatus( statusicon::Passive ) ;
	}
}

void qCheckGMail::showToolTip( QString x,QString y,QString z )
{
	statusicon::setToolTip( x,y,z ) ;
}

void qCheckGMail::showPausedIcon( bool paused )
{
	if( paused ){
		statusicon::setOverlayIcon( QString( "qCheckGMail-GotMail" ) ) ;
	}else{
		statusicon::setOverlayIcon( QString( "" ) ) ;
	}
}

void qCheckGMail::changeIcon( QString icon )
{
	statusicon::setIcon( icon );
	statusicon::setAttentionIcon( icon ) ;
}

void qCheckGMail::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void qCheckGMail::run()
{
	m_enableDebug = this->enableDebug() ;

	this->setLocalLanguage();

	m_reportOnAllAccounts = configurationoptionsdialog::reportOnAllAccounts() ;

	QObject * parent = statusicon::statusQObject() ;

	QAction * ac = new QAction( parent ) ;
	ac->setText( tr( "check mail now" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( checkMail() ) ) ;
	statusicon::addAction( ac ) ;

	ac = new QAction( parent ) ;
	ac->setText( tr( "pause checking mail" ) ) ;
	ac->setObjectName( QString( "pauseCheckingMail" ) ) ;
	ac->setCheckable( true ) ;
	ac->setChecked( false ) ;
	connect( ac,SIGNAL( toggled( bool ) ),this,SLOT( pauseCheckingMail( bool ) ) ) ;
	statusicon::addAction( ac ) ;

	ac = new QAction( parent ) ;
	ac->setText( tr( "configure accounts" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( configureAccounts() ) ) ;
	statusicon::addAction( ac ) ;

	ac = new QAction( parent ) ;
	ac->setText( tr( "configure password" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( configurePassWord() ) ) ;
	statusicon::addAction( ac ) ;

	ac = new QAction( parent ) ;
	ac->setText( tr( "configure options" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( configurationoptionWindow() ) ) ;
	statusicon::addAction( ac ) ;

	statusicon::addQuitAction() ;

	m_mutex = new QMutex() ;

	m_checkingMail = false ;

	m_manager = new QNetworkAccessManager( this ) ;
	connect( m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( emailStatusQueryResponce( QNetworkReply * ) ) ) ;

	m_interval = configurationoptionsdialog::getTimeFromConfigFile() ;
	connect( m_timer,SIGNAL( timeout() ),this,SLOT( checkMail() ) ) ;
	m_timer->start( m_interval ) ;

	this->showToolTip( QString( "qCheckGMailError" ),tr( "status" ),tr( "opening wallet" ) ) ;

	this->getAccountsInfo() ;
}

void qCheckGMail::noInternet( void )
{
	QString header = tr( "network problem detected" ) ;
	QString msg    = tr( "could not connect to the internet" ) ;
	QString icon   = QString( "qCheckGMailError" ) ;

	this->showToolTip( icon,header,msg ) ;
	this->changeIcon( icon );
	this->doneCheckingMail() ;
}

void qCheckGMail::emailStatusQueryResponce( QNetworkReply * r )
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

QString qCheckGMail::displayName()
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
	this->showToolTip( z,tr( "account related error was detected" ),e ) ;
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
		QString r = QString( "<tr valign=middle><td>%1</td><td width=50 align=right>0</td></tr>" ) ;
		m_accountsStatus += r.arg( this->displayName() ) ;
	}else{
		m_mailCount += mailCount_1 ;
		QString r = QString( "<tr valign=middle><td><b>%1</b></td><td width=50 align=right><b>%2</b></td></tr>" ) ;
		m_accountsStatus += r.arg( this->displayName() ).arg( mailCount ) ;
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
				QString icon = QString( "qCheckGMail-GotMail" ) ;
				this->changeIcon( icon ) ;
				this->setTrayIconToVisible( true ) ;
				if( m_mailCount == 1 ){
					this->showToolTip( icon,tr( "found 1 new email" ),m_accountsStatus ) ;
				}else{
					QString x = QString::number( m_mailCount ) ;
					this->showToolTip( icon,tr( "found %2 new emails" ).arg( x ),m_accountsStatus ) ;
				}
				statusicon::newEmailNotify() ;
			}else{
				QString icon = QString( "qCheckGMail" ) ;
				this->changeIcon( icon ) ;
				this->setTrayIconToVisible( false ) ;
				this->showToolTip( icon,tr( "no new email found" ),m_accountsStatus ) ;
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

		QString icon = QString( "qCheckGMail-GotMail" ) ;
		this->changeIcon( icon ) ;
		this->setTrayIconToVisible( true ) ;
		this->showToolTip( icon,this->displayName(),info ) ;
		statusicon::newEmailNotify() ;
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
				this->showToolTip( QString( "qCheckGMail" ),tr( "status" ),tr( "no new email found" ) ) ;
				this->changeIcon( QString( "qCheckGMail" ) ) ;
				this->setTrayIconToVisible( false ) ;
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

void qCheckGMail::pauseCheckingMail( bool pauseAction )
{
	this->showPausedIcon( pauseAction ) ;

	if( pauseAction ){
		this->stopTimer();
	}else{
		this->startTimer();

		m_mutex->lock();
		bool checking = m_checkingMail ;
		m_mutex->unlock();
		if( checking ){
			this->stuck();
		}else{
			this->checkMail() ;
		}
	}
}

void qCheckGMail::configureAccounts()
{
	walletmanager * wallet = new walletmanager() ;
	connect( wallet,SIGNAL( walletmanagerClosed() ),this,SLOT( walletmanagerClosed() ) ) ;
	connect( wallet,SIGNAL( getAccountsInfo( QVector<accounts> ) ),this,SLOT( getAccountsInfo( QVector<accounts> ) ) ) ;
	wallet->ShowUI() ;
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

void qCheckGMail::reportOnAllAccounts( bool reportOnAllAccounts )
{
	m_reportOnAllAccounts = reportOnAllAccounts ;
}

void qCheckGMail::walletmanagerClosed( void )
{
	m_mutex->lock();
	m_redoMailCheck = true ;
	m_mutex->unlock();
}

void qCheckGMail::stuck()
{
	/*
	 * We will get here if an attempt to check for email update is made while another attempt is already in progress.
	 * Mail checking usually takes a few seconds and hence the most likely reason to get here is if the network is down
	 * and the already in progress attempt is stuck somewhere in QNetworkAccessManager object.
	 */
	//QString err = tr( "qCheckGMail is not responding and restarting it is recommended" ) ;
	QString x = tr( "network problem detected" ) ;
	QString msg_1 = tr( "email checking is taking longer than expected." ) ;
	QString msg_2 = tr( "Recommending restarting qCheckGMail if the problem persists" ) ;
	QString z = QString( "<table><tr><td>%1</td></tr><tr><td>%2</td></tr></table>" ).arg( msg_1 ).arg( msg_2 ) ;

	QString icon = QString( "qCheckGMailError" ) ;
	this->changeIcon( icon );
	this->showToolTip( icon,x,z ) ;
	this->setTrayIconToVisible( true ) ;
}

/*
 * This should be the only function that initiate email checking
 */
void qCheckGMail::checkMail()
{
	if( m_numberOfAccounts > 0 ){

		bool cancheckMail = false ;

		m_mutex->lock() ;

		if( m_checkingMail ){
			this->stuck() ;
		}else{
			cancheckMail   = true ;
			m_checkingMail = true ;
		}

		m_redoMailCheck = false ;

		m_mutex->unlock() ;

		if( cancheckMail ){
			m_accountsStatus  = QString( "<table>" ) ;
			m_mailCount       = 0 ;
			m_currentAccount  = 0 ;
			this->checkMail( m_accounts.at( m_currentAccount ) ) ;
		}else{
			;
		}
	}else{
		qDebug() << tr( "dont have credentials,(re)trying to open wallet" ) ;
		this->getAccountsInfo() ;
	}
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

void qCheckGMail::objectGone( QObject * obj )
{
	if( m_enableDebug ){
		qDebug() << "destroyed object:" << obj->objectName() ;
	}
}

void qCheckGMail::getAccountsInfo( QVector<accounts> acc )
{
	m_accounts = acc ;

	m_numberOfAccounts = m_accounts.size() ;

	if( m_numberOfAccounts > 0 ){
		this->checkMail() ;
	}else{
		/*
		 * wallet is empty,warn the user about it
		 */
		this->noAccountConfigured() ;
	}
}

const QVector<accounts>& qCheckGMail::configuredAccounts()
{
	return m_accounts ;
}

void qCheckGMail::configurePassWord()
{
	walletmanager * wallet = new walletmanager() ;
	wallet->changeWalletPassword() ;
}

void qCheckGMail::getAccountsInfo()
{
	walletmanager * wallet = new walletmanager() ;
	connect( wallet,SIGNAL( getAccountsInfo( QVector<accounts> ) ),this,SLOT( getAccountsInfo( QVector<accounts> ) ) ) ;
	wallet->getAccounts() ;
}

void qCheckGMail::noAccountConfigured()
{
	QString x( "qCheckGMailError" ) ;
	this->changeIcon( x ) ;
	this->showToolTip( x,tr( "account related error was detected" ),tr( "no account appear to be configured in the wallet" ) ) ;
}

void qCheckGMail::setLocalLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;

	QString lang     = configurationoptionsdialog::localLanguage() ;
	QString langPath = configurationoptionsdialog::localLanguagePath() ;

	QByteArray r = lang.toLatin1() ;

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

	QByteArray r = lang.toLatin1() ;

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

QWidget * qCheckGMail::widget()
{
	return statusicon::widget() ;
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

void qCheckGMail::startTimer()
{
	QList<QAction*> ac = statusicon::getMenuActions() ;
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

int qCheckGMail::instanceAlreadyRunning()
{
	QString lang = configurationoptionsdialog::localLanguage() ;
	QByteArray r = lang.toLatin1() ;

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
	QByteArray r = lang.toLatin1() ;

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
