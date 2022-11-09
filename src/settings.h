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
#ifndef SETTINGS_H
#define SETTINGS_H

#include "util.hpp"
#include <QSettings>

class settings
{
public:
	settings() ;
	void setProfile( const QString& ) ;
	bool autoStartEnabled() ;
	void enableAutoStart( bool ) ;
	QString localLanguage() ;
	QString localLanguagePath() ;
	int checkForUpdatesInterval() ;
	QString walletName( LXQt::Wallet::BackEnd ) ;
	QString walletName() ;
	QString applictionName() ;
	QString logFile() ;
	QString storageSystem() ;
	util::unique_wallet_ptr secureStorageSystem() ;
	bool audioNotify() ;
	bool visualNotify() ;
	bool showHyperlinks() ;
	bool alwaysShowTrayIcon() ;
	int notificationTimeOut() ;
	int waitTimeBeforeRetrying() ;
	QString clientID() ;
	QString clientSecret() ;
	QString audioPlayer() ;
	QString noEmailIcon() ;
	QString pausedIcon() ;
	QString newEmailIcon() ;
	QString errorIcon() ;
	QString checkingMailIcon() ;
	QString fontFamily() ;
	QString fontColor() ;
	QString visibleIconState() ;
	QString defaultApplication() ;
	QString stringRunTimePortNumber() ;
	QStringList profileEmailList() ;
	void setWindowDimensions( const QString& window,const QString& dimenstion ) ;
	QString windowsDimensions( const QString& window ) ;
	bool usingInternalStorageSystem() ;
	int fontSize() ;
	int portNumber() ;
	bool displayEmailCount() ;
	int networkTimeOut() ;
	void setRuntimePortNumber( int ) ;
	void saveLocalLanguage( const QString& ) ;
	void setCheckForUpdatesInterval( const QString& ) ;
	void setNotificationTimeOut( int ) ;
	QString getOption( const char * ) ;
	void setAudioNotify( bool ) ;
	void setVisualNotify( bool ) ;
	void setIconAlwaysVisible( bool ) ;
	void saveStorageSystem( const QString& ) ;
	void saveReportOnAllAccounts( bool b ) ;
private:	
	template< typename Type >
	QVariant getSetting( const char * key,const Type& defautValue )
	{
		auto opt = this->getOption( key ) ;

		if( !m_settings.contains( opt ) ){

			m_settings.setValue( opt,defautValue ) ;
		}

		return m_settings.value( opt ) ;
	}
	template< typename Type >
	void setSetting( const char * key,const Type& value )
	{
		m_settings.setValue( this->getOption( key ),value ) ;
	}
	int m_runtimePortNumber ;
	QSettings m_settings ;
	QString m_profile ;
} ;

#endif
