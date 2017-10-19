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

#include "3rdParty/json.hpp"

#include <string.h>
#include <utility>

qCheckGMail::qCheckGMail( const QString& profile ) :
	m_profile( profile ),
	m_networkRequest( QUrl( "https://accounts.google.com/o/oauth2/token" ) )
{
	m_networkRequest.setRawHeader( "Host","accounts.google.com" ) ;
	m_networkRequest.setRawHeader( "Content-Type","application/x-www-form-urlencoded" ) ;
}

qCheckGMail::~qCheckGMail()
{
}

void qCheckGMail::setTrayIconToVisible( bool showIcon )
{
	if( showIcon ){

		m_statusicon.setStatus( m_statusicon.NeedsAttention ) ;
	}else{
		m_statusicon.setStatus( m_statusicon.Passive ) ;
	}
}

void qCheckGMail::showToolTip( const QString& x,const QString& y,const QString& z )
{
	m_statusicon.setToolTip( x,y,z ) ;
}

void qCheckGMail::showPausedIcon( bool paused )
{
	if( paused ){

		m_statusicon.setOverlayIcon( m_newEmailIcon ) ;
	}else{
		m_statusicon.setOverlayIcon( QString() ) ;
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

void qCheckGMail::start()
{
        QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void qCheckGMail::run()
{
	configurationoptionsdialog::setProfile( m_profile ) ;

	m_statusicon.setCategory( m_statusicon.ApplicationStatus ) ;
        QCoreApplication::setApplicationName( "qCheckGMail" ) ;

	m_enableDebug         = m_statusicon.enableDebug() ;
	m_reportOnAllAccounts = configurationoptionsdialog::reportOnAllAccounts() ;
	m_audioNotify         = configurationoptionsdialog::audioNotify() ;
	m_interval            = configurationoptionsdialog::getTimeFromConfigFile() ;
	m_newEmailIcon        = configurationoptionsdialog::newEmailIcon() ;
	m_errorIcon           = configurationoptionsdialog::errorIcon() ;
	m_noEmailIcon         = configurationoptionsdialog::noEmailIcon() ;
	m_displayEmailCount   = configurationoptionsdialog::displayEmailCount() ;
	m_networkTimeOut      = configurationoptionsdialog::networkTimeOut() ;
	m_defaultApplication  = configurationoptionsdialog::defaultApplication() ;
	m_profileEmailList    = configurationoptionsdialog::profileEmailList() ;
        m_clientID            = configurationoptionsdialog::clientID() ;
        m_clientSecret        = configurationoptionsdialog::clientSecret() ;

	m_applicationIcon     = m_noEmailIcon ;

	m_numberOfAccounts  = 0 ;
	m_numberOfLabels    = 0 ;

        m_checkingMail      = false ;

        m_clickActions.onLeftClick = [ & ](){

		if( m_defaultApplication == "browser" ){

			if( m_accounts.size() > 0 ){

                                auto url = m_accounts.first().defaultLabelUrl() ;

				int index = url.size() - strlen( "/feed/atom/" ) ;
				url.truncate( index ) ;

				QDesktopServices::openUrl( QUrl( url ) ) ;
			}else{
				QDesktopServices::openUrl( QUrl( "https://mail.google.com/" ) ) ;
			}
		}else{
			QProcess::startDetached( m_defaultApplication ) ;
		}
	} ;

	m_statusicon.setIconClickedActions( m_clickActions ) ;

	this->changeIcon( m_errorIcon ) ;
	this->setTrayIconToVisible( true ) ;

        m_timeOut = [ this ](){

                auto e = new QTimer( this ) ;

		connect( e,SIGNAL( timeout() ),this,SLOT( timerExpired() ),Qt::QueuedConnection ) ;

                return e ;
        }() ;

        m_timer = [ this ](){

                auto e = new QTimer( this ) ;

		connect( e,SIGNAL( timeout() ),this,SLOT( checkMail() ),Qt::QueuedConnection ) ;

                e->start( m_interval ) ;

                return e ;
        }() ;

	this->setLocalLanguage() ;
	this->addActionsToMenu() ;
	this->showToolTip( m_errorIcon,tr( "Status" ),tr( "Opening Wallet" ) ) ;
	this->getAccountsInfo() ;
}

void qCheckGMail::iconClicked()
{
	m_clickActions.onLeftClick() ;
}

void qCheckGMail::addActionsToMenu()
{
        auto _connect = [ this ]( const char * signal,const char * slot,QAction * ac ){

               connect( ac,signal,this,slot ) ;
        } ;

        _connect( SIGNAL( triggered() ),SLOT( checkMail() ),[ this ](){

	       return m_statusicon.getAction( tr( "Check Mail Now" ) ) ;
        }() ) ;

        _connect( SIGNAL( toggled( bool ) ),SLOT( pauseCheckingMail( bool ) ),[ this ](){

		auto ac = m_statusicon.getAction( tr( "Pause Checking Mail" ) ) ;

		ac->setObjectName( "PauseCheckingMail" ) ;
                ac->setCheckable( true ) ;
                ac->setChecked( false ) ;

                return ac ;
        }() ) ;

        _connect( SIGNAL( triggered() ),SLOT( configureAccounts() ),[ this ](){

		return m_statusicon.getAction( tr( "Configure Accounts" ) ) ;
        }() ) ;

        _connect( SIGNAL( triggered() ),SLOT( configurePassWord() ),[ this ](){

		auto ac = m_statusicon.getAction( tr( "Configure Password" ) ) ;

		ac->setObjectName( "ConfigurePassword" ) ;
                ac->setEnabled( configurationoptionsdialog::usingInternalStorageSystem() ) ;

                return ac ;
        }() ) ;

         _connect( SIGNAL( triggered() ),SLOT( configurationoptionWindow() ),[ this ](){

		return m_statusicon.getAction( tr( "Configure Options" ) ) ;
        }() ) ;

	m_statusicon.addQuitAction() ;
}

void qCheckGMail::noInternet( const QString& e )
{
	auto header = tr( "Network Problem Detected" ) ;
	auto msg    = tr( "Could Not Connect To The Internet" ) ;

        if( e.isEmpty() ){

                this->showToolTip( m_errorIcon,header,msg ) ;
        }else{
                this->showToolTip( m_errorIcon,header,e ) ;
        }

	this->changeIcon( m_errorIcon ) ;
	this->doneCheckingMail() ;
}

void qCheckGMail::timerExpired()
{
	m_timeOut->stop() ;

	m_manager.cancel( m_networkReply ) ;

        this->failedToCheckForNewEmail() ;

        this->doneCheckingMail() ;

        /*
	 * network time out occured,retry
	 */

        this->checkMail() ;
}

QString qCheckGMail::displayName()
{
        const auto& account     = m_accounts.at( m_currentAccount ) ;
        const auto& displayName = account.displayName() ;
        const auto& accountName = account.accountName() ;
        const auto label        = account.labelUrlAt( m_currentLabel ).split( "/" ).last() ;

	if( label.isEmpty() ){

		if( displayName.isEmpty() ){

			return accountName ;
		}else{
			return displayName ;
		}
	}else{
		if( displayName.isEmpty() ){

			return QString( "%1/%2" ).arg( accountName,label ) ;
		}else{
			return QString( "%1/%2" ).arg( displayName,label ) ;
		}
	}
}

QString qCheckGMail::getAtomComponent( const QByteArray& msg,const QString& cmp,int from )
{
        auto x = QString( "<%1>" ).arg( cmp ) ;
        auto z = QString( "</%1>" ).arg( cmp ) ;

        auto index_1 = msg.indexOf( x,from ) + x.size() ;
        auto index_2 = msg.indexOf( z ) - index_1  ;

	return QString( msg.mid( index_1,index_2 ) ) ;
}

QString qCheckGMail::getAtomComponent( const QByteArray& msg,const QString& cmp,const QString& entry )
{
	return this->getAtomComponent( msg,cmp,msg.indexOf( entry ) ) ;
}

void qCheckGMail::wrongAccountNameOrPassword()
{
        auto x = m_accounts.at( m_currentAccount ).accountName() ;
	auto e = tr( "%1 Account Has Wrong Username/Password Combination" ).arg( x ) ;

	this->changeIcon( m_errorIcon ) ;
	this->showToolTip( m_errorIcon,tr( "Account Related Error Was Detected" ),e ) ;
	this->doneCheckingMail() ;
}

static QString _account_status( int current_account,const QString& displayName,const QString& mailCount )
{
	QString d_name = displayName ;

	if( d_name.size() >= 32 ){

		d_name.truncate( 29 ) ;
		d_name += "..." ;
	}else{
		while( d_name.size() < 32 ){

			d_name += " " ;
		}
	}

	QString e = [ & ](){

		if( mailCount.toInt() > 0 ){

			return "<b>%1 %2</b>" ;
		}else{
			return "%1 %2" ;
		}
	}() ;

	if( current_account > 0 ){

		return "<br>" + e.arg( d_name,mailCount ) ;
	}else{
		return e.arg( d_name,mailCount ) ;
	}
}

/*
 * This function goes through all accounts and give reports of all of their states
 */
void qCheckGMail::reportOnAllAccounts( const QByteArray& msg,bool error )
{
        if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) || error ){

		auto& acc = *( m_accounts.data() + m_currentAccount ) ;

		if( acc.refreshToken().isEmpty() ){

                        /*
                         * Wrong user name or password entered
                         */
			m_accountsStatus += _account_status( m_currentAccount,this->displayName(),"-1" ) ;
                }else{
			if( m_badAccessToken ){

				/*
				 * We had a bad token twice for the same account???
				 *
				 * Bail out to prevent an endless loop.
				 */
				m_accountsStatus += _account_status( m_currentAccount,this->displayName(),"-1" ) ;
			}else{
				/*
				 * We will get here if:
				 * 1. The access token has expired
				 * 2. User revoked access to the account.
				 */

				m_badAccessToken = true ;

				acc.setAccessToken( QString() ) ;

				return this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
			}
                }
	}else{
                auto mailCount = this->getAtomComponent( msg,"fullcount" ) ;

                auto mailCount_1 = mailCount.toInt() ;

		if( mailCount_1 == 0 ){

			m_accountsStatus += _account_status( m_currentAccount,this->displayName(),"0" ) ;
		}else{
			m_mailCount += mailCount_1 ;
			m_accountsStatus += _account_status( m_currentAccount,this->displayName(),mailCount ) ;
		}

                auto acc = m_accounts.data() + m_currentAccount ;
                auto& label = acc->getAccountLabel( m_currentLabel ) ;

                this->checkAccountLastUpdate( label,msg,mailCount_1 ) ;
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

/*
 * This function stops on the first account it finds with a new email.
 * This function will hence report only the first account it finds with new email
 *
 * This mail checking way is visually more appealing on the tray bubble when only one
 * account is set up
 */
void qCheckGMail::reportOnlyFirstAccountWithMail( const QByteArray& msg,bool error )
{
	int count = 0 ;
	QString mailCount ;

        if( msg.contains( "<TITLE>Unauthorized</TITLE>" ) || error ){

		auto& acc = *( m_accounts.data() + m_currentAccount ) ;

		if( acc.refreshToken().isEmpty() ){

                        m_accountFailed = true ;
                }else{
			if( m_badAccessToken ){

				m_accountFailed = true ;
			}else{
				m_badAccessToken = true ;
				acc.setAccessToken( QString() ) ;
				return this->checkMail( acc,acc.labelUrlAt( m_currentLabel ) ) ;
			}
                }
	}else{
		mailCount = this->getAtomComponent( msg,"fullcount" ) ;
		count = mailCount.toInt() ;

                auto acc = m_accounts.data() + m_currentAccount ;
                auto& label = acc->getAccountLabel( m_currentLabel ) ;

                this->checkAccountLastUpdate( label,msg,count ) ;
	}

	if( count > 0 ){

		QString info ;

		if( count == 1 ){

                        auto x = this->getAtomComponent( msg,"name" ) ;
			info = tr( "<table><tr><td>1 Email From <b>%1</b> Is Waiting For You</td></tr></table>" ).arg( x ) ;
		}else{
			info = tr( "%1 Emails Are Waiting For You" ).arg( mailCount ) ;
		}

		this->changeIcon( m_newEmailIcon,count ) ;
		this->setTrayIconToVisible( true ) ;
		this->showToolTip( m_newEmailIcon,this->displayName(),info ) ;
                this->audioNotify() ;
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
                        const auto& acc = m_accounts.at( m_currentAccount ) ;
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
				this->showToolTip( m_noEmailIcon,tr( "Status" ),tr( "No New Email Found" ) ) ;

				if( m_accountFailed ){

					this->changeIcon( m_noEmailIcon,-1 ) ;
				}else{
					this->changeIcon( m_noEmailIcon ) ;
				}

				this->setTrayIconToVisible( false ) ;
                                this->doneCheckingMail() ;
			}
		}
	}
}

void qCheckGMail::checkAccountLastUpdate( accountLabel& label,const QByteArray& msg,int mailCount )
{
	if( label.emailCount() == -1 ){

		/*
		 * we will get here on the first update check after the program start.
		 */
		if( mailCount > 0 ){

			/*
			 * This label has new email(s),mark the time the last email was added or read
			 */
                        m_accountUpdated = true ;
			label.setLastModifiedTime( this->getAtomComponent( msg,"modified","entry" ) ) ;
		}else{
			/*
			 * This label has no new email,mark the time it went to this state
			 */
			label.setLastModifiedTime( this->getAtomComponent( msg,"modified" ) ) ;
		}
	}else{
		if( mailCount == 0 ){

			/*
			 * This label has no new email,mark the time it went to this state
			 */
			label.setLastModifiedTime( this->getAtomComponent( msg,"modified" ) ) ;
		}else{
			/*
			 * This label has atleast one new email
			 */
                        auto m = this->getAtomComponent( msg,"modified","entry" ) ;

                        const auto& z = label.lastModified() ;

                        if( m != z ){

				/*
				 * something changed in this label
				 */
				if( mailCount >= label.emailCount() ){

					/*
					 * Two scenarios will bring us here.
					 * y number of new emails were added.
					 *
					 * x number of emails were read and x number of emails were added to cause total number of
					 * emails to remain the same.
					 */

                                        m_accountUpdated = true ;
				}else{
					/*
					 * Total number of unread emails went down probably because atleast one unread email was read
					 */
				}

				label.setLastModifiedTime( m ) ;
			}else{
				/*
				 * No activity was registered for this label
				 */
			}
		}
	}

        label.setEmailCount( mailCount ) ;
}

void qCheckGMail::audioNotify()
{
        if( m_accountUpdated && m_audioNotify ){

		m_statusicon.newEmailNotify() ;
	}
}

void qCheckGMail::doneCheckingMail()
{
	m_mutex.lock() ;

        m_checkingMail = false ;

        auto redoMailCheck = m_redoMailCheck ;

	m_mutex.unlock() ;

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

		m_timeOut->stop() ;

		this->stopTimer() ;
	}else{
		this->startTimer() ;

		m_mutex.lock() ;

                bool checking = m_checkingMail ;

		m_mutex.unlock() ;

		if( checking ){

			this->failedToCheckForNewEmail() ;
		}else{
			this->checkMail() ;
		}
	}
}

void qCheckGMail::configurationoptionWindow()
{
        configurationoptionsdialog::instance( this ) ;
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
void qCheckGMail::checkMail()
{
	if( m_numberOfAccounts > 0 ){

		bool cancheckMail = false ;

		m_mutex.lock() ;

		if( m_checkingMail ){

			this->failedToCheckForNewEmail() ;
		}else{
			cancheckMail   = true ;
			m_checkingMail = true ;
		}

		m_redoMailCheck = false ;

		m_mutex.unlock() ;

		if( cancheckMail ){

			m_accountsStatus  = "<table>" ;
			m_mailCount       = 0 ;
			m_currentAccount  = 0 ;
                        m_accountUpdated  = false ;
			m_accountFailed   = false ;

			this->checkMail( m_accounts.at( m_currentAccount ) ) ;
                }
	}else{
		qDebug() << tr( "Dont Have Credentials,(Re)Trying To Open Wallet" ) ;
		this->getAccountsInfo() ;
	}
}

void qCheckGMail::checkMail( const accounts& acc )
{
	m_manager.QtNAM().setNetworkAccessible( QNetworkAccessManager::Accessible ) ;

	m_badAccessToken = false ;
	m_currentLabel   = 0 ;
	m_numberOfLabels = acc.numberOfLabels() ;

	this->checkMail( acc,acc.defaultLabelUrl() ) ;
}

static QString _parseJSON( const QByteArray& json,const char * property )
{
	if( !json.isEmpty() ){

		try{
			auto e = nlohmann::json::parse( json.constData() ) ;

			auto s = e.find( property ) ;

			if( s != e.end() ){

				return QString::fromStdString( s.value() ) ;
			}

		}catch( ... ){}
	}

	return QString() ;
}

void qCheckGMail::getAccessToken( const accounts& acc,const QString& refresh_token,const QString& UrlLabel )
{
	m_manager.post( m_networkRequest,[ & ](){

                auto id     = "client_id="     + m_clientID ;
                auto secret = "client_secret=" + m_clientSecret ;
                auto token  = "refresh_token=" + refresh_token ;
                auto type   = "grant_type=refresh_token" ;

                return QString( "%1&%2&%3&%4" ).arg( id,secret,token,type ).toLatin1() ;

	}(),[ UrlLabel,this,&acc ]( QNetworkReply& n ){

		auto e = _parseJSON( n.readAll(),"access_token" ) ;

		acc.setAccessToken( e ) ;

		QNetworkRequest request( QUrl( UrlLabel.toLatin1().constData() ) ) ;

		request.setRawHeader( "Authorization","Bearer " + e.toLatin1() ) ;

		this->networkAccess( request ) ;
        } ) ;
}

gmailauthorization::function_t qCheckGMail::getAuthorization()
{
        return [ this ]( const QString& authocode,std::function< void( const QString& ) > function ){

		m_manager.post( m_networkRequest,[ & ](){

                         auto id     = "client_id="     + m_clientID ;
                         auto secret = "client_secret=" + m_clientSecret ;
                         auto code   = "code="          + authocode ;
                         auto uri    = "redirect_uri=urn:ietf:wg:oauth:2.0:oob" ;
                         auto grant  = "grant_type=authorization_code" ;

                         return QString( "%1&%2&%3&%4&%5" ).arg( id,secret,uri,grant,code ).toLatin1() ;

		 }(),[ funct = std::move( function ) ]( QNetworkReply& e ){

			funct( _parseJSON( e.readAll(),"refresh_token" ) ) ;
                } ) ;
        } ;
}

void qCheckGMail::networkAccess( const QNetworkRequest& request )
{
	m_networkReply = m_manager.get( request,[ this ]( QNetworkReply& e ){

		auto content = e.readAll() ;

		if( m_enableDebug ){

			qDebug() << content << "\n" ;

			if( e.error() != QNetworkReply::NetworkError::NoError ){

				qDebug() << e.errorString() << "\n" ;

				qDebug() << e.error() << "\n\n" ;
			}
		}

                m_timeOut->stop() ;

                auto _report = [ & ]( bool e ){

                        if( m_reportOnAllAccounts ){

                                this->reportOnAllAccounts( content,e ) ;
                        }else{
                                this->reportOnlyFirstAccountWithMail( content,e ) ;
                        }
                } ;

		if( e.error() == QNetworkReply::AuthenticationRequiredError ){

                        _report( true ) ;
                }else{
                        if( content.isEmpty() ){

				this->noInternet() ;
                        }else{
                                _report( false ) ;
                        }
                }
	} ) ;

        m_timeOut->start( m_networkTimeOut ) ;
}

void qCheckGMail::checkMail( const accounts& acc,const QString& UrlLabel )
{
        const auto& refreshToken = acc.refreshToken() ;

        if( refreshToken.isEmpty() ){

		QUrl url( UrlLabel ) ;

		url.setUserName( acc.accountName() ) ;
		url.setPassword( acc.passWord() ) ;

		this->networkAccess( QNetworkRequest( url ) ) ;
        }else{
                const auto& accessToken = acc.accessToken() ;

                if( accessToken.isEmpty() ){

                        /*
                         * We will get here when we are running for the first time after startup
                         * or if an access token has expired.
                         */
                        this->getAccessToken( acc,refreshToken,UrlLabel ) ;
		}else{
			QNetworkRequest s( QUrl( UrlLabel.toLatin1().constData() ) ) ;

			s.setRawHeader( "Authorization","Bearer " + accessToken.toLatin1() ) ;

			this->networkAccess( s ) ;
                }
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
        walletmanager::instance( m_applicationIcon,[ this ](){

		m_mutex.lock() ;
                m_redoMailCheck = true ;
		m_mutex.unlock() ;

        },this->getAuthorization(),[ this ]( QVector< accounts >&& e ){

                 this->getAccountsInfo( std::move( e ) ) ;

        } ).ShowUI() ;
}

void qCheckGMail::configurePassWord()
{
        walletmanager::instance( m_applicationIcon ).changeWalletPassword() ;
}

void qCheckGMail::getAccountsInfo()
{
        walletmanager::instance( [ this ]( QVector< accounts >&& e ){

                this->getAccountsInfo( std::move( e ) ) ;

        } ).getAccounts() ;
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
        auto lang     = configurationoptionsdialog::localLanguage() ;
        auto langPath = configurationoptionsdialog::localLanguagePath() ;

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
        auto lang     = configurationoptionsdialog::localLanguage() ;
        auto langPath = configurationoptionsdialog::localLanguagePath() ;

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
	m_interval = configurationoptionsdialog::getTimeFromConfigFile() ;
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

				m_timer->stop() ;
				m_timer->start( m_interval ) ;
			}

			break ;
		}
	}
}

void qCheckGMail::stopTimer()
{
	m_timer->stop() ;
}

int qCheckGMail::instanceAlreadyRunning()
{
        auto lang = configurationoptionsdialog::localLanguage() ;
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

                auto langPath = configurationoptionsdialog::localLanguagePath() ;

                QTranslator translator ;

                translator.load( r.constData(),langPath + "/translations.qm/" ) ;

                qapp.installTranslator( &translator ) ;

                qDebug() << tr( "another instance is already running,exiting this one" ) ;
	}

	return 1 ;
}

int qCheckGMail::autoStartDisabled()
{
        auto lang = configurationoptionsdialog::localLanguage() ;
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

		QString langPath = configurationoptionsdialog::localLanguagePath() ;

                QTranslator translator ;

                translator.load( r.constData(),langPath + "/translations.qm/" ) ;

                qapp.installTranslator( &translator ) ;

		qDebug() << tr( "Autostart Disabled,Exiting This One" ) ;
	}

	return 1 ;
}

bool qCheckGMail::autoStartEnabled()
{
	return configurationoptionsdialog::autoStartEnabled() ;
}
