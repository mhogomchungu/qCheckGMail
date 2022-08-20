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

static QString _configPath( void )
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

void settings::setProfile( const QString& e )
{
	if( !m_profile.isEmpty() && m_settings.contains( m_profile ) ){

		m_profile = e ;
	}

	m_settings.setPath( QSettings::IniFormat,QSettings::UserScope,_configPath() ) ;
}

bool settings::autoStartEnabled()
{
	auto opt = this->getOption( "autostart" ) ;

	if( m_settings.contains( opt ) ){

		return m_settings.value( opt ).toBool() ;
	}else{
		m_settings.setValue( opt,true ) ;
		m_settings.sync() ;
		return true ;
	}
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
	m_settings.setValue( this->getOption( "storageSystem" ),system ) ;
}

void settings::saveReportOnAllAccounts( bool b )
{
	m_settings.setValue( this->getOption( "reportOnAllAccounts" ),b ) ;
}

QString settings::logFile()
{
	return _configPath() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME,PROGRAM_NAME ) ;
}

QString settings::storageSystem()
{
	return m_settings.value( this->getOption( "storageSystem" ) ).toString() ;
}

util::unique_wallet_ptr settings::secureStorageSystem()
{
	using bk = LXQt::Wallet::BackEnd ;

	auto opt = this->getOption( "storageSystem" ) ;

	if( m_settings.contains( opt ) ){

		auto value = m_settings.value( opt ).toString() ;

		if( value == "gnome wallet" ){

			if( LXQt::Wallet::backEndIsSupported( bk::libsecret ) ){

				return _get_bk( bk::libsecret ) ;
			}else{
				m_settings.setValue( opt,QString( "internal wallet" ) ) ;
				return _get_bk( bk::internal ) ;
			}

		}else if( value.contains( "kde" ) ){

			if( LXQt::Wallet::backEndIsSupported( bk::kwallet ) ){

				return _get_bk( bk::kwallet ) ;
			}else{
				m_settings.setValue( opt,"internal wallet" ) ;
				return _get_bk( bk::internal ) ;
			}
		}else{
			QString wallet( "internal wallet" ) ;

			if( value != wallet ){

				m_settings.setValue( opt,wallet ) ;
			}

			return _get_bk( bk::internal ) ;
		}
	}else{
		if( LXQt::Wallet::backEndIsSupported( bk::kwallet ) ){

			m_settings.setValue( opt,QString( DEFAULT_KDE_WALLET ) ) ;
			return _get_bk( bk::kwallet ) ;

		}else if( LXQt::Wallet::backEndIsSupported( bk::libsecret ) ){

			m_settings.setValue( opt,QString( "gnome wallet" ) ) ;
			return _get_bk( bk::libsecret ) ;
		}else{
			m_settings.setValue( opt,QString( "internal wallet" ) ) ;
			return _get_bk( bk::internal ) ;
		}
	}
}

bool settings::audioNotify()
{
	auto opt = this->getOption( "audioNotify" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,true ) ;
	}

	return m_settings.value( opt ).toBool() ;
}

void settings::setIconAlwaysVisible( bool e )
{
	m_settings.setValue( this->getOption( "alwaysShowTrayIcon" ),e ) ;
}

bool settings::alwaysShowTrayIcon()
{
	auto opt = this->getOption( "alwaysShowTrayIcon" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,false ) ;
	}

	return m_settings.value( opt ).toBool() ;
}

QString settings::clientID()
{
	auto opt = this->getOption( "clientID" ) ;

	if( !m_settings.contains( opt ) ){

		auto e = "90790670661-5jnrcfsocksfsh2ajnnqihhhk82798aq.apps.googleusercontent.com" ;
		m_settings.setValue( opt,e ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::clientSecret()
{
	auto opt = this->getOption( "clientSecret" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"LRfPCp9m4PLK-WTo3jHMAQ4i" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::audioPlayer()
{
	auto opt = this->getOption( "audioPlayer" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"mplayer" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::noEmailIcon()
{
	auto opt = this->getOption( "noEmailIconColor" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"qCheckGMail-hasNoMail" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::pausedIcon()
{
	auto opt = this->getOption( "pausedEmailIconColor" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"qCheckGMail-paused" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::newEmailIcon()
{
	auto opt = this->getOption( "newEmailIconColor" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"qCheckGMail-hasMail" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::errorIcon()
{
	auto opt = this->getOption( "errorIconColor" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"qCheckGMail-hasError" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::fontFamily()
{
	auto opt = this->getOption( "displayEmailCountFontFamily" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"Helvetica" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::fontColor()
{
	auto opt = this->getOption( "displayEmailCountFontColor" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"black" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::visibleIconState()
{
	auto opt = this->getOption( "visibleIconState" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"NeedsAttention" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QString settings::defaultApplication()
{
	auto opt = this->getOption( "defaultApplication" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"browser" ) ;
	}

	return m_settings.value( opt ).toString() ;
}

QStringList settings::profileEmailList()
{
	if( !m_profile.isEmpty() && m_settings.contains( m_profile ) ){

		return util::split( m_settings.value( m_profile ).toString() ) ;
	}else{
		return QStringList() ;
	}
}

bool settings::usingInternalStorageSystem()
{
	auto opt = this->getOption( "storageSystem" ) ;
	return m_settings.value( opt ).toString() == "internal wallet" ;
}

int settings::fontSize()
{
	auto opt = this->getOption( "displayEmailCountFontSize" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"80" ) ;
	}

	return m_settings.value( opt ).toInt() ;
}

int settings::portNumber()
{
	auto opt = this->getOption( "PortNumber" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,10000 ) ;
	}

	return m_settings.value( opt ).toInt() ;
}

bool settings::displayEmailCount()
{
	auto opt = this->getOption( "displayEmailCount" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,true ) ;
	}

	return m_settings.value( opt ).toBool() ;
}

int settings::networkTimeOut()
{
	auto opt = this->getOption( "networkTimeOut" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,2 ) ;
	}

	return 1000 * 60 * m_settings.value( opt ).toInt() ;
}

void settings::setAudioNotify( bool audioNotify )
{
	m_settings.setValue( this->getOption( "audioNotify" ),audioNotify ) ;
}

void settings::enableAutoStart( bool b )
{
	m_settings.setValue( this->getOption( "autostart" ),b ) ;
}

bool settings::reportOnAllAccounts()
{
	auto opt = this->getOption( "reportOnAllAccounts" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,true ) ;
	}

	return m_settings.value( opt ).toBool() ;
}



QString settings::localLanguage()
{
	auto opt = this->getOption( "language" ) ;

	if( !m_settings.contains( opt ) ){

		m_settings.setValue( opt,"english_US" ) ;
	}

	return m_settings.value( opt ).toString() ;
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
	m_settings.setValue( this->getOption( "language" ),e ) ;
}

void settings::saveTimeToConfigFile( const QString& e )
{
	m_settings.setValue( this->getOption( "interval" ),e ) ;
}

int settings::getTimeFromConfigFile()
{
	auto opt = this->getOption( "interval" ) ;

	if( m_settings.contains( opt ) ){

		bool ok ;
		auto time = m_settings.value( opt ).toInt( &ok ) ;

		if( ok ){

			return time * 60 * 1000 ;
		}else{
			return 30 * 60 * 1000 ;
		}
	}else{
		m_settings.setValue( opt,"30" ) ;

		return 30 * 60 * 1000 ;
	}
}
