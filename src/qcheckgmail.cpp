/*
 *
 *  Copyright (c) 2013
 *  name : Francis Banyikwa
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
#include "util.hpp"

#include <string.h>
#include <utility>

#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

qCheckGMail::qCheckGMail( const qCheckGMail::args& args ) :
	m_manager( 15000 ),
	m_networkRequest( QUrl( "https://accounts.google.com/o/oauth2/token" ) ),
	m_qApp( args.app ),
	m_args( m_qApp.arguments() ),
	m_statusicon( m_settings )
{
	m_networkRequest.setRawHeader( "Content-Type","application/x-www-form-urlencoded" ) ;
}

qCheckGMail::~qCheckGMail()
{
}

void qCheckGMail::setTrayIconToVisible( bool showIcon )
{
	if( showIcon ){

		if( m_visibleIconState == "NeedsAttention" ){

			m_statusicon.setStatus( m_statusicon.NeedsAttention ) ;
			m_networkRequest.setRawHeader( "Host","accounts.google.com" ) ;

		}else if( m_visibleIconState == "Passive" ){

			m_statusicon.setStatus( m_statusicon.Passive ) ;

		}else if( m_visibleIconState == "Active" ){

			m_statusicon.setStatus( m_statusicon.Active ) ;
		}else{
			m_statusicon.setStatus( m_statusicon.NeedsAttention ) ;
		}
	}else{
		if( m_alwaysShowTrayIcon ){

			m_statusicon.setStatus( m_statusicon.Active ) ;
		}else{
			m_statusicon.setStatus( m_statusicon.Passive ) ;
		}
	}
}

void qCheckGMail::showToolTip( const QString& x,const QString& y,const QString& z )
{
	m_statusicon.setToolTip( x,y,z ) ;
}

void qCheckGMail::showPausedIcon( bool paused )
{
	if( paused ){

		m_statusicon.setIcon( m_pausedIcon ) ;
	}else{
		m_statusicon.setIcon( m_noEmailIcon ) ;
	}
}

void qCheckGMail::changeIcon( const QString& icon )
{
	m_statusicon.setIcon( icon ) ;
}

void qCheckGMail::changeIcon( const QString& icon,int count )
{
	if( m_displayEmailCount ){

		m_statusicon.setIcon( icon,count ) ;
	}
}

void qCheckGMail::start( const QByteArray& )
{
	if( m_args.contains( "-a" ) ){

		if( m_settings.autoStartEnabled() ){

			this->start() ;
		}else{
			m_qApp.exit( qCheckGMail::autoStartDisabled() ) ;
		}
	}else{
		this->start() ;
	}
}

void qCheckGMail::event( const QByteArray& )
{
}

void qCheckGMail::start()
{
	int j = m_args.size() ;

	for( int i = 0 ; i < j ; i++ ){

		if( m_args.at( i ) == "-p" ){

			if( i + 1 < j ){

				m_settings.setProfile( m_args.at( i + 1 ) ) ;

				break ;
			}
		}
	}

	m_statusicon.setCategory( m_statusicon.ApplicationStatus ) ;
	QCoreApplication::setApplicationName( "qCheckGMail" ) ;

	m_enableDebug	      = m_statusicon.enableDebug() ;
	m_reportOnAllAccounts = m_settings.reportOnAllAccounts() ;
	m_audioNotify	      = m_settings.audioNotify() ;
	m_interval	      = m_settings.getTimeFromConfigFile() ;
	m_newEmailIcon	      = m_settings.newEmailIcon() ;
	m_errorIcon	      = m_settings.errorIcon() ;
	m_noEmailIcon	      = m_settings.noEmailIcon() ;
	m_pausedIcon          = m_settings.pausedIcon() ;
	m_displayEmailCount   = m_settings.displayEmailCount() ;
	m_networkTimeOut      = m_settings.networkTimeOut() ;
	m_defaultApplication  = m_settings.defaultApplication() ;
	m_profileEmailList    = m_settings.profileEmailList() ;
	m_clientID	      = m_settings.clientID() ;
	m_clientSecret	      = m_settings.clientSecret() ;
	m_visibleIconState    = m_settings.visibleIconState() ;
	m_alwaysShowTrayIcon  = m_settings.alwaysShowTrayIcon() ;

	m_applicationIcon     = m_noEmailIcon ;

	m_numberOfAccounts  = 0 ;
	m_numberOfLabels    = 0 ;

	m_checkingMail      = false ;

	m_clickActions.onLeftClick = [ & ](){

		if( m_accounts.size() > 0 ){

			this->openMail( m_accounts.first() ) ;
		}else{
			this->openMail() ;
		}
	} ;

	m_statusicon.setIconClickedActions( m_clickActions ) ;

	this->changeIcon( m_errorIcon ) ;

	this->setTrayIconToVisible( true ) ;

	connect( &m_timer,&QTimer::timeout,this,[ this ](){ this->checkMail() ; },Qt::QueuedConnection ) ;

	m_timer.start( m_interval ) ;

	this->setLocalLanguage() ;
	this->addActionsToMenu() ;
	this->showToolTip( m_errorIcon,tr( "Status" ),tr( "Opening Wallet" ) ) ;
	this->getAccountsInfo() ;
}

static void _start_detached( bool debug,QString& exe,const QString& url )
{
	if( exe.isEmpty() ){

		return ;
	}

	if( exe == "browser" ){

		QDesktopServices::openUrl( QUrl( url ) ) ;
	}else{
		auto s = util::split( exe,' ' ) ;

		auto m = s.takeAt( 0 ) ;

		for( auto& it : s ){

			if( it == "%{url}" ){

				it = url ;

				break ;
			}
		}

		if( debug ){

			std::cout << "\"" + m.toStdString() + "\"" ;

			for( const auto& it : s ){

				std::cout << " \"" + it.toStdString() + "\"" ;
			}

			std::cout << std::endl ;
		}

		QProcess::startDetached( m,s ) ;
	}
}

void qCheckGMail::openMail( const accounts& acc )
{
	if( m_accounts.size() > 0 ){

		auto url = acc.defaultLabelUrl() ;

		if( url.endsWith( "/a/gmail.com/feed/atom/" ) ){

			url.truncate( url.size() - int( ( sizeof( "/a/gmail.com/feed/atom/" ) - 1 ) ) ) ;

		}else if( url.endsWith( "/feed/atom/" ) ){

			url.truncate( url.size() - int( ( sizeof( "/feed/atom/" ) - 1 ) ) ) ;
		}

		_start_detached( m_enableDebug,m_defaultApplication,url ) ;
	}else{
		_start_detached( m_enableDebug,m_defaultApplication,"https://mail.google.com/" ) ;
	}
}

void qCheckGMail::openMail()
{
	_start_detached( m_enableDebug,m_defaultApplication,"https://mail.google.com/" ) ;
}

void qCheckGMail::iconClicked()
{
	m_clickActions.onLeftClick() ;
}

void qCheckGMail::addActionsToMenu()
{
	auto cm = static_cast< void( qCheckGMail::* )( bool ) >( &qCheckGMail::checkMail ) ;

	util::connect( &QAction::triggered,cm,this,[ this ](){

	       return m_statusicon.getAction( tr( "Check Mail Now" ) ) ;
	}() ) ;

	util::connect( &QAction::toggled,&qCheckGMail::pauseCheckingMail,this,[ this ](){

		auto ac = m_statusicon.getAction( tr( "Pause Checking Mail" ) ) ;

		ac->setObjectName( "PauseCheckingMail" ) ;
		ac->setCheckable( true ) ;
		ac->setChecked( false ) ;

		return ac ;
	}() ) ;

	util::connect( &QAction::triggered,&qCheckGMail::configureAccounts,this,[ this ](){

		return m_statusicon.getAction( tr( "Configure Accounts" ) ) ;
	}() ) ;

	util::connect( &QAction::triggered,&qCheckGMail::configurePassWord,this,[ this ](){

		auto ac = m_statusicon.getAction( tr( "Configure Password" ) ) ;

		ac->setObjectName( "ConfigurePassword" ) ;
		ac->setEnabled( m_settings.usingInternalStorageSystem() ) ;

		return ac ;
	}() ) ;

	 util::connect( &QAction::triggered,&qCheckGMail::configurationoptionWindow,this,[ this ](){

		return m_statusicon.getAction( tr( "Configure Options" ) ) ;
	}() ) ;

	m_menu = m_statusicon.getMenu( tr( "Open Mail" ) ) ;

	connect( m_menu,&QMenu::triggered,[ this ]( QAction * ac ){

		for( const auto& it : m_accounts ){

			if( it.accountName() == ac->objectName() ){

				this->openMail( it ) ;
				break ;
			}
		}
	} ) ;

	connect( m_statusicon.getOGMenu(),&QMenu::aboutToShow,[ this ](){

		m_menu->clear() ;

		for( const auto& it : m_accounts ){

			auto ac = m_menu->addAction( it.accountName() ) ;
			ac->setObjectName( it.accountName() ) ;
		}
	} ) ;

	m_statusicon.addQuitAction() ;
}

QString qCheckGMail::displayName( const QString& label )
{
	const auto& account     = m_accounts.at( m_currentAccount ) ;
	const auto& accountName = account.accountName() ;

	if( label == "INBOX" ){

		return accountName ;
	}else{
		return QString( "%1/%2" ).arg( accountName,label ) ;
	}
}

struct emailInfo
{
	bool invalid ;
	QString labelName ;
	QString labelUnreadEmails ;
	QString totalMessages ;
} ;

static emailInfo _getEmailInfo( const QByteArray& json )
{
	if( json.isEmpty() ){

		return { true,{},{},{} } ;
	}else{
		auto m = QJsonDocument::fromJson( json ) ;

		auto a = m.object().value( "name" ).toString() ;
		auto b = QString::number( m.object().value( "messagesUnread" ).toInt() ) ;
		auto c = QString::number( m.object().value( "messagesTotal" ).toInt() ) ;

		return { false,a,b,c } ;
	}
}

void qCheckGMail::wrongAccountNameOrPassword()
{
	auto x = m_accounts.at( m_currentAccount ).accountName() ;
	auto e = tr( "%1 Account Has Wrong Username/Password Combination" ).arg( x ) ;

	this->changeIcon( m_errorIcon ) ;
	this->showToolTip( m_errorIcon,tr( "Account Related Error Was Detected" ),e ) ;
	this->doneCheckingMail() ;
}

static void _account_status( QString& status,const QString& displayName,const QString& mailCount )
{
	QString d_name = displayName ;

	if( d_name.size() >= 32 ){

		d_name.truncate( 29 ) ;
		d_name += "..." ;
	}else{
		//while( d_name.size() < 32 ){

		//	d_name += " " ;
		//}
	}

	QString e = [ & ](){

		if( mailCount.toInt() > 0 ){

			return "<b>%1 %2</b>" ;
		}else{
			return "%1 %2" ;
		}
	}() ;

	if( status == "<table>" ){

		status += e.arg( d_name,mailCount ) ;
	}else{
		status += "<br>" + e.arg( d_name,mailCount ) ;
	}
}

/*
 * This function goes through all accounts and give reports of all of their states
 */
void qCheckGMail::reportOnAllAccounts( int counter,const QByteArray& msg,qCheckGMail::networkStatus status )
{
	auto emailInfo = _getEmailInfo( msg ) ;

	if( emailInfo.invalid ){

		m_errorOccured = true ;

		_account_status( m_accountsStatus,this->displayName(),status.errorString() ) ;

	}else if( status.gmailError() ){

		auto& acc = *( m_accounts.data() + m_currentAccount ) ;

		if( acc.refreshToken().isEmpty() ){

			/*
			 * RefreshToken should never be empy
			 */

			m_errorOccured = true ;

			_account_status( m_accountsStatus,this->displayName(),"Status: Internal Error" ) ;
		}else{
			if( m_badAccessToken ){

				/*
				 * We had a bad token twice for the same account???
				 *
				 * Bail out to prevent an endless loop.
				 */

				m_errorOccured = true ;

				_account_status( m_accountsStatus,this->displayName(),status.errorString() ) ;
			}else{
				/*
				 * We will get here if:
				 * 1. The access token has expired
				 * 2. User revoked access to the account.
				 */

				m_badAccessToken = true ;

				acc.setAccessToken( QString() ) ;

				return this->checkMail( counter,acc,acc.labelUrlAt( m_currentLabel ) ) ;
			}
		}
	}else{
		if( status.success() ){

			const auto& mailCount = emailInfo.labelUnreadEmails ;

			auto mailCount_1 = mailCount.toInt() ;

			if( mailCount_1 == 0 ){

				_account_status( m_accountsStatus,this->displayName( emailInfo.labelName ),"0" ) ;
			}else{
				m_mailCount += mailCount_1 ;
				_account_status( m_accountsStatus,this->displayName( emailInfo.labelName ),mailCount ) ;
			}
		}else{
			m_errorOccured = true ;
			_account_status( m_accountsStatus,this->displayName(),status.errorString() ) ;
		}
	}

	/*
	 * done processing a label in an account,go to the next label if present
	 */
	m_currentLabel++ ;

	if( m_currentLabel < m_numberOfLabels ){

		/*
		 * account has more labels and we are at the next one and are about to go through it
		 */
		const auto& acc = m_accounts.at( m_currentAccount ) ;
		this->checkMail( counter,acc,acc.labelUrlAt( m_currentLabel ) ) ;
	}else{
		/*
		 * we are done processing an account,go to the next one if available
		 */
		m_currentAccount++ ;

		if( m_currentAccount < m_numberOfAccounts ){

			/*
			 * more accounts are configured and we are at the next one and are about to go through it
			 */
			this->checkMail( counter,m_accounts.at( m_currentAccount ) ) ;
		}else{
			/*
			 * done checking all labels on all accounts
			 */
			m_accountsStatus += "</table>" ;

			if( m_mailCount > 0 ){

				this->changeIcon( m_newEmailIcon,m_mailCount ) ;
				this->setTrayIconToVisible( true ) ;

				if( m_mailCount == 1 ){

					this->showToolTip( m_newEmailIcon,tr( "Found 1 New Email" ),m_accountsStatus ) ;
				}else{
					auto x = QString::number( m_mailCount ) ;
					this->showToolTip( m_newEmailIcon,tr( "Found %1 New Emails" ).arg( x ),m_accountsStatus ) ;
				}

				this->audioNotify() ;
			}else{
				this->changeIcon( m_noEmailIcon ) ;
				this->setTrayIconToVisible( false ) ;
				this->showToolTip( m_noEmailIcon,tr( "No New Email Found" ),m_accountsStatus ) ;
			}

			this->doneCheckingMail() ;
		}
	}
}

void qCheckGMail::audioNotify()
{
	if( m_accountUpdated && m_audioNotify ){

		m_statusicon.newEmailNotify() ;
	}
}

void qCheckGMail::doneCheckingMail()
{
	if( m_errorOccured ){

		this->changeIcon( m_errorIcon ) ;
	}
}

void qCheckGMail::pauseCheckingMail( bool pauseAction )
{
	this->showPausedIcon( pauseAction ) ;

	if( pauseAction ){

		this->stopTimer() ;
	}else{
		this->startTimer() ;

		this->checkMail() ;
	}
}

void qCheckGMail::configurationoptionWindow()
{
	class meaw : public configurationoptionsdialog::actions
	{
	public:
		meaw( qCheckGMail * g ) : m_parent( g )
		{
		}
		void configurationWindowClosed( int s ) override
		{
			m_parent->configurationWindowClosed( s ) ;
		}
		void enablePassWordChange( bool s ) override
		{
			m_parent->enablePassWordChange( s ) ;
		}
		void reportOnAllAccounts( bool s ) override
		{
			m_parent->reportOnAllAccounts( s ) ;
		}
		void audioNotify( bool s ) override
		{
			m_parent->audioNotify( s ) ;
		}
		void alwaysShowTrayIcon( bool s ) override
		{
			m_parent->alwaysShowTrayIcon( s ) ;
		}
	private:
		qCheckGMail * m_parent ;
	};

	configurationoptionsdialog::instance( this,m_settings,{ util::type_identity< meaw >(),this } ) ;
}

void qCheckGMail::enablePassWordChange( bool changeable )
{
	auto e = m_statusicon.getMenuActions() ;

	auto j = e.size() ;

	for( decltype( j ) i = 0 ; i < j ; i++ ){

		if( e.at( i )->objectName() == "ConfigurePassword" ){

			e.at( i )->setEnabled( changeable ) ;

			break ;
		}
	}
}

void qCheckGMail::configurationWindowClosed( int r )
{
	if( m_interval != r ){

		this->setTimer( r ) ;
	}
}

void qCheckGMail::audioNotify( bool audioNotify )
{
	m_audioNotify = audioNotify ;
}

void qCheckGMail::alwaysShowTrayIcon( bool e )
{
	m_alwaysShowTrayIcon = e ;

	if( e ){

		this->setTrayIconToVisible( false ) ;
	}else{
		if( m_mailCount == 0 ){

			m_statusicon.setStatus( m_statusicon.Passive ) ;
		}
	}
}

void qCheckGMail::reportOnAllAccounts( bool reportOnAllAccounts )
{
	m_reportOnAllAccounts = reportOnAllAccounts ;
}

void qCheckGMail::failedToCheckForNewEmail()
{
	/*
	 * We will get here if an attempt to check for email update is made while another attempt is already in progress.
	 * Mail checking usually takes a few seconds and hence the most likely reason to get here is if the network is down
	 * and the already in progress attempt is stuck somewhere in QNetworkAccessManager object.
	 */

	auto x = tr( "Network Problem Detected" ) ;
	auto msg_1 = tr( "Email Checking Is Taking Longer Than Expected." ) ;
	auto msg_2 = tr( "Recommending Restarting qCheckGMail If The Problem Persists" ) ;
	auto z = QString( "<table><tr><td>%1</td></tr><tr><td>%2</td></tr></table>" ).arg( msg_1,msg_2 ) ;

	this->changeIcon( m_errorIcon ) ;
	this->showToolTip( m_errorIcon,x,z ) ;
	this->setTrayIconToVisible( true ) ;
}

/*
 * This should be the only function that initiate email checking
 */
void qCheckGMail::checkMail( bool )
{
	if( m_numberOfAccounts > 0 ){

		m_accountsStatus  = "<table>" ;
		m_mailCount       = 0 ;
		m_currentAccount  = 0 ;
		m_accountUpdated  = false ;
		m_accountFailed   = false ;
		m_errorOccured    = false ;
		m_counter++ ;

		this->checkMail( m_counter,m_accounts.at( m_currentAccount ) ) ;
	}else{
		std::cout << tr( "Dont Have Credentials,(Re)Trying To Open Wallet" ) << std::endl ;
		this->getAccountsInfo() ;
	}
}

void qCheckGMail::checkMail( int counter,const accounts& acc )
{
	//m_manager.QtNAM().setNetworkAccessible( QNetworkAccessManager::Accessible ) ;

	m_badAccessToken = false ;
	m_currentLabel   = 0 ;
	m_numberOfLabels = acc.numberOfLabels() ;

	this->checkMail( counter,acc,acc.defaultLabelUrl() ) ;
}

static QString _parseJSON( const QByteArray& json,const char * key )
{
	auto m = QJsonDocument::fromJson( json ).object() ;

	return m.value( key ).toString() ;
}

void qCheckGMail::getAccessToken( int counter,
				  const accounts& acc,
				  const QString& refresh_token,
				  const QString& UrlLabel )
{
	m_manager.post( m_networkRequest,[ & ](){

		util::urlOpts opts ;

		opts.add( "client_id",m_clientID ) ;
		opts.add( "client_secret",m_clientSecret ) ;
		opts.add( "refresh_token",refresh_token ) ;
		opts.add( "grant_type","refresh_token" ) ;

		return opts.toUtf8() ;

	}(),[ UrlLabel,this,&acc,refresh_token,counter ]( const NetworkAccessManager::reply& reply ){

		if( reply.success() ){

			auto e = _parseJSON( reply.data(),"access_token" ) ;

			acc.setAccessToken( e ) ;

			QNetworkRequest request( QUrl( UrlLabel.toUtf8().constData() ) ) ;

			request.setRawHeader( "Authorization","Bearer " + e.toUtf8() ) ;

			this->networkAccess( counter,request ) ;
		}else{
			QNetworkRequest request( QUrl( UrlLabel.toUtf8().constData() ) ) ;

			this->networkAccess( counter,request ) ;
		}
	} ) ;
}

gmailauthorization::getAuth qCheckGMail::getAuthorization()
{
	class meaw : public gmailauthorization::authActions
	{
	public:
		meaw( qCheckGMail * g ) : m_parent( g )
		{
		}
		void operator()( const QString& authocode,gmailauthorization::AuthResult function ) override
		{
			m_parent->m_manager.post( m_parent->m_networkRequest,[ & ](){

				auto s = m_parent->m_settings.stringRunTimePortNumber() ;

				util::urlOpts opts ;

				opts.add( "client_id",m_parent->m_clientID ) ;
				opts.add( "client_secret",m_parent->m_clientSecret ) ;
				opts.add( "code",authocode ) ;
				opts.add( "grant_type","authorization_code" ) ;
				opts.add( "redirect_uri","http://127.0.0.1:" + s ) ;

				return opts.toUtf8() ;

			 }(),[ funct = std::move( function ) ]( const NetworkAccessManager::reply& reply ){

				if( reply.success() ){

					auto m = reply.data() ;

					funct( _parseJSON( m,"refresh_token" ),m ) ;
				}else{
					funct( {},{} ) ;
				}
			 } ) ;
		}
	private:
		qCheckGMail * m_parent ;
	};

	return { util::type_identity< meaw >(),this } ;
}

walletmanager::Wallet qCheckGMail::walletHandle()
{
	class meaw : public walletmanager::wallet
	{
	public:
		meaw( qCheckGMail * g ) : m_parent( g )
		{
		}
		void data( QVector< accounts >&& e ) override
		{
			m_parent->getAccountsInfo( std::move( e ) ) ;
		}
		void closed() override
		{
		}
	private:
		qCheckGMail * m_parent ;
	} ;

	return { util::type_identity< meaw >(),this } ;
}

struct GMailError
{
	bool hasError ;
	QString errorMsg ;
} ;

static GMailError _gmailError( const QByteArray& msg )
{
	auto obj = QJsonDocument::fromJson( msg ).object().value( "error" ) ;

	if( obj.isObject() ){

		auto arr = obj.toObject().value( "errors" ).toArray() ;

		if( arr.size() > 0 ){

			auto xbj = arr[ 0 ].toObject() ;

			auto msg = xbj.value( "message" ).toString() ;

			if( !msg.isEmpty() ){

				return { true,"Error: " + msg } ;
			}
		}

		return { true,"Error: Unknown GMail Error" } ;
	}else{
		return { false,{} } ;
	}
}

void qCheckGMail::networkAccess( int counter,const QNetworkRequest& request )
{
	m_manager.get( request,[ this,counter ]( const NetworkAccessManager::reply& reply ){

		if( counter != m_counter ){

			/*
			 * We will get here if there are more than one on going process of checking mail
			 * and we allow only the most recent one to proceed.
			 */
			std::cerr << "Expected counter to be\" " << m_counter << "\" but it is \"" << counter << "\"" << std::endl ;

			return ;
		}

		auto error = reply.error() ;

		if( error == QNetworkReply::OperationCanceledError ){

			return this->reportOnAllAccounts( counter,"","Error: Operation Cancelled" ) ;
		}

		auto content = reply.data() ;

		if( m_enableDebug ){

			std::cerr << content + "\n" << std::endl ;

			if( error != QNetworkReply::NetworkError::NoError ){

				std::cerr << reply.errorString() << std::endl ;

				std::cerr << error << std::endl ;
			}
		}

		if( reply.timeOut() ){

			this->reportOnAllAccounts( counter,content,"Error: Timeout" ) ;
		}else{
			auto err = _gmailError( content ) ;

			using qc = qCheckGMail::networkStatus::state ;

			if( err.hasError ){

				this->reportOnAllAccounts( counter,content,{ qc::gmailError,std::move( err.errorMsg ) } ) ;

			}else if( error == QNetworkReply::HostNotFoundError ){

				this->reportOnAllAccounts( counter,content,"Error: Host Not Found" ) ;

			}else if( error == QNetworkReply::NoError ){

				this->reportOnAllAccounts( counter,content,qc::success ) ;

			}else if( error == QNetworkReply::TimeoutError ){

				this->reportOnAllAccounts( counter,content,"Error: TimeOut" ) ;
			}else{
				this->reportOnAllAccounts( counter,content,"Error: " + reply.errorString() ) ;
			}
		}
	} ) ;
}

void qCheckGMail::getGMailAccountInfo( const QString& authocode,addaccount::GmailAccountInfo ginfo )
{
	m_manager.post( m_networkRequest,[ & ](){

		util::urlOpts opts ;

		opts.add( "client_id",m_clientID ) ;
		opts.add( "client_secret",m_clientSecret ) ;
		opts.add( "refresh_token",authocode ) ;
		opts.add( "grant_type","refresh_token" ) ;

		return opts.toUtf8() ;

	}(),[ this,ginfo = std::move( ginfo ) ]( const NetworkAccessManager::reply& reply ){

		if( reply.success() ){

			auto e = _parseJSON( reply.data(),"access_token" ) ;

			this->getLabels( e,std::move( ginfo ) ) ;
		}else{
			this->getLabels( {},std::move( ginfo ) ) ;
		}
	} ) ;
}

void qCheckGMail::getGMailAccountInfo( const QByteArray& accName,addaccount::GmailAccountInfo ginfo )
{
	for( const auto& it : m_accounts ){

		if( it.accountName() ==accName ){

			this->getLabels( it.accessToken(),std::move( ginfo ) ) ;

			break ;
		}
	}
}

void qCheckGMail::getLabels( const QString& accessToken,addaccount::GmailAccountInfo ginfo )
{
	if( accessToken.isEmpty() ){

		return ginfo( {} ) ;
	}

	QNetworkRequest r( QUrl( "https://gmail.googleapis.com/gmail/v1/users/me/labels" ) ) ;
	r.setRawHeader( "Authorization","Bearer " + accessToken.toUtf8() ) ;

	this->m_manager.get( r,[ ginfo = std::move( ginfo ) ]( const NetworkAccessManager::reply& reply ){

		if( reply.success() ){

			auto ss = reply.data() ;

			const auto arr = QJsonDocument::fromJson( ss ).object().value( "labels" ).toArray() ;

			addaccount::labels labels ;

			for( const auto& it : arr ){

				auto obj = it.toObject() ;
				auto id = obj.value( "id" ).toString() ;
				auto name = obj.value( "name" ).toString() ;

				labels.entries.append( { name,id } ) ;
			}

			ginfo( std::move( labels ) ) ;
		}else{
			ginfo( {} ) ;
		}
	} ) ;
}

void qCheckGMail::checkMail( int counter,const accounts& acc,const QString& UrlLabel )
{
	const auto& accessToken = acc.accessToken() ;

	if( accessToken.isEmpty() ){
		/*
		 * We will get here when we are running for the first time after startup
		 * or if an access token has expired.
		 */
		this->getAccessToken( counter,acc,acc.refreshToken(),UrlLabel ) ;
	}else{
		QNetworkRequest s( QUrl( UrlLabel.toUtf8().constData() ) ) ;

		s.setRawHeader( "Authorization","Bearer " + accessToken.toUtf8() ) ;

		this->networkAccess( counter,s ) ;
	}
}

void qCheckGMail::getAccountsInfo( QVector< accounts >&& acc )
{
	m_accounts.clear() ;

	if( m_profileEmailList.isEmpty() ){

		m_accounts = std::move( acc ) ;
	}else{
		for( const auto& it : acc ){

			if( m_profileEmailList.contains( it.accountName() ) ){

				m_accounts.append( it ) ;
			}
		}
	}

	m_numberOfAccounts = m_accounts.size() ;

	if( m_numberOfAccounts > 0 ){

		this->showToolTip( m_errorIcon,tr( "Status" ),QString() ) ;
		this->checkMail() ;
	}else{
		/*
		 * wallet is empty,warn the user about it
		 */
		this->noAccountConfigured() ;
	}
}

QString qCheckGMail::defaultApplication()
{
	return m_defaultApplication ;
}

void qCheckGMail::configureAccounts()
{
	class meaw : public addaccount::gMailInfo
	{
	public:
		meaw( qCheckGMail * m ) : m_parent( m )
		{
		}
		void operator()( const QString& authocode,addaccount::GmailAccountInfo returnEmail ) override
		{
			m_parent->getGMailAccountInfo( authocode,std::move( returnEmail ) ) ;
		}
		void operator()( const QByteArray& accountName,addaccount::GmailAccountInfo returnEmail ) override
		{
			m_parent->getGMailAccountInfo( accountName,std::move( returnEmail ) ) ;
		}
	private:
		qCheckGMail * m_parent ;
	};

	walletmanager::instance( m_applicationIcon,
				 m_settings,
				 this->walletHandle(),
				 this->getAuthorization(),
				 { util::type_identity< meaw >(),this } ).ShowUI() ;
}

void qCheckGMail::configurePassWord()
{
	walletmanager::instance( m_applicationIcon,m_settings ).changeWalletPassword() ;
}

void qCheckGMail::getAccountsInfo()
{
	walletmanager::instance( this->walletHandle(),m_settings ).getAccounts() ;
}

void qCheckGMail::noAccountConfigured()
{
	this->changeIcon( m_errorIcon ) ;
	this->showToolTip( m_errorIcon,
			   tr( "Account Related Error Was Detected" ),
			   tr( "No Account Appear To Be Configured In The Wallet" ) ) ;
}

void qCheckGMail::setLocalLanguage()
{
	auto lang     = m_settings.localLanguage() ;
	auto langPath = m_settings.localLanguagePath() ;

	auto r = lang.toLatin1() ;

	if( r == "english_US" ){

		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
	       QCoreApplication::installTranslator( [ & ](){

			auto e = new QTranslator( this ) ;

			e->load( r.constData(),langPath + "/translations.qm/" ) ;

			return e ;
		}() ) ;

		QCoreApplication::installTranslator( [ & ](){

		       auto e = new QTranslator( this ) ;

		       e->load( r.constData(),langPath + "/lxqt_wallet/translations.qm/" ) ;

		       return e ;
		}() ) ;
	}
}

void qCheckGMail::setLocalLanguage( QCoreApplication& qapp,QTranslator * translator )
{
	settings s ;

	auto lang     = s.localLanguage() ;
	auto langPath = s.localLanguagePath() ;

	auto r = lang.toLatin1() ;

	if( r == "english_US" ){

		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath + "/translations.qm/" ) ;
		qapp.installTranslator( translator ) ;
	}
}

void qCheckGMail::setTimer()
{
	m_interval = m_settings.getTimeFromConfigFile() ;
}

void qCheckGMail::setTimer( int time )
{
	m_interval = time ;
	this->startTimer() ;
}

void qCheckGMail::startTimer()
{
	auto ac = m_statusicon.getMenuActions() ;

	auto j = ac.size() ;

	for( decltype( j ) i = 0 ; i < j ; i++ ){

		if( ac.at( i )->objectName() == "PauseCheckingMail" ){

			if( !ac.at( i )->isChecked() ){

				m_timer.stop() ;
				m_timer.start( m_interval ) ;
			}

			break ;
		}
	}
}

void qCheckGMail::stopTimer()
{
	m_timer.stop() ;
}

int qCheckGMail::instanceAlreadyRunning()
{
	settings s ;

	auto lang = s.localLanguage() ;
	auto r = lang.toLatin1() ;

	if( r == "english_US" ){

		/*
		 *english_US language,its the default and hence dont load anything
		 */

		qDebug() << tr( "another instance is already running,exiting this one" ) ;
	}else{
		const char * x[ 2 ] = { "qCheckGMail",nullptr } ;

		int e = 1 ;

		QCoreApplication qapp( e,const_cast< char ** >( x ) ) ;

		auto langPath = s.localLanguagePath() ;

		QTranslator translator ;

		translator.load( r.constData(),langPath + "/translations.qm/" ) ;

		qapp.installTranslator( &translator ) ;

		qDebug() << tr( "another instance is already running,exiting this one" ) ;
	}

	return 1 ;
}

int qCheckGMail::autoStartDisabled()
{
	settings s ;

	auto lang = s.localLanguage() ;
	auto r = lang.toLatin1() ;

	if( r == "english_US" ){

		/*
		 *english_US language,its the default and hence dont load anything
		 */

		qDebug() << tr( "Autostart Disabled,Exiting This One" ) ;
	}else{
		const char * x[ 2 ] = { "qCheckGMail",nullptr } ;

		int e = 1 ;

		QCoreApplication qapp( e,const_cast< char ** >( x ) ) ;

		QString langPath = s.localLanguagePath() ;

		QTranslator translator ;

		translator.load( r.constData(),langPath + "/translations.qm/" ) ;

		qapp.installTranslator( &translator ) ;

		std::cerr << tr( "Autostart Disabled,Exiting This One" ) << std::endl ;
	}

	return 1 ;
}

bool qCheckGMail::autoStartEnabled()
{
	return settings().autoStartEnabled() ;
}
