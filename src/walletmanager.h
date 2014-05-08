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


#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QVector>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QIcon>
#include <QEvent>
#include <QKeyEvent>

#include "accounts.h"
#include "addaccount.h"
#include "configurationoptionsdialog.h"

#include "../lxqt_wallet/frontend/lxqt_wallet.h"

namespace Ui {
class walletmanager;
}

class walletmanager : public QDialog
{
	typedef enum{
		getAccountInfo,
		showAccountInfo
	}openOperation ;

	Q_OBJECT
public:
	explicit walletmanager( const QString& icon = QString(),QDialog * parent = 0 ) ;
	void changeWalletPassword( void ) ;
	void ShowUI( void ) ;
	void getAccounts( void ) ;
	~walletmanager();
signals:
	void walletmanagerClosed( void ) ;
	void getAccountsInfo( QVector<accounts> ) ;
private slots:
	void walletpassWordChanged( bool ) ;
	void walletIsOpen( bool ) ;
	void deleteRow() ;
	void pushButtonAdd( void ) ;
	void pushButtonClose( void ) ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void editEntry( void ) ;
	void addAccount( QString,QString,QString,QString ) ;
	void editAccount( int,QString,QString,QString,QString ) ;
	void taskComplete( int ) ;
	void enableAll( void ) ;
private:
	void disableAll( void ) ;
	void changePassword( bool ) ;
	void buildGUI( void ) ;
	void selectRow( int row,bool highlight = true ) ;
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;

	Ui::walletmanager * m_ui ;

	QVector<accounts> m_accounts ;
	LxQt::Wallet::Wallet * m_wallet ;

	int m_deleteRow ;
	QTableWidget * m_table ;
	bool m_getAccInfo ;
	openOperation m_action ;
	QString m_accName ;
	QString m_accPassWord ;
	QString m_accLabels ;
	QString m_accDisplayName ;
	QString m_icon ;
	int m_row ;
};

#endif // CONFIGURATIONDIALOG_H
