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
#include <QMap>

#include <kstatusnotifieritem.h>
#include <kmenu.h>
#include <kwallet.h>

#include <QVector>

#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "accounts.h"
#include "configurationdialog.h"

class qCheckGMail : public KStatusNotifierItem
{
	Q_OBJECT
public:
	qCheckGMail();
	~qCheckGMail();
	void start( void ) ;
	static int instanceAlreadyRunning( void ) ;
	static bool autoStartEnabled( void ) ;
private slots:
	void run( void ) ;
	void gotReply( QNetworkReply * ) ;
	void pauseCheckingMail( bool ) ;
	void configurationWindow( void ) ;
	void checkMail( void ) ;
	void walletOpened( bool ) ;
	void configurationDialogClosed( void ) ;
private:
	void deleteKWallet( void ) ;
	void getAccountsInfo( void ) ;
	void walletNotOPenedError( void ) ;
	void checkMail( const QString& userName,const QString& password,const QString& label );
	void checkMail( const accounts& acc ) ;

	void changeIcon( QString icon ) ;
	void getAccountsInformation( void ) ;
	QStringList getAccountNames( void ) ;
	void setTimerEvents( void ) ;
	void startTimer( void ) ;
	void stopTimer( void ) ;
	void setTimer( void ) ;
	void processMailStatus( const QByteArray& ) ;
	void noAccountConfigured( void ) ;
	KMenu * m_menu ;
	QTimer * m_timer ;
	bool m_gotCredentials ;
	int m_interval ;
	QNetworkAccessManager * m_manager ;
	QVector<accounts> m_accounts ;
	QVector<accounts> m_accounts_backUp ;
	KWallet::Wallet * m_wallet ;
	QString m_walletName ;
	QString m_accountName ;
	QString m_labelUrl ;
	QStringList m_labelUrls ;
};

#endif // QCHECKGMAIL_H
