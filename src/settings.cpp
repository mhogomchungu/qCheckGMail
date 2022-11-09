/*
 *
 *  Copyright (c) 2022
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

#include "settings.h"

#include "language_path.h"
#include "tray_application_type.h"

#include <QStandardPaths>

#define PROGRAM_NAME "qCheckGMail"
#define ORGANIZATION_NAME "qCheckGMail"

#define DEFAULT_KDE_WALLET     "default kde wallet"
#define qCheckGMail_KDE_wALLET "qCheckGMail kde wallet"

static QString _configPath()
{
	return QStandardPaths::writableLocation( QStandardPaths::GenericConfigLocation ) ;
}

QString settings::getOption( const char * opt )
{
	if( m_profile.isEmpty() ){

		return opt ;
	}else{
		return QString( "%1/%2").arg( m_profile,opt ) ;
	}
}

static util::unique_wallet_ptr _get_bk( LXQt::Wallet::BackEnd bk )
{
	auto w = util::unique_wallet_ptr( LXQt::Wallet::getWalletBackend( bk ).release() ) ;

	w->log( []( const QString& e ){ Q_UNUSED( e ) } ) ;

	return w ;
}

settings::settings() : m_settings( ORGANIZATION_NAME,PROGRAM_NAME )
{
}

QStringList settings::profileEmailList()
{
	if( !m_profile.isEmpty() && m_settings.contains( m_profile ) ){

		return util::split( m_settings.value( m_profile ).toString() ) ;
	}else{
		return QStringList() ;
	}
}

void settings::setWindowDimensions( const QString& window,const QString& dimenstion )
{
	m_settings.setValue( "WindowDimensions_" + window,dimenstion ) ;
}

QString settings::windowsDimensions( const QString& window )
{
	auto m = "WindowDimensions_" + window ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,QString() ) ;
	}

	return m_settings.value( m ).toString() ;
}

void settings::setProfile( const QString& e )
{
	if( !m_profile.isEmpty() && m_settings.contains( m_profile ) ){

		m_profile = e ;
	}

	m_settings.setPath( QSettings::IniFormat,QSettings::UserScope,_configPath() ) ;
}

void settings::setRuntimePortNumber( int s )
{
	m_runtimePortNumber = s ;
}

QString settings::stringRunTimePortNumber()
{
	return QString::number( m_runtimePortNumber ) ;
}

QString settings::walletName( LXQt::Wallet::BackEnd backEnd )
{
	if( backEnd == LXQt::Wallet::BackEnd::kwallet ){

		auto opt = this->getOption( "storageSystem" ) ;

		if( m_settings.value( opt ).toString() == qCheckGMail_KDE_wALLET ){

			return this->walletName() ;
		}else{
			util::unique_wallet_ptr w( _get_bk( backEnd ) ) ;

			if( w ){

				return w->localDefaultWalletName() ;
			}else{
				return "kdewallet" ;
			}
		}
	}else{
		return this->walletName() ;
	}
}

QString settings::walletName()
{
	return "qCheckGMailv2" ;
}

QString settings::applictionName()
{
	return "qCheckGMail" ;
}

void settings::saveStorageSystem( const QString& system )
{
	this->setSetting( "storageSystem",system ) ;
}

QString settings::logFile()
{
	return _configPath() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME,PROGRAM_NAME ) ;
}

QString settings::storageSystem()
{
	return this->getSetting( "storageSystem",[](){

		using bk = LXQt::Wallet::BackEnd ;

		if( LXQt::Wallet::backEndIsSupported( bk::kwallet ) ){

			return DEFAULT_KDE_WALLET ;

		}else if( LXQt::Wallet::backEndIsSupported( bk::libsecret ) ){

			return "gnome wallet" ;
		}else{
			return "internal wallet" ;
		}

	}() ).toString() ;
}

util::unique_wallet_ptr settings::secureStorageSystem()
{
	using bk = LXQt::Wallet::BackEnd ;

	auto value = this->storageSystem() ;

	if( value == "gnome wallet" ){

		if( LXQt::Wallet::backEndIsSupported( bk::libsecret ) ){

			return _get_bk( bk::libsecret ) ;
		}else{
			return _get_bk( bk::internal ) ;
		}

	}else if( value.contains( "kde" ) ){

		if( LXQt::Wallet::backEndIsSupported( bk::kwallet ) ){

			return _get_bk( bk::kwallet ) ;
		}else{
			return _get_bk( bk::internal ) ;
		}
	}else{
		return _get_bk( bk::internal ) ;
	}
}

bool settings::autoStartEnabled()
{
	return this->getSetting( "autostart",true ).toBool() ;
}

bool settings::audioNotify()
{
	return this->getSetting( "audioNotify",true ).toBool() ;
}

bool settings::visualNotify()
{
	return this->getSetting( "visualNotify",true ).toBool() ;
}

bool settings::showHyperlinks()
{
	return this->getSetting( "ShowHyperLinks",true ).toBool() ;
}

void settings::setIconAlwaysVisible( bool e )
{
	this->setSetting( "alwaysShowTrayIcon",e ) ;
}

bool settings::alwaysShowTrayIcon()
{
	return this->getSetting( "alwaysShowTrayIcon",true ).toBool() ;
}

int settings::notificationTimeOut()
{
	return this->getSetting( "NotificationTimeOut",5000 ).toInt() ;
}

int settings::waitTimeBeforeRetrying()
{
	return this->getSetting( "WaitTimeBeforeRetrying",2000 ).toInt() ;
}

void settings::setNotificationTimeOut( int e )
{
	this->setSetting( "NotificationTimeOut",e ) ;
}

QString settings::clientID()
{
	auto e = "90790670661-5jnrcfsocksfsh2ajnnqihhhk82798aq.apps.googleusercontent.com" ;

	return this->getSetting( "clientID",e ).toString() ;
}

QString settings::clientSecret()
{
	return this->getSetting( "clientSecret","LRfPCp9m4PLK-WTo3jHMAQ4i" ).toString() ;
}

QString settings::audioPlayer()
{
	return this->getSetting( "audioPlayer","mplayer" ).toString() ;
}

QString settings::noEmailIcon()
{
	return this->getSetting( "noEmailIconName","qCheckGMail-hasNoMail" ).toString() ;
}

QString settings::pausedIcon()
{
	return this->getSetting( "pausedEmailIconName","qCheckGMail-paused" ).toString() ;
}

QString settings::newEmailIcon()
{
	return this->getSetting( "newEmailIconName","qCheckGMail-hasMail" ).toString() ;
}

QString settings::errorIcon()
{
	return this->getSetting( "errorIconName","qCheckGMail-hasError" ).toString() ;
}

QString settings::checkingMailIcon()
{
	return this->getSetting( "checkingMailIconName","qCheckGMail-paused" ).toString() ;
}

QString settings::fontFamily()
{
	return this->getSetting( "displayEmailCountFontFamily","Helvetica" ).toString() ;
}

QString settings::fontColor()
{
	return this->getSetting( "displayEmailCountFontColor","black" ).toString() ;
}

QString settings::visibleIconState()
{
	return this->getSetting( "visibleIconState","NeedsAttention" ).toString() ;
}

QString settings::defaultApplication()
{
	return this->getSetting( "defaultApplication","browser" ).toString() ;
}

bool settings::usingInternalStorageSystem()
{
	return this->storageSystem() == "internal wallet" ;
}

int settings::fontSize()
{
	return this->getSetting( "displayEmailCountFontSize","80" ).toInt() ;
}

int settings::portNumber()
{
	return this->getSetting( "PortNumber",10000 ).toInt() ;
}

bool settings::displayEmailCount()
{
	return this->getSetting( "displayEmailCount",true ).toBool() ;
}

int settings::networkTimeOut()
{
	return 1000 * this->getSetting( "networkTimeOutInSeconds",30 ).toInt();
}

void settings::setAudioNotify( bool audioNotify )
{
	this->setSetting( "audioNotify",audioNotify ) ;
}

void settings::setVisualNotify( bool visualNotify )
{
	this->setSetting( "visualNotify",visualNotify ) ;
}

void settings::enableAutoStart( bool b )
{
	this->setSetting( "autostart",b ) ;
}

QString settings::localLanguage()
{
	return this->getSetting( "language","english_US" ).toString() ;
}

QString settings::localLanguagePath()
{
	/*
	 * LANGUAGE_FILE_PATH is set by the build system and
	 * it will contain something line $install_prefix/share/qCheckGMail/
	 */
	return LANGUAGE_FILE_PATH ;
}

void settings::saveLocalLanguage( const QString& e )
{
	this->setSetting( "language",e ) ;
}

void settings::setCheckForUpdatesInterval( const QString& m )
{
	bool ok ;
	auto time = m.toInt( &ok ) ;

	if( ok ){

		this->setSetting( "checkForUpdatesIntervalInMinutes",time ) ;
	}
}

int settings::checkForUpdatesInterval()
{
	return 60 * 1000 * this->getSetting( "checkForUpdatesIntervalInMinutes",30 ).toInt() ;
}
