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

#ifndef CONFIGURATIONOPTIONSDIALOG_H
#define CONFIGURATIONOPTIONSDIALOG_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#include "language_path.h"
#include "tray_application_type.h"

#include "../lxqt_wallet/frontend/lxqt_wallet.h"

namespace Ui {
class configurationoptionsdialog;
}

class urlOpts
{
public:
	urlOpts( const QString& url ) : m_values( url + "?" )
	{
	}
	urlOpts()
	{
	}
	urlOpts& add( const QString& key,const QString& value )
	{
		m_values += key + "=" + value + "&" ;
		return *this ;
	}
	QByteArray toUtf8() const
	{
		auto m = m_values.toUtf8() ;
		m.truncate( m.size() - 1 ) ;
		return m ;
	}
	QString toString() const
	{
		auto m = m_values ;
		m.truncate( m.size() - 1 ) ;
		return m ;
	}
private:
	QString m_values ;
};

using unique_wallet_ptr = std::unique_ptr< LXQt::Wallet::Wallet,void( * )( QObject * ) > ;

class configurationoptionsdialog : public QDialog
{
	Q_OBJECT
public:
        static void instance( QObject * parent )
        {
                new configurationoptionsdialog( parent ) ;
        }

        explicit configurationoptionsdialog( QObject * parent = 0 ) ;
	~configurationoptionsdialog() ;
	static bool autoStartEnabled( void ) ;
	static void enableAutoStart( bool ) ;
	static bool reportOnAllAccounts( void ) ;
	static QString localLanguage( void ) ;
	static QString localLanguagePath( void ) ;
	static int getTimeFromConfigFile( void ) ;
	static QString walletName( LXQt::Wallet::BackEnd ) ;
	static QString logFile( void ) ;

	static unique_wallet_ptr secureStorageSystem( void ) ;
	static bool audioNotify( void ) ;
        static QString clientID( void ) ;
        static QString clientSecret( void ) ;
	static QString audioPlayer( void ) ;
	static QString noEmailIcon( void ) ;
	static QString newEmailIcon( void ) ;
	static QString errorIcon( void ) ;
	static QString fontFamily( void ) ;
	static QString fontColor( void ) ;
	static QString visibleIconState( void ) ;
	static QString defaultApplication( void ) ;
	static QString stringRunTimePortNumber( void ) ;
        static QStringList profileEmailList( void ) ;
	static bool usingInternalStorageSystem( void ) ;
	static int fontSize( void ) ;
	static int portNumber( void ) ;
	static bool displayEmailCount( void ) ;
	static int networkTimeOut( void ) ;
	static void setProfile( const QString& profile ) ;
	static void setRuntimePortNumber( int ) ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void audioNotify( bool ) ;
	void setTimer( int ) ;
	void reportOnAllAccounts( bool ) ;
	void enablePassWordChange( bool ) ;
private slots:
	void pushButtonClose( void ) ;
private:
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void setAudioNotify( bool ) ;
	void saveStorageSystem( const QString& ) ;
	void saveReportOnAllAccounts( bool ) ;
	void saveTimeToConfigFile( void ) ;
	void setSupportedLanguages( void ) ;
	void saveLocalLanguage( void ) ;
	void closeEvent( QCloseEvent * ) ;
        Ui::configurationoptionsdialog * m_ui ;
};

#endif // CONFIGURATIONOPTIONSDIALOG_H
