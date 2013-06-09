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

#define DEBUG 0

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

	this->setTimer() ;
	this->setTimerEvents();
	this->startTimer();
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
#if DEBUG
	qDebug() << "\n" << msg;
#endif
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

	int count = mails.toInt() ;

	if( count > 0 ){

		QString info ;

		if( count == 1 ){
			info = tr( "1 email is waiting for you" ) ;
		}else{
			info = tr( "%2 emails are waiting for you" ).arg( mails ) ;
		}

		this->setStatus( KStatusNotifierItem::NeedsAttention ) ;
		QString icon = QString( "qCheckGMail-GotMail" ) ;
		this->changeIcon( icon ) ;

		QString x = m_labelUrl.split( "/" ).last() ;
		QString accountName ;

		if( x.isEmpty() ){
			accountName = m_accountName ;
		}else{
			accountName = m_accountName + QString( "/" ) + x ;
		}

		this->setToolTip( icon,accountName,info ) ;

		/*
		 * done checking,restoring accounts from back up
		 */
		 m_accounts = m_accounts_backUp ;
		 m_labelUrls.clear() ;
	}else{
		if( m_labelUrls.size() > 0 ){
			/*
			 * account has more labels to go through,go through them
			 */
			QString label = m_labelUrls.at( 0 ) ;

			m_labelUrls.removeAt( 0 ) ; //remve the label we are going to process next
			this->checkMail( m_accounts.at( 0 ).userName(),m_accounts.at( 0 ).passWord(),label ) ;
		}else{
			/*
			 * No mail was found on the previous account,if there are more accounts,check them
			 */
			if( m_accounts.size() > 1 ){
				/*
				 * there are more accounts,remove the entry previously checked and check the next account
				 */
				m_accounts.remove( 0 ) ;
				m_labelUrls.clear() ;
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
				 m_labelUrls.clear() ;
			}
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
	this->stopTimer();
	configurationDialog * cfg = new configurationDialog( &m_wallet ) ;
	connect( cfg,SIGNAL( configurationDialogClosed() ),this,SLOT( configurationDialogClosed() ) ) ;
	cfg->ShowUI() ;
}

void qCheckGMail::configurationDialogClosed( void )
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
	m_labelUrls = acc.LabelUrls() ;
	m_labelUrls.removeAt( 0 ) ; // remove the first default label
	this->checkMail( acc.userName(),acc.passWord(),acc.defaultLabel() ) ;
}

void qCheckGMail::checkMail( const QString& userName,const QString& password,const QString& label )
{
	m_accountName = userName ;
	m_labelUrl    = label    ;

	QUrl url( m_labelUrl ) ;

	url.setUserName( m_accountName ) ;
	url.setPassword( password ) ;

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
		m_accounts_backUp = configurationDialog::getAccounts( m_wallet ) ;

		m_gotCredentials = m_accounts_backUp.size() > 0 ;

		if( m_gotCredentials ){
			m_accounts = m_accounts_backUp ;
			this->checkMail( m_accounts.at( 0 ) ) ;
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
	const char * x = "qCheckGMailError" ;
	const char * y = "error" ;
	const char * z = "no account appear to be configured in the wallet" ;
	this->changeIcon( QString( x ) ) ;
	this->setToolTip( QString( x ),tr( y ),tr( z ) ) ;
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
