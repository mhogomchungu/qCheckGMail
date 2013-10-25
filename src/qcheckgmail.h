/*
 *
 *  Copyright (c) 2013
 *  name : mhogo mchungu
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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>

#include "statusicon.h"
#include "accounts.h"
#include "walletmanager.h"
#include "configurationoptionsdialog.h"
#include "tray_application_type.h"

class qCheckGMail : public statusicon
{
	Q_OBJECT
public:
	qCheckGMail() ;
	~qCheckGMail();
	void start( void ) ;
	void earlyExit( void ) ;
	static int instanceAlreadyRunning( void ) ;
	static int autoStartDisabled( void ) ;
	static bool autoStartEnabled( void ) ;
	static void setLocalLanguage( QCoreApplication&,QTranslator * ) ;
	QWidget * widget( void ) ;
	const QVector<accounts>& configuredAccounts( void ) ;
private slots:
	void run( void ) ;
	void configurationWindowClosed( int ) ;
	void emailStatusQueryResponce( QNetworkReply * ) ;
	void pauseCheckingMail( bool ) ;
	void configureAccounts( void ) ;
	void configurationoptionWindow( void ) ;
	void checkMail( void ) ;
	void walletmanagerClosed( void ) ;
	void setTimer( int ) ;
	void objectDestroyed( void ) ;
	void reportOnAllAccounts( bool ) ;
	void objectGone( QObject * ) ;
	void getAccountsInfo( QVector<accounts> ) ;
	void configurePassWord( void ) ;
private:
	QString displayName( void ) ;
	QString getAtomComponent( const QByteArray&,const QString&,int from = 0 ) ;
	QString getAtomComponent( const QByteArray&,const QString&,const QString& ) ;
	void setTrayIconToVisible( bool ) ;
	void showToolTip( QString,QString,QString ) ;
	void showPausedIcon( bool ) ;
	void noInternet( void ) ;
	void wrongAccountNameOrPassword( void ) ;
	void setUpEmailNotifications( void ) ;
	void setLocalLanguage( void ) ;
	void walletNotOPenedError( void ) ;
	void checkMail( const accounts& acc,const QString& label ) ;
	void checkMail( const accounts& acc ) ;
	void changeIcon( QString icon ) ;
	void getAccountsInfo( void ) ;
	void startTimer( void ) ;
	void stopTimer( void ) ;
	void setTimer( void ) ;
	void reportOnAllAccounts( const QByteArray& ) ;
	void reportOnlyFirstAccountWithMail( const QByteArray& ) ;
	void noAccountConfigured( void ) ;
	void doneCheckingMail( void ) ;
	void stuck( void ) ;
	void audioNotify( void ) ;
	void checkAccountLastUpdate( const QByteArray&,int ) ;
	QTimer * m_timer ;
	int m_interval ;
	QNetworkAccessManager * m_manager ;
	QVector<accounts> m_accounts ;
	QString m_accountsStatus ;
	bool m_newMailFound ;
	bool m_checkingMail ;
	bool m_reportOnAllAccounts ;
	bool m_enableDebug ;
	bool m_redoMailCheck ;
	bool m_accountUpdated ;
	int m_numberOfLabels ;
	int m_currentLabel ;
	int m_numberOfAccounts ;
	int m_currentAccount ;
	int m_mailCount ;
	QMutex * m_mutex ;
	QString m_accountNameColumnWidth ;
};

#endif // QCHECKGMAIL_H
