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

// 5 minutes update interval
#define CHECK_UPDATE_INTERVAL 1000 * 5 * 60

qCheckGMail::qCheckGMail() : m_menu( new KMenu() ),m_timer( new QTimer() ),
	m_gotCredentials( false ),m_walletName( "qCheckGmail" )
{
	this->setStatus( KStatusNotifierItem::Passive ) ;
	this->setCategory( KStatusNotifierItem::ApplicationStatus ) ;
	this->changeIcon( QString( "qCheckGMailError" ) ) ;
	this->setToolTip( QString( "qCheckGMailError" ),tr( "status" ),tr( "opening wallet" ) ) ;
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
	m_manager = new QNetworkAccessManager( this ) ;

	connect( m_manager,SIGNAL( finished( QNetworkReply * ) ),this,SLOT( gotReply( QNetworkReply * ) ) ) ;

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

	connect( ac,SIGNAL( triggered() ),this,SLOT( configurationWindow() ) ) ;

	m_menu->addAction( ac ) ;

	this->setContextMenu( m_menu ) ;

	this->getAccountsInformation();
}

void qCheckGMail::gotReply( QNetworkReply * r )
{
	QByteArray content = r->readAll() ;
	if( content.isEmpty() ){
		this->setToolTip( QString( "qCheckGMail" ),
				  tr( "failed to connect" ),
				  tr( "check mail skipped,user is not connected to the internet" ) ) ;
		this->changeIcon( QString( "qCheckGMailError" ) );
	}else{
		this->processMailStatus( content ) ;
	}
	r->deleteLater();
}

void qCheckGMail::processMailStatus( const QByteArray& msg )
{
	//qDebug() << msg;

	if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) ){
		this->changeIcon( QString( "qCheckGMailError" ) ) ;
		this->setToolTip( QString( "qCheckGMailError" ),
				  tr( "failed to log in" ),
				  tr( "wrong username/password combination" ) ) ;
		return	;
	}

	int index_1 = msg.indexOf( "<fullcount>" ) ;

	int index_2 = msg.indexOf( "</fullcount>" ) ;

	int c = strlen( "<fullcount>" ) ;

	QByteArray md = msg.mid( index_1 + c ,index_2 - ( index_1 + c ) ) ;
	QString mails = QString( md ) ;

	index_1 = msg.indexOf( "<title>" ) + strlen( "<title>" );

	index_2 = msg.indexOf( "</title>" ) ;

	c = strlen( "<title>" ) ;

	QString account = msg.mid( index_1 + c,index_2 - ( index_1 + c ) ) ;

	QString accountName ;
	int count = mails.toInt() ;

	QStringList accNames = this->getAccountNames() ;
	int r = accNames.size() ;

	QString info ;

	for( int i = 0 ; i < r ; i++ ){
		accountName = accNames.at( i ) ;
		if( account.contains( accountName ) ){
			if( count == 1 ){
				info = tr( "1 email is waiting for you" ) ;
			}else if( count > 1 ){
				info = tr( "%2 emails are waiting for you" ).arg( mails ) ;
			}
			break ;
		}
	}

	if( count > 0 ){
		this->setStatus( KStatusNotifierItem::NeedsAttention ) ;
		QString icon = QString( "qCheckGMail-GotMail" ) ;
		this->changeIcon( icon ) ;
		this->setToolTip( icon,accountName,info ) ;

		/*
		 * done checking,restoring accounts from back up
		 */
		 m_accounts = m_accounts_backUp ;
	}else{
		/*
		 * No mail was found on the previous account,if there are more accounts,check them
		 */
		if( m_accounts.size() > 1 ){
			/*
			 * there are more accounts,remove the entry previously checked and check the next account
			 */
			m_accounts.remove( 0 ) ;
			this->checkMail( m_accounts.at( 0 ) ) ;
		}else{
			/*
			 * there are no more accounts and new mail not found in any of them
			 */
			
			this->setToolTip( QString( "qCheckGMail"),tr( "status" ),tr( "no new email found" ) ) ;
			this->changeIcon( QString( "qCheckGMail" ) ) ;

			/*
			 * done checking,restoring accounts from back up
			 */
			 m_accounts = m_accounts_backUp ;
		}
	}
}

void qCheckGMail::pauseCheckingMail( bool b )
{
	if( b ){
		this->stopTimer();
		this->setOverlayIconByName( QString( "qCheckGMail-GotMail" ) );
	}else{
		this->setOverlayIconByName( QString( "" ) );
		this->startTimer();
	}
}

void qCheckGMail::configurationWindow()
{
	configurationDialog * cfg = new configurationDialog( &m_wallet ) ;
	connect( cfg,SIGNAL( configurationDialogClosed() ),this,SLOT( configurationDialogClosed() ) ) ;
	cfg->ShowUI() ;
}

void qCheckGMail::configurationDialogClosed( void )
{
	if( m_wallet->isOpen() ){
		this->setUpAccounts();
		this->startTimer();
	}else{
		this->deleteKWallet();
	}
}

void qCheckGMail::deleteKWallet()
{
	m_wallet->deleteLater();
	m_wallet = 0 ;
}

void qCheckGMail::checkMail()
{
	if( m_gotCredentials ){
		/*
		* check for updates on the first account
		*/
		if( m_accounts.size() > 0 ){
			this->checkMail( m_accounts.at( 0 ) );
		}else{
			qDebug() << "BUGG!!,tried to check for mails when when there are no accounts configured" ;
		}
	}else{
		qDebug() << tr( "dont have credentials,(re)trying to open wallet" ) ;
		this->getAccountsInformation() ;
	}
}

void qCheckGMail::checkMail( const accounts& acc )
{
	QUrl url( acc.LastLabel() ) ;

	url.setUserName( acc.userName() ) ;
	url.setPassword( acc.passWord() ) ;

	QNetworkRequest rqt( url ) ;

	m_manager->get( rqt ) ;
}

void qCheckGMail::getAccountsInformation()
{
	m_wallet = KWallet::Wallet::openWallet( "qCheckGmail",0,KWallet::Wallet::Asynchronous ) ;
	connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
}

void qCheckGMail::walletOpened( bool opened )
{	
	if( m_wallet ){
		if( opened ){
			this->setUpAccounts();
		}else{
			this->walletNotOPenedError();
		}
		this->setTimerEvents();
		this->setTimer() ;
		this->startTimer();
	}else{
		qDebug() << "BUGG!!,walletOpened(): m_wallet is void" ;
	}
}

void qCheckGMail::setUpAccounts()
{
	if( !m_wallet ){
		qDebug() << "BUGG!!,setUpAccounts(): m_wallet is void" ;
		return ;
	}

	m_wallet->setFolder( m_wallet->PasswordFolder() ) ;

	QStringList userNames = m_wallet->entryList() ;

	if( userNames.size() == 0 ){
		/*
		 * wallet is empty,ask a user to configure it
		 */
		this->changeIcon( QString( "qCheckGMailError" ) ) ;
		this->setToolTip( QString( "qCheckGMailError" ),
				  tr( "error" ),
				  tr( "no account appear to be configured in the wallet" ) ) ;

		m_gotCredentials = false ;
	}else{
		int j = userNames.size() ;
		QString passWord ;
		QString name ;

		m_accounts_backUp.clear() ;

		for( int i = 0 ; i < j ; i++ ){
			name = userNames.at( i ) ;
			m_wallet->readPassword( name,passWord ) ;
			m_accounts_backUp.append( accounts( name,passWord ) ) ;
		}

		m_gotCredentials = true ;
		/*
		* initialize accounts
		*/
		m_accounts = m_accounts_backUp ;

		this->checkMail() ;
	}

	m_wallet->closeWallet( m_walletName,false ) ;
	this->deleteKWallet();
}

void qCheckGMail::walletNotOPenedError()
{
	qDebug() << "wallet not opened";
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
		l.append( m_accounts.at( i ).userName() ) ;
	}
	return l ;
}

void qCheckGMail::setTimer()
{
	m_interval = CHECK_UPDATE_INTERVAL ;
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
	tr( "another instance is already running,exiting this one" ) ;
	return 1 ;
}

bool qCheckGMail::autoStartEnabled()
{
	return true ;
}
