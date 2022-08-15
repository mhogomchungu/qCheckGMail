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

#ifndef QCHECKGMAIL_H
#define QCHECKGMAIL_H

#include <QObject>
#include <QDebug>
#include <QMetaObject>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QStringList>
#include <QTranslator>
#include <QVector>
#include <QUrl>
#include <QCoreApplication>
#include <QMutex>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QtNetwork/QNetworkRequest>

#include <functional>
#include <memory>
#include <utility>

#include "statusicon.h"
#include "accounts.h"
#include "walletmanager.h"
#include "configurationoptionsdialog.h"
#include "tray_application_type.h"
#include "network_access_manager.hpp"
#include "gmailauthorization.h"
#include "settings.h"

#include <functional>
#include <utility>

class qCheckGMail : public QObject
{
	Q_OBJECT
public:
	struct args
	{
		QApplication& app ;
	} ;
	qCheckGMail( const qCheckGMail::args& ) ;
	~qCheckGMail() override ;

	void start( const QByteArray& ) ;

	void earlyExit() ;

	void event( const QByteArray& ) ;

	static int instanceAlreadyRunning() ;
	static int autoStartDisabled() ;

	static bool autoStartEnabled() ;

	static void setLocalLanguage( QCoreApplication&,QTranslator * ) ;

	QWidget * widget() ;
	QString defaultApplication() ;

	void iconClicked() ;
private:
	void start() ;
	void configurationWindowClosed( int ) ;
	void pauseCheckingMail( bool ) ;
	void configureAccounts() ;
	void configurationoptionWindow() ;
	void checkMail( bool = false ) ;
	void setTimer( int ) ;
	void reportOnAllAccounts( bool ) ;
	void configurePassWord() ;
	void audioNotify( bool ) ;
	void alwaysShowTrayIcon( bool ) ;
	void enablePassWordChange( bool ) ;

	void getAccountsInfo( QVector< accounts >&& ) ;

	QString displayName( const QString& ) ;

	gmailauthorization::getAuth getAuthorization() ;

	walletmanager::Wallet walletHandle() ;

	void getAccessToken( const accounts&,const QString&,const QString& ) ;

	void networkAccess( const QNetworkRequest& ) ;

	void getGMailAccountInfo( const QString& authocode,addaccount::GmailAccountInfo returnEmail ) ;
	void getGMailAccountInfo( const QByteArray& accName,addaccount::GmailAccountInfo returnEmail ) ;

	void getLabels( const QString& accessToken,addaccount::GmailAccountInfo ) ;

	void setTrayIconToVisible( bool ) ;
	void showToolTip( const QString&,const QString&,const QString& ) ;
	void showPausedIcon( bool ) ;
	void noInternet( const QString& = QString() ) ;
	void wrongAccountNameOrPassword() ;
	void setUpEmailNotifications() ;
	void setLocalLanguage() ;
	void walletNotOPenedError() ;
	void checkMail( const accounts& acc,const QString& label ) ;
	void checkMail( const accounts& acc ) ;
	void openMail( const accounts& acc ) ;
	void openMail() ;
	void changeIcon( const QString& icon ) ;
	void changeIcon( const QString& icon,int ) ;
	void getAccountsInfo() ;
	void startTimer() ;
	void stopTimer() ;
	void setTimer() ;
	void reportOnAllAccounts( const QByteArray&,bool ) ;
	void reportOnlyFirstAccountWithMail( const QByteArray&,bool ) ;
	void noAccountConfigured() ;
	void doneCheckingMail() ;
	void failedToCheckForNewEmail() ;
	void audioNotify() ;
	void addActionsToMenu() ;

	int m_interval ;
	int m_numberOfLabels ;
	int m_currentLabel ;
	int m_numberOfAccounts ;
	int m_currentAccount ;
	int m_mailCount ;
	int m_networkTimeOut ;

	bool m_audioNotify ;
	bool m_displayEmailCount ;
	bool m_newMailFound ;
	bool m_checkingMail ;
	bool m_reportOnAllAccounts ;
	bool m_enableDebug ;
	bool m_redoMailCheck ;
	bool m_accountUpdated ;
	bool m_timeExpired ;
	bool m_accountFailed ;
	bool m_badAccessToken ;
	bool m_alwaysShowTrayIcon ;

	QString m_accountsStatus ;
	QString m_newEmailIcon ;
	QString m_errorIcon ;
	QString m_noEmailIcon ;
	QString m_applicationIcon ;
	QString m_defaultApplication ;
	QString m_accountNameColumnWidth ;
	QString m_clientID ;
	QString m_clientSecret ;
	QString m_visibleIconState ;

	QStringList m_profileEmailList ;

	QMutex m_mutex ;

	QTimer m_timer ;

	QMenu * m_menu ;

	statusicon::clickActions m_clickActions ;

	NetworkAccessManager m_manager ;

	QNetworkRequest m_networkRequest ;

	QVector< accounts > m_accounts ;

	QApplication& m_qApp ;
	QStringList m_args ;

	settings m_settings ;

	statusicon m_statusicon ;
};

#endif // QCHECKGMAIL_H
