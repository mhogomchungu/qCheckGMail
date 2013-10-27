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

#ifndef TASK_H
#define TASK_H

#include <QRunnable>
#include <QObject>
#include <QString>
#include <QVector>

#include "accounts.h"
#include "../lxqt_wallet/frontend/lxqt_wallet.h"

#define LABEL_IDENTIFIER        "-qCheckGMail-LABEL_ID"
#define DISPLAY_NAME_IDENTIFIER "-qCheckGMail-DISPLAY_NAME_ID"

namespace lxqt{
namespace Wallet{
class Wallet ;
}
}

class Task : public QObject,public QRunnable
{
	Q_OBJECT
public:
	typedef enum{
		addAccount,
		deleteAccount,
		editAccount,
		getAccountInfo,
		showAccountInfo
	}action ;

	Task( lxqt::Wallet::Wallet * wallet,const QString& accName,const QString& accPassWord,
	      const QString& accLabels,const QString& accDisplayName ) ;
	Task( lxqt::Wallet::Wallet * wallet,const QString& accName ) ;
	Task( lxqt::Wallet::Wallet * wallet,QVector<accounts> * ) ;

	~Task() ;

	void start( Task::action ) ;
signals:
	void taskFinished( int ) ;
private:
	const QByteArray& getAccInfo( const QVector<lxqt::Wallet::walletKeyValues>&,const QString& acc ) ;
	void addKey( const QString& accName,const QString& accDisplayName,const QString& accLabels ) ;
	void deleteKey( const QString& accName,const QString& accDisplayName,const QString& accLabels ) ;
	void run( void ) ;
	Task::action m_action ;
	lxqt::Wallet::Wallet * m_wallet ;
	QString m_accName ;
	QString m_accPassWord ;
	QString m_accLabels ;
	QString m_accDisplayName ;
	QVector<accounts> * m_acc ;
	QByteArray m_emptyEntry ;
};

#endif // TASK_H
