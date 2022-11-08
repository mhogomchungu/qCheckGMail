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
#include "icon_file_path.h"
#include "utils/threads.hpp"
#include "utils/qtimer.hpp"

#include <string.h>
#include <utility>

#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

static auto a = "org.freedesktop.Notifications" ;
static auto b = "/org/freedesktop/Notifications" ;
static auto c = "org.freedesktop.Notifications" ;

qCheckGMail::qCheckGMail( const qCheckGMail::args& args ) :
	m_networkTimeOut( m_settings.networkTimeOut() ),
	m_manager( m_networkTimeOut ),
	m_networkRequest( QUrl( m_auth ) ),
	m_qApp( args.app ),
	m_args( m_qApp.arguments() ),
	m_logWindow( m_settings,m_args.contains( "-d" ) ),
	m_statusicon( m_settings,this->clickActions() ),
	m_dbusConnection( QDBusConnection::sessionBus() ),
	m_dbusInterface( a,b,c,m_dbusConnection )
{
	m_networkRequest.setRawHeader( "Content-Type","application/x-www-form-urlencoded" ) ;

	m_dbusConnection.connect( a,b,c,"NotificationClosed",
				  this,SLOT( notificationClosed( quint32,quint32 ) ) ) ;

	m_dbusConnection.connect( a,b,c,"ActionInvoked",
				  this,SLOT( actionInvoked( quint32,QString ) ) ) ;

	auto m = m_dbusInterface.call( "GetCapabilities" ).arguments() ;

	if( m.size() > 0 ){

		auto s = m.first().toStringList() ;

		m_notificationSupportshyperlinks = s.contains( "body-hyperlinks" ) ;

		if( s.size() > 0 ){

			auto e = "Pop Notification Has Following Capabilities:\n" ;

			m_logWindow.update( logWindow::TYPE::INFO,e + s.join( ", " ),true ) ;
		}else{
			auto e = "Pop Notification Has No Capabilities" ;

			m_logWindow.update( logWindow::TYPE::INFO,e,true ) ;
		}
	}
}

qCheckGMail::~qCheckGMail()
{
}

void qCheckGMail::setTrayIconToVisible( bool showIcon )
{
	if( showIcon ){

		if( m_visibleIconState == "NeedsAttention" ){

			m_statusicon.setStatus( m_statusicon.NeedsAttention ) ;

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

void qCheckGMail::showToolTip( const QString& iconName,
			       const QString& title,
			       const QString& subTitle )
{
	m_statusicon.setToolTip( iconName,title,subTitle ) ;
}

void qCheckGMail::showToolTip( const QString& iconName,
			       const QString& title,
			       std::vector< qCheckGMail::accountsStatus >& subTitle )
{
	if( subTitle.size() > 0 ){

		QString m = "<table>" ;

		auto iter = subTitle.begin() ;

		const auto& e = *iter ;

		if( e.txt.endsWith( " 0" ) ){

			m += e.txt ;
		}else{
			m += "<b>" + e.txt + "</b>" ;
		}

		iter++ ;

		for( ; iter != subTitle.end() ; iter++ ){

			const auto& e = *iter ;

			if( e.txt.endsWith( " 0" ) ){

				m += "<br>" + e.txt ;
			}else{
				m += "<br><b>" + e.txt + "</b>" ;
			}
		}

		m += "</table>" ;

		m_statusicon.setToolTip( iconName,title,m ) ;
	}
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

void qCheckGMail::hasEvent( const QByteArray& )
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

	m_retryWaitTime       = m_settings.waitTimeBeforeRetrying() ;
	m_notificationTimeOut = m_settings.notificationTimeOut() ;
	m_visualNotify        = m_settings.visualNotify() ;
	m_audioNotify	      = m_settings.audioNotify() ;
	m_interval	      = m_settings.checkForUpdatesInterval() ;
	m_newEmailIcon	      = m_settings.newEmailIcon() ;
	m_errorIcon	      = m_settings.errorIcon() ;
	m_checkingMailIcon    = m_settings.checkingMailIcon() ;
	m_noEmailIcon	      = m_settings.noEmailIcon() ;
	m_pausedIcon          = m_settings.pausedIcon() ;
	m_displayEmailCount   = m_settings.displayEmailCount() ;
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

	this->changeIcon( m_errorIcon ) ;

	this->setTrayIconToVisible( true ) ;

	auto ff = [ this ](){ this->checkMail() ; } ;

	connect( &m_timer,&QTimer::timeout,this,std::move( ff ),Qt::QueuedConnection ) ;

	m_timer.start( m_interval ) ;

	this->setLocalLanguage() ;
	this->addActionsToMenu() ;
	this->showToolTip( m_errorIcon,tr( "Status" ),tr( "Opening Wallet" ) ) ;
	this->getAccountsInfo() ;
}

void qCheckGMail::openMail()
{
	this->openInbox( "https://mail.google.com/mail/u/0/#inbox" ) ;
}

void qCheckGMail::openInbox( const QString& url )
{
	if( !m_defaultApplication.isEmpty() ){

		if( m_defaultApplication == "browser" ){

			QDesktopServices::openUrl( QUrl( url ) ) ;
		}else{
			auto e = m_defaultApplication ;

			e.replace( "%{url}",url ) ;

			auto s = util::splitPreserveQuotes( e ) ;

			auto m = s.takeAt( 0 ) ;

			e = "Running Command\n" + e + " " + s.join( " " ) ;

			m_logWindow.update( logWindow::TYPE::INFO,e ) ;

			QProcess::startDetached( m,s ) ;
		}
	}
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

	auto tgr = &QAction::triggered ;

	util::connect( tgr,&qCheckGMail::configureAccounts,this,[ this ](){

		return m_statusicon.getAction( tr( "Configure Accounts" ) ) ;
	}() ) ;

	util::connect( tgr,&qCheckGMail::configurePassWord,this,[ this ](){

		auto ac = m_statusicon.getAction( tr( "Configure Password" ) ) ;

		ac->setObjectName( "ConfigurePassword" ) ;
		ac->setEnabled( m_settings.usingInternalStorageSystem() ) ;

		return ac ;
	}() ) ;

	util::connect( tgr,&qCheckGMail::configurationoptionWindow,this,[ this ](){

		return m_statusicon.getAction( tr( "Configure Options" ) ) ;
	}() ) ;

	util::connect( tgr,&qCheckGMail::showLogWindow,this,[ this ](){

		return m_statusicon.getAction( tr( "Show Log Window" ) ) ;
	}() ) ;

	m_menu = m_statusicon.getMenu( tr( "Open Mail" ) ) ;

	connect( m_menu,&QMenu::triggered,[ this ]( QAction * ac ){

		for( const auto& it : m_accounts ){

			if( it.accountName() == ac->objectName() ){

				auto m = ac->objectName() ;

				auto u = "https://mail.google.com/mail/u/?authuser=" ;

				if( m.endsWith( "@gmail.com" ) ){

					this->openInbox( u + m ) ;
				}else{
					this->openInbox( u + m + "@gmail.com" ) ;
				}

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

void qCheckGMail::showLogWindow()
{
	m_logWindow.Show() ;
}

void qCheckGMail::logPOST( const util::urlOpts& e )
{
	auto m = "POST\n" + m_auth + "\n" + e.toUtf8() + "\n" ;

	m_logWindow.update( logWindow::TYPE::REQUEST,m ) ;
}

qCheckGMail::errMessage qCheckGMail::errorMessage( const utils::network::reply& reply )
{
	auto error = reply.error() ;

	auto code = "Error " + QString::number( static_cast< int >( error ) ) + ": " ;

	if( error == QNetworkReply::OperationCanceledError ){

		return { tr( "Operation Cancelled" ),code + "Operation Cancelled" } ;

	}else if( error == QNetworkReply::HostNotFoundError ){

		return { tr( "Host Not Found" ),code + "Host Not Found" } ;

	}else if( error == QNetworkReply::TimeoutError ){

		return { tr( "Network TimeOut" ),code + "Network TimeOut" } ;
	}else{
		auto err = reply.errorString() ;

		return { err,code + err } ;
	}
}

statusicon::clickActions qCheckGMail::clickActions()
{
	class meaw : public statusicon::clickActionsInterface
	{
	public:
		meaw( qCheckGMail& e ) : m_parent( e )
		{
		}
		void onLeftClick() const override
		{
			m_parent.openMail() ;
		}
		void onRightClick() const override
		{
		}
		void onMiddleClick() const override
		{
		}
		void onDoubleClick() const override
		{
		}
	private:
		qCheckGMail& m_parent ;
	} ;

	return { util::type_identity< meaw >(),*this } ;
}

QString qCheckGMail::displayName( const QString& label )
{
	const auto& account     = m_accounts.at( m_currentAccount ) ;
	const auto& accountName = account.accountName() ;

	if( label.isEmpty() ){

		const auto& m = account.nameUiAt( m_currentLabel ) ;

		if( m.isEmpty() || m == "INBOX" ){

			return accountName ;
		}else{
			return QString( "%1/%2" ).arg( accountName,m ) ;
		}

	}else if( label == "INBOX" ){

		return accountName ;
	}else{
		return QString( "%1/%2" ).arg( accountName,label ) ;
	}
}

struct emailInfo
{
	QString labelName ;
	QString labelUnreadEmails ;
	QString totalMessages ;
} ;

static emailInfo _getEmailInfo( const QByteArray& json )
{
	auto m = QJsonDocument::fromJson( json ).object() ;

	auto a = m.value( "name" ).toString() ;
	auto b = QString::number( m.value( "messagesUnread" ).toInt() ) ;
	auto c = QString::number( m.value( "messagesTotal" ).toInt() ) ;

	return { a,b,c } ;
}

/*
 * This function goes through all accounts and give reports of all of their states
 */
void qCheckGMail::updateUi( int counter,
			    const QByteArray& msg,
			    qCheckGMail::result result )
{
	const auto& acc = m_accounts[ m_currentAccount ] ;

	auto _account_status = []( std::vector< qCheckGMail::accountsStatus >& status,
				   const QString& displayName,
				   const QString& mailCount,
				   const QString& accName,
				   bool success ){
		if( displayName.size() >= 32 ){

			QString d_name = displayName ;

			d_name.truncate( 29 ) ;
			d_name += "..." ;

			status.emplace_back( success,d_name + " " + mailCount,accName ) ;
		}else{
			status.emplace_back( success,displayName + " " + mailCount,accName ) ;
		}
	} ;

	if( result.success() ){

		if( msg.isEmpty() ){

			/*
			 * We should not get here because we do not expect to not
			 * get any data from gmail.
			 */

			m_errorOccured = true ;
			_account_status( m_accountsStatus,
					 this->displayName(),
					 result.errorString(),
					 acc.accountName(),
					 false ) ;
		}else{
			auto emailInfo = _getEmailInfo( msg ) ;

			const auto& mailCount = emailInfo.labelUnreadEmails ;

			auto mailCount_1 = mailCount.toInt() ;

			if( mailCount_1 == 0 ){

				_account_status( m_accountsStatus,
						 this->displayName( emailInfo.labelName ),
						 "0",
						 acc.accountName(),
						 true ) ;
			}else{
				m_mailCount += mailCount_1 ;
				_account_status( m_accountsStatus,
						 this->displayName( emailInfo.labelName ),
						 mailCount,
						 acc.accountName(),
						 true ) ;
			}
		}

	}else if( result.gmailError() ){

		if( result.gmailErrorOfUnauthenticated() ){

			if( m_badAccessToken ){

				/*
				 * We had a bad token twice for the same account???
				 *
				 * Bail out to prevent an endless loop.
				 */

				m_errorOccured = true ;
				_account_status( m_accountsStatus,
						 this->displayName(),
						 result.errorString(),
						 acc.accountName(),
						 false ) ;
			}else{
				m_badAccessToken = true ;

				auto& acc = m_accounts[ m_currentAccount ] ;

				acc.setAccessToken( QString() ) ;

				this->checkMail( counter,acc,acc.labelUrlAt( m_currentLabel ),false ) ;

				return ;
			}
		}else{
			m_errorOccured = true ;
			_account_status( m_accountsStatus,
					 this->displayName(),
					 result.errorString(),
					 acc.accountName(),
					 false ) ;
		}
	}else{
		m_errorOccured = true ;
		_account_status( m_accountsStatus,
				 this->displayName(),
				 result.errorString(),
				 acc.accountName(),
				 false ) ;
	}

	/*
	 * done processing a label in an account,go to the next label if present
	 */
	m_currentLabel++ ;

	if( m_currentLabel < m_numberOfLabels ){

		/*
		 * account has more labels and we are at the next one and are
		 * about to go through it
		 */
		const auto& acc = m_accounts.at( m_currentAccount ) ;
		this->checkMail( counter,acc,acc.labelUrlAt( m_currentLabel ),false ) ;
	}else{
		/*
		 * we are done processing an account,go to the next one if available
		 */
		m_currentAccount++ ;

		if( m_currentAccount < m_numberOfAccounts ){

			/*
			 * more accounts are configured and we are at the next one and
			 * are about to go through it
			 */
			this->checkMail( counter,m_accounts.at( m_currentAccount ) ) ;
		}else{
			/*
			 * done checking all labels on all accounts
			 */ ;

			if( m_mailCount > 0 ){

				this->changeIcon( m_newEmailIcon,m_mailCount ) ;
				this->setTrayIconToVisible( true ) ;

				if( m_mailCount == 1 ){

					this->showToolTip( m_newEmailIcon,
							   tr( "Found 1 New Email" ),
							   m_accountsStatus ) ;
				}else{
					auto x = QString::number( m_mailCount ) ;
					auto m = tr( "Found %1 New Emails" ).arg( x ) ;
					this->showToolTip( m_newEmailIcon,
							   m,
							   m_accountsStatus ) ;
				}

				if( m_audioNotify ){

					this->audioNotify() ;
				}

				if( m_visualNotify ){

					utils::qthread::run( this,&qCheckGMail::visualNotify ) ;
				}
			}else{
				this->changeIcon( m_noEmailIcon ) ;
				this->setTrayIconToVisible( false ) ;
				this->showToolTip( m_noEmailIcon,
						   tr( "No New Email Found" ),
						   m_accountsStatus ) ;
			}

			this->doneCheckingMail() ;
		}
	}
}

void qCheckGMail::audioNotify()
{
	auto m = util::splitPreserveQuotes( m_settings.audioPlayer() ) ;

	if( m.size() > 0 ){

		auto exe = m.takeFirst() ;

		m.append( AUDIO_NOTIFY_FILE ) ;

		QProcess::startDetached( exe,m ) ;
	}
}

void qCheckGMail::visualNotify()
{
	if( m_accountsStatus.empty() ){

		return ;
	}

	QStringList l ;
	QVariantMap mm ;

	mm.insert( "image-path",ICON_FILE_PATH ) ;
	mm.insert( "category","email.arrived" ) ;
	mm.insert( "desktop-entry","qCheckGMail" ) ;
	mm.insert( "suppress-sound",true ) ;
	mm.insert( "urgency",1 ) ;

	auto x = QString::number( m_mailCount ) ;
	auto m = tr( "Found %1 New Emails" ).arg( x ) ;

	QString e ;

	if( m_notificationSupportshyperlinks ){

		for( const auto& m : m_accountsStatus ){

			if( m.success && !m.txt.endsWith( " 0" ) ){

				auto s = m.accName ;

				if( !s.endsWith( "@gmail.com" ) ){

					 s += "@gmail.com" ;
				}

				auto mm = "\n<a href=\"https://mail.google.com/mail/u/?authuser=" ;

				e += mm + s + "\">" + m.txt + "</a>" ;
			}else{
				e += "\n" + m.txt ;
			}
		}
	}else{
		for( const auto& m : m_accountsStatus ){

			e += "\n" + m.txt ;
		}
	}

	e = e.mid( 1 ) ;

	auto a = static_cast< qint32 >( m_notificationTimeOut ) ;
	auto aa = "qCheckGMail" ;

	if( !m_notificationSupportshyperlinks ){

		l.append( "default" ) ;
		l.append( tr( "Open Default Inbox" ) ) ;
	}

	auto result = m_dbusInterface.call( "Notify",aa,m_dbusId,"",m,e,l,mm,a ) ;

	auto s = result.arguments() ;

	if( s.size() > 0 ){

		m_dbusId = s.at( 0 ).toUInt() ;
	}else{
		m_dbusId = 0 ;
	}
}

void qCheckGMail::actionInvoked( quint32 u,QString )
{
	if( !m_notificationSupportshyperlinks ){

		if( u == m_dbusId ){

			this->openMail() ;
		}
	}
}

void qCheckGMail::notificationClosed( quint32 id,quint32 reason )
{
	Q_UNUSED( reason )

	if( id == m_dbusId ){

		m_dbusId = 0 ;
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
		meaw( qCheckGMail& e ) : m_parent( e )
		{
		}
		void configurationWindowClosed( int s ) override
		{
			m_parent.configurationWindowClosed( s ) ;
		}
		void enablePassWordChange( bool s ) override
		{
			m_parent.enablePassWordChange( s ) ;
		}
		void audioNotify( bool s ) override
		{
			m_parent.audioNotify( s ) ;
		}
		void visualNotify( bool s ) override
		{
			m_parent.visuallyNotify( s ) ;
		}
		void alwaysShowTrayIcon( bool s ) override
		{
			m_parent.alwaysShowTrayIcon( s ) ;
		}
	private:
		qCheckGMail& m_parent ;
	};

	configurationoptionsdialog::instance( this,m_settings,{ util::type_identity< meaw >(),*this } ) ;
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

void qCheckGMail::visuallyNotify( bool visualNotify )
{
	m_visualNotify = visualNotify ;
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

/*
 * This should be the only function that initiate email checking
 */
void qCheckGMail::checkMail()
{
	if( m_numberOfAccounts > 0 ){

		m_accountsStatus.clear() ;

		m_mailCount       = 0 ;
		m_currentAccount  = 0 ;
		m_accountFailed   = false ;
		m_errorOccured    = false ;
		m_counter++ ;

		if( m_checkingMailIcon.isEmpty() ){

			this->showToolTip( m_errorIcon,
					   "...",
					   tr( "Checking For Email Updates" ) ) ;
		}else{
			this->changeIcon( m_checkingMailIcon ) ;

			this->showToolTip( m_checkingMailIcon,
					   "...",
					   tr( "Checking For Email Updates" ) ) ;
		}

		this->checkMail( m_counter,m_accounts.at( m_currentAccount ) ) ;
	}else{
		auto m = "Dont Have Credentials,(Re)Trying To Open Wallet" ;

		m_logWindow.update( logWindow::TYPE::INFO,m,true ) ;

		this->getAccountsInfo() ;
	}
}

void qCheckGMail::checkMail( bool )
{
	this->checkMail() ;
}

void qCheckGMail::checkMail( int counter,const accounts& acc )
{
	//m_manager.QtNAM().setNetworkAccessible( QNetworkAccessManager::Accessible ) ;

	m_badAccessToken = false ;
	m_currentLabel   = 0 ;
	m_numberOfLabels = acc.numberOfLabels() ;

	this->checkMail( counter,acc,acc.defaultLabelUrl(),false ) ;
}

static QByteArray _hideSecret( const QByteArray& json )
{
	auto m = QJsonDocument::fromJson( json ).object() ;

	if( m.contains( "access_token" ) ){

		m.insert( "access_token","$ACCESS_TOKEN" ) ;
	}

	if( m.contains( "refresh_token" ) ){

		m.insert( "access_token","$REFRESH_TOKEN" ) ;
	}

	return QJsonDocument( m ).toJson( QJsonDocument::JsonFormat::Indented ) ;
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

		util::urlOpts opts1 ;

		opts1.add( "client_id","$CLIENT_ID" ) ;
		opts1.add( "client_secret","$CLIENT_SECRET" ) ;

		if( refresh_token.isEmpty() ){

			opts1.add( "refresh_token","$EMPTY_REFRESH_TOKEN" ) ;
		}else{
			opts1.add( "refresh_token","$REFRESH_TOKEN" ) ;
		}

		this->logPOST( opts1 ) ;

		return opts.toUtf8() ;

	}(),[ UrlLabel,this,&acc,refresh_token,counter ]( const utils::network::reply& reply ){

		if( reply.success() ){

			auto data = reply.data() ;

			m_logWindow.update( logWindow::TYPE::RESPONCE,_hideSecret( data ) ) ;

			auto e = _parseJSON( data,"access_token" ) ;

			if( e.isEmpty() ){

				if( data.isEmpty() ){

					auto m = "qCheckGMail: Empty Responce Received Unexectedly" ;

					m_logWindow.update( logWindow::TYPE::RESPONCE,m,true ) ;
				}else{
					QString m = "qCheckGMail: Below JSON Data Was Expected" ;
					m+= " To Contain \"access_token\"  Key But It Does Not\n" ;

					m_logWindow.update( logWindow::TYPE::RESPONCE,
							    m + _hideSecret( data ),
							    true ) ;
				}

				this->updateUi( counter,{},tr( "Unexpected Data Received" ) ) ;
			}else{
				acc.setAccessToken( e ) ;

				auto url = UrlLabel.toUtf8() ;

				QNetworkRequest request( QUrl( url.constData() ) ) ;

				request.setRawHeader( "Authorization","Bearer " + e.toUtf8() ) ;

				auto m = "GET\n" + url + "\nAuthorization:Bearer $ACCESS_TOKEN\n" ;

				m_logWindow.update( logWindow::TYPE::REQUEST,m ) ;

				this->networkAccess( { counter,false,request,acc,UrlLabel } ) ;
			}
		}else{
			if( reply.timeOut() ){

				auto err = this->networkTimeOut() ;
				m_logWindow.update( logWindow::TYPE::ERROR,err.unTranslated,true ) ;
				this->updateUi( counter,{},err.translated ) ;
			}else{
				auto err = this->errorMessage( reply ) ;
				m_logWindow.update( logWindow::TYPE::ERROR,err.unTranslated,true ) ;
				this->updateUi( counter,{},err.translated ) ;
			}
		}
	} ) ;
}

void qCheckGMail::getAuthorization( const QString& authocode,
				    gmailauthorization::AuthResult function )
{
	m_manager.post( m_networkRequest,[ & ](){

		auto s = m_settings.stringRunTimePortNumber() ;

		util::urlOpts opts ;

		opts.add( "client_id",m_clientID ) ;
		opts.add( "client_secret",m_clientSecret ) ;
		opts.add( "code",authocode ) ;
		opts.add( "grant_type","authorization_code" ) ;
		opts.add( "redirect_uri","http://127.0.0.1:" + s ) ;

		util::urlOpts opts1 ;

		opts1.add( "client_id","$CLIENT_ID" ) ;
		opts1.add( "client_secret","$CLIENT_SECRET" ) ;

		if( authocode.isEmpty() ){

			opts1.add( "code","$EMPTY_AUTHOCODE" ) ;
		}else{
			opts1.add( "code","$AUTHOCODE" ) ;
		}

		opts1.add( "grant_type","authorization_code" ) ;
		opts1.add( "redirect_uri","http://127.0.0.1:" + s ) ;

		this->logPOST( opts1 ) ;

		return opts.toUtf8() ;

	 }(),[ this,funct = std::move( function ) ]( const utils::network::reply& reply ){

		if( reply.success() ){

			auto m = reply.data() ;

			m_logWindow.update( logWindow::TYPE::RESPONCE,_hideSecret( m ) ) ;

			funct( _parseJSON( m,"refresh_token" ),m ) ;
		}else{
			m_logWindow.update( logWindow::TYPE::RESPONCE,
					    "qCheckGMail: Failed To Get A Responce" ) ;

			funct( {},{} ) ;
		}
	 } ) ;
}

gmailauthorization::getAuth qCheckGMail::getAuthorization()
{
	class meaw : public gmailauthorization::authActions
	{
	public:
		meaw( qCheckGMail& g ) : m_parent( g )
		{
		}
		void operator()( const QString& authocode,
				 gmailauthorization::AuthResult function ) override
		{
			m_parent.getAuthorization( authocode,std::move( function ) ) ;
		}
	private:
		qCheckGMail& m_parent ;
	};

	return { util::type_identity< meaw >(),*this } ;
}

walletmanager::Wallet qCheckGMail::walletHandle()
{
	class meaw : public walletmanager::wallet
	{
	public:
		meaw( qCheckGMail& g ) : m_parent( g )
		{
		}
		void data( QVector< accounts >&& e ) override
		{
			m_parent.getAccountsInfo( std::move( e ) ) ;
		}
		void closed() override
		{
		}
	private:
		qCheckGMail& m_parent ;
	} ;

	return { util::type_identity< meaw >(),*this } ;
}

qCheckGMail::GMailError qCheckGMail::gmailError( const QByteArray& msg )
{
	auto obj = QJsonDocument::fromJson( msg ).object() ;

	obj = obj.value( "error" ).toObject() ;

	auto code = [ & ](){

		int code = obj.value( "code" ).toInt() ;

		if( code == 401 ){

			return qCheckGMail::result::errorCode::unauthenticated ;
		}else{
			return qCheckGMail::result::errorCode::unknown ;
		}
	}() ;

	auto arr = obj.value( "errors" ).toArray() ;

	if( arr.size() > 0 ){

		auto xbj = arr[ 0 ].toObject() ;

		auto msg = xbj.value( "message" ).toString() ;

		if( !msg.isEmpty() ){

			return { code,msg } ;
		}
	}

	return { code,QObject::tr( "Unknown GMail Error" ) } ;
}

void qCheckGMail::networkAccess( const networkAccessContext& ctx )
{
	m_manager.get( ctx.request,[ this,ctx = ctx ]( const utils::network::reply& reply ){

		if( ctx.counter != m_counter ){

			/*
			 * We will get here if there are more than one
			 * on going process of checking mail
			 * and we allow only the most recent one to proceed.
			 */

			auto n1 = QString::number( m_counter ) ;
			auto n2 = QString::number( ctx.counter ) ;

			QString mm = "Expected counter to be \"%1\" but it is \"%2\"" ;

			m_logWindow.update( logWindow::TYPE::INFO,mm.arg( n1,n2 ) ) ;

		}else if( reply.timeOut() ){

			auto err = this->networkTimeOut() ;

			m_logWindow.update( logWindow::TYPE::ERROR,err.unTranslated,true ) ;

			this->updateUi( ctx.counter,{},err.translated ) ;
		}else{
			auto error = reply.error() ;

			if( error == QNetworkReply::NoError ){

				auto content = reply.data() ;

				m_logWindow.update( logWindow::TYPE::RESPONCE,content ) ;

				using qc = qCheckGMail::result::state ;

				this->updateUi( ctx.counter,content,qc::success ) ;

			}else if( error == QNetworkReply::AuthenticationRequiredError ){

				auto content = reply.data() ;

				auto err = this->gmailError( content ) ;

				using qc = qCheckGMail::result::state ;

				m_logWindow.update( logWindow::TYPE::RESPONCE,
						    content,
						    true ) ;

				this->updateUi( ctx.counter,{},{ qc::gmailError,
							     err.code,
							     std::move( err.errorMsg ) } ) ;
			}else{
				if( error == QNetworkReply::TemporaryNetworkFailureError ||
				    error == QNetworkReply::NetworkSessionFailedError ){

					if( !ctx.retrying ){

						auto err = this->errorMessage( reply ) ;

						m_logWindow.update( logWindow::TYPE::ERROR,
								    err.unTranslated,
								    true ) ;

						auto m = QString::number( m_retryWaitTime / 1000 ) ;

						auto e = QString( "Waiting %1 Seconds Before Retrying" ).arg( m ) ;

						m_logWindow.update( logWindow::TYPE::INFO,e,true ) ;

						utils::qtimer::run( m_retryWaitTime,[ this,ctx = ctx ](){

							this->checkMail( ctx.counter,ctx.acc,ctx.label,true ) ;
						} ) ;

						return ;
					}
				}

				auto err = this->errorMessage( reply ) ;

				m_logWindow.update( logWindow::TYPE::ERROR,
						    err.unTranslated,
						    true ) ;

				this->updateUi( ctx.counter,{},err.translated ) ;
			}
		}
	} ) ;
}

void qCheckGMail::getGMailAccountInfoWithoutToken( const QString& authocode,
						   addaccount::GmailAccountInfo ginfo )
{
	m_manager.post( m_networkRequest,[ & ](){

		util::urlOpts opts ;

		opts.add( "client_id",m_clientID ) ;
		opts.add( "client_secret",m_clientSecret ) ;
		opts.add( "refresh_token",authocode ) ;
		opts.add( "grant_type","refresh_token" ) ;

		util::urlOpts opts1 ;

		opts1.add( "client_id","$CLIENT_ID" ) ;
		opts1.add( "client_secret","$CLIENT_SECRET" ) ;

		if( authocode.isEmpty() ){

			opts1.add( "code","$EMPTY_AUTHOCODE" ) ;
		}else{
			opts1.add( "code","$AUTHOCODE" ) ;
		}

		opts1.add( "refresh_token","$AUTHOCODE" ) ;
		opts1.add( "grant_type","refresh_token" ) ;

		this->logPOST( opts1 ) ;

		return opts.toUtf8() ;

	}(),[ this,ginfo = std::move( ginfo ) ]( const utils::network::reply& reply )mutable{

		if( reply.success() ){

			auto data = reply.data() ;

			m_logWindow.update( logWindow::TYPE::RESPONCE,_hideSecret( data ) ) ;

			auto e = _parseJSON( data,"access_token" ) ;

			this->getLabels( e,std::move( ginfo ) ) ;
		}else{
			if( reply.timeOut() ){

				auto err = this->networkTimeOut() ;
				ginfo( err.translated ) ;
				m_logWindow.update( logWindow::TYPE::ERROR,err.unTranslated,true ) ;
			}else{
				auto err = reply.errorString() ;
				ginfo( err ) ;				
				m_logWindow.update( logWindow::TYPE::ERROR,err,true ) ;
			}
		}
	} ) ;
}

void qCheckGMail::getGMailAccountInfoWithToken( const QString& accName,
						addaccount::GmailAccountInfo ginfo )
{
	for( const auto& it : m_accounts ){

		if( it.accountName() ==accName ){

			this->getLabels( it.accessToken(),std::move( ginfo ) ) ;

			break ;
		}
	}
}

void qCheckGMail::getLabels( const QString& accessToken,
			     addaccount::GmailAccountInfo ginfo )
{
	QUrl url( "https://gmail.googleapis.com/gmail/v1/users/me/labels" ) ;

	QNetworkRequest r( url ) ;
	r.setRawHeader( "Authorization","Bearer " + accessToken.toUtf8() ) ;

	QString m = "GET\nhttps://gmail.googleapis.com/gmail/v1/users/me/labels" ;
	QString mm = "\nAuthorization:Bearer $ACCESS_TOKEN\n" ;

	m_logWindow.update( logWindow::TYPE::REQUEST,m + mm ) ;

	using rpy = utils::network::reply ;

	m_manager.get( r,[ this,ginfo = std::move( ginfo ) ]( const rpy& reply ){

		if( reply.success() ){

			auto ss = reply.data() ;

			m_logWindow.update( logWindow::TYPE::RESPONCE,ss ) ;

			const auto arrr = QJsonDocument::fromJson( ss ) ;

			auto arr = arrr.object().value( "labels" ).toArray() ;

			addaccount::labels labels ;

			for( const auto& it : arr ){

				auto obj = it.toObject() ;
				auto id = obj.value( "id" ).toString() ;
				auto name = obj.value( "name" ).toString() ;

				labels.entries.append( { name,id } ) ;
			}

			ginfo( std::move( labels ) ) ;
		}else{
			if( reply.timeOut() ){

				auto err = this->networkTimeOut() ;
				ginfo( err.translated ) ;
				m_logWindow.update( logWindow::TYPE::ERROR,err.unTranslated,true ) ;
			}else{
				auto err = reply.errorString() ;
				ginfo( err ) ;
				m_logWindow.update( logWindow::TYPE::ERROR,err,true ) ;
			}
		}
	} ) ;
}

void qCheckGMail::checkMail( int counter,const accounts& acc,const QString& UrlLabel,bool retrying )
{
	const auto& accessToken = acc.accessToken() ;

	if( accessToken.isEmpty() ){

		const auto& refreshToken = acc.refreshToken() ;

		if( refreshToken.isEmpty() ){

			m_logWindow.update( logWindow::TYPE::INFO,"Missing Refresh Token" ) ;

			this->updateUi( counter,{},tr( "Missing Refresh Token" ) ) ;
		}else{
			/*
			 * We will get here when we are running for the
			 * first time after startup or if an access token has expired.
			 */
			this->getAccessToken( counter,acc,refreshToken,UrlLabel ) ;
		}
	}else{
		auto url = UrlLabel.toUtf8() ;

		QNetworkRequest s( QUrl( url.constData() ) ) ;

		s.setRawHeader( "Authorization","Bearer " + accessToken.toUtf8() ) ;

		auto m = "GET\n" + url + "\nAuthorization:Bearer $ACCESS_TOKEN\n" ;

		m_logWindow.update( logWindow::TYPE::REQUEST,m ) ;

		this->networkAccess( { counter,retrying,s,acc,UrlLabel } ) ;
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

	auto ff = []( const accounts& lhs,const accounts& rhs ){

		return lhs.accountName().length() < rhs.accountName().length() ;
	} ;

	std::sort( m_accounts.begin(),m_accounts.end(),std::move( ff ) ) ;

	m_numberOfAccounts = m_accounts.size() ;

	if( m_numberOfAccounts > 0 ){

		this->showToolTip( m_errorIcon,tr( "Status" ),{} ) ;
		this->checkMail() ;
	}else{
		/*
		 * wallet is empty,warn the user about it
		 */
		this->noAccountConfigured() ;
	}
}

qCheckGMail::errMessage qCheckGMail::networkTimeOut()
{
	auto e = QString::number( m_networkTimeOut / 1000 ) ;

	auto a = tr( "%1 Seconds Network TimeOut" ).arg( e ) ;
	auto b = QString( "%1 Seconds Network TimeOut" ).arg( e ) ;

	return { std::move( a ),std::move( b ) } ;
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
		meaw( qCheckGMail& m ) : m_parent( m )
		{
		}
		void withoutToken( const QString& authocode,
				 addaccount::GmailAccountInfo returnEmail ) override
		{
			m_parent.getGMailAccountInfoWithoutToken( authocode,std::move( returnEmail ) ) ;
		}
		void withToken( const QString& accountName,
				 addaccount::GmailAccountInfo returnEmail ) override
		{
			m_parent.getGMailAccountInfoWithToken( accountName,std::move( returnEmail ) ) ;
		}
	private:
		qCheckGMail& m_parent ;
	};

	walletmanager::instance( m_applicationIcon,
				 m_settings,
				 m_logWindow,
				 this->walletHandle(),
				 this->getAuthorization(),
				 { util::type_identity< meaw >(),*this } ).ShowUI() ;
}

void qCheckGMail::configurePassWord()
{
	auto& e = walletmanager::instance( m_applicationIcon,m_settings,m_logWindow ) ;

	e.changeWalletPassword() ;
}

void qCheckGMail::getAccountsInfo()
{
	auto& e = walletmanager::instance( this->walletHandle(),m_settings,m_logWindow ) ;

	e.getAccounts() ;
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
	m_interval = m_settings.checkForUpdatesInterval() ;
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
