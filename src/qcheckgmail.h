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
#include <QtDBus>

#include <functional>
#include <memory>
#include <utility>

#include "statusicon.h"
#include "accounts.h"
#include "walletmanager.h"
#include "configurationoptionsdialog.h"
#include "tray_application_type.h"
#include "utils/network_access_manager.hpp"
#include "gmailauthorization.h"
#include "settings.h"
#include "logwindow.h"

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

	void hasEvent( const QByteArray& ) ;

	static int instanceAlreadyRunning() ;
	static int autoStartDisabled() ;

	static bool autoStartEnabled() ;

	static void setLocalLanguage( QCoreApplication&,QTranslator * ) ;

	QWidget * widget() ;
	QString defaultApplication() ;
private slots:
	void notificationClosed( quint32,quint32 ) ;
	void actionInvoked( quint32,QString ) ;
private:
	void start() ;
	void configurationWindowClosed( int ) ;
	void pauseCheckingMail( bool ) ;
	void configureAccounts() ;
	void configurationoptionWindow() ;
	void setTimer( int ) ;
	void configurePassWord() ;
	void audioNotify( bool ) ;
	void visuallyNotify( bool ) ;
	void alwaysShowTrayIcon( bool ) ;
	void enablePassWordChange( bool ) ;

	void getAccountsInfo( QVector< accounts >&& ) ;

	QString displayName( const QString& = QString() ) ;

	gmailauthorization::getAuth getAuthorization() ;
	void getAuthorization( const QString&,
			       gmailauthorization::AuthResult function ) ;

	walletmanager::Wallet walletHandle() ;

	void getAccessToken( int,const accounts&,const QString&,const QString& ) ;

	struct networkAccessContext
	{
		int counter ;
		bool retrying ;
		const QNetworkRequest& request ;
		const accounts& acc ;
		const QString& label ;
	} ;

	void networkAccess( const networkAccessContext& ) ;

	void getGMailAccountInfoWithoutToken( const QString&,addaccount::GmailAccountInfo ) ;
	void getGMailAccountInfoWithToken( const QString&,addaccount::GmailAccountInfo ) ;

	void getLabels( const QString& accessToken,addaccount::GmailAccountInfo ) ;

	void setTrayIconToVisible( bool ) ;
	void showToolTip( const QString& iconName,
			  const QString& title,
			  const QString& subTitle ) ;
	void showPausedIcon( bool ) ;
	void setUpEmailNotifications() ;
	void setLocalLanguage() ;
	void walletNotOPenedError() ;
	void checkMail( int,const accounts& acc,const QString& label,bool retrying ) ;
	void checkMail( int,const accounts& acc ) ;
	void checkMail() ;
	void checkMail( bool ) ;
	void openMail() ;
	void openInbox( const QString& ) ;
	void changeIcon( const QString& icon ) ;
	void changeIcon( const QString& icon,int ) ;
	void getAccountsInfo() ;
	void startTimer() ;
	void stopTimer() ;
	void setTimer() ;
	class result{
	public:
		enum class state{ success,gmailError,otherError } ;
		enum class errorCode{ unauthenticated,unknown } ;

		result( QString s ) :
			m_errorString( std::move( s ) ),
			m_state( qCheckGMail::result::state::otherError )
		{
			m_errorString.insert( 0,": " ) ;
		}
		result( const char * s ) : m_errorString( s ),
			m_state( qCheckGMail::result::state::otherError )
		{
			m_errorString.insert( 0,": " ) ;
		}
		result( qCheckGMail::result::state s ) : m_state( s )
		{
		}
		result( qCheckGMail::result::state s,
			qCheckGMail::result::errorCode e,
			QString m ) :
			m_errorString( std::move( m ) ),m_state( s ),m_errorCode( e )
		{
			m_errorString.insert( 0,": " ) ;
		}
		bool success()
		{
			return m_state == qCheckGMail::result::state::success ;
		}
		bool gmailError()
		{
			return m_state == qCheckGMail::result::state::gmailError ;
		}
		bool gmailErrorOfUnauthenticated()
		{
			using err = qCheckGMail::result::errorCode ;
			return m_errorCode == err::unauthenticated ;
		}
		const QString& errorString()
		{
			return m_errorString ;
		}
	private:
		QString m_errorString ;
		qCheckGMail::result::state m_state ;
		qCheckGMail::result::errorCode m_errorCode ;
	} ;
	void updateUi( int,const QByteArray&,qCheckGMail::result ) ;
	void noAccountConfigured() ;
	void doneCheckingMail() ;
	void audioNotify() ;
	void visualNotify() ;
	void addActionsToMenu() ;
	void showLogWindow() ;
	void logPOST( const util::urlOpts& ) ;

	struct GMailError
	{
		qCheckGMail::result::errorCode code ;
		QString errorMsg ;
	} ;

	qCheckGMail::GMailError gmailError( const QByteArray& msg ) ;

	struct errMessage
	{
		QString translated ;
		QString unTranslated ;
	} ;

	qCheckGMail::errMessage errorMessage( const utils::network::reply& ) ;
	qCheckGMail::errMessage networkTimeOut() ;

	statusicon::clickActions clickActions() ;

	settings m_settings ;

	int m_interval ;
	int m_numberOfLabels ;
	int m_currentLabel ;
	int m_numberOfAccounts ;
	int m_currentAccount ;
	int m_mailCount ;
	int m_networkTimeOut ;
	int m_notificationTimeOut ;
	int m_retryWaitTime ;

	quint32 m_dbusId = 0 ;

	bool m_audioNotify ;
	bool m_displayEmailCount ;
	bool m_newMailFound ;
	bool m_checkingMail ;
	bool m_timeExpired ;
	bool m_accountFailed ;
	bool m_badAccessToken ;
	bool m_alwaysShowTrayIcon ;
	bool m_errorOccured ;
	bool m_visualNotify ;

	QString m_accountsStatus ;
	QString m_newEmailIcon ;
	QString m_errorIcon ;
	QString m_noEmailIcon ;
	QString m_pausedIcon ;
	QString m_checkingMailIcon ;
	QString m_applicationIcon ;
	QString m_defaultApplication ;
	QString m_accountNameColumnWidth ;
	QString m_clientID ;
	QString m_clientSecret ;
	QString m_visibleIconState ;

	QMenu * m_menu ;

	QStringList m_profileEmailList ;

	QMutex m_mutex ;

	QTimer m_timer ;

	utils::network::manager m_manager ;

	const QString m_auth = "https://accounts.google.com/o/oauth2/token" ;

	QNetworkRequest m_networkRequest ;

	QVector< accounts > m_accounts ;

	QApplication& m_qApp ;
	QStringList m_args ;

	logWindow m_logWindow ;

	statusicon m_statusicon ;

	int m_counter = -1 ;

	QDBusConnection m_dbusConnection ;

	QDBusInterface m_dbusInterface ;
};

#endif // QCHECKGMAIL_H
