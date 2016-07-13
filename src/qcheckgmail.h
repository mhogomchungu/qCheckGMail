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
#include <functional>
#include <memory>
#include <utility>

#include "statusicon.h"
#include "accounts.h"
#include "walletmanager.h"
#include "configurationoptionsdialog.h"
#include "tray_application_type.h"
#include "networkAccessManager.hpp"
#include "gmailauthorization.h"

#include <functional>
#include <utility>

class qCheckGMail : public QObject
{
	Q_OBJECT
public:
	explicit qCheckGMail( const QString& profile = QString() ) ;
	~qCheckGMail();

        void start( void ) ;
	void earlyExit( void ) ;

        static int instanceAlreadyRunning( void ) ;
	static int autoStartDisabled( void ) ;

        static bool autoStartEnabled( void ) ;

        static void setLocalLanguage( QCoreApplication&,QTranslator * ) ;

        QWidget * widget( void ) ;
	QString defaultApplication( void ) ;

        void iconClicked( void ) ;

private slots:
	void run( void ) ;
	void configurationWindowClosed( int ) ;
	void pauseCheckingMail( bool ) ;
	void configureAccounts( void ) ;
	void configurationoptionWindow( void ) ;
	void checkMail( void ) ;
	void setTimer( int ) ;
	void reportOnAllAccounts( bool ) ;
	void configurePassWord( void ) ;
        void audioNotify( bool ) ;
        void enablePassWordChange( bool ) ;
	void timerExpired( void  ) ;
private:
        void getAccountsInfo( QVector< accounts >&& ) ;

	QString displayName( void ) ;
	QString getAtomComponent( const QByteArray&,const QString&,int from = 0 ) ;
	QString getAtomComponent( const QByteArray&,const QString&,const QString& ) ;

	gmailauthorization::function_t getAuthorization() ;

        void getAccessToken( const accounts&,const QString&,const QString& ) ;

        void networkAccess( const QNetworkRequest& ) ;

        void setTrayIconToVisible( bool ) ;
	void showToolTip( const QString&,const QString&,const QString& ) ;
	void showPausedIcon( bool ) ;
        void noInternet( const QString& = QString() ) ;
	void wrongAccountNameOrPassword( void ) ;
	void setUpEmailNotifications( void ) ;
	void setLocalLanguage( void ) ;
	void walletNotOPenedError( void ) ;
        void checkMail( const accounts& acc,const QString& label ) ;
	void checkMail( const accounts& acc ) ;
	void changeIcon( const QString& icon ) ;
	void changeIcon( const QString& icon,int ) ;
	void getAccountsInfo( void ) ;
	void startTimer( void ) ;
	void stopTimer( void ) ;
	void setTimer( void ) ;
        void reportOnAllAccounts( const QByteArray&,bool ) ;
        void reportOnlyFirstAccountWithMail( const QByteArray&,bool ) ;
	void noAccountConfigured( void ) ;
	void doneCheckingMail( void ) ;
	void failedToCheckForNewEmail( void ) ;
        void audioNotify( void ) ;
        void checkAccountLastUpdate( accountLabel&,const QByteArray&,int ) ;
        void addActionsToMenu( void ) ;

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

        QString m_accountsStatus ;
	QString m_newEmailIcon ;
	QString m_errorIcon ;
	QString m_noEmailIcon ;
	QString m_applicationIcon ;
	QString m_defaultApplication ;
	QString m_profile ;
        QString m_accountNameColumnWidth ;
        QString m_clientID ;
        QString m_clientSecret ;

        QStringList m_profileEmailList ;

        std::unique_ptr< QMutex > m_mutex ;

	QTimer * m_timeOut ;
        QTimer * m_timer ;

        statusicon::clickActions m_clickActions ;

        QNetworkReply * m_networkReply ;

        NetworkAccessManager m_manager ;

        QVector< accounts > m_accounts ;

        std::unique_ptr< statusicon > m_statusicon ;
};

#endif // QCHECKGMAIL_H
