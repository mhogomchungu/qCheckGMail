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

#include "accounts.h"
#include "kwallet.h"
#include "addaccount.h"
#include "configurationoptionsdialog.h"

namespace Ui {
class kwalletmanager;
}

class kwalletmanager : public QDialog
{
	Q_OBJECT
public:
	explicit kwalletmanager( QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	void getAccounts( void ) ;
	~kwalletmanager();
signals:
	void kwalletmanagerClosed( void ) ;
	void getAccountsInfo( QVector<accounts> ) ;
private slots:
	void deleteRow() ;
	void pushButtonAdd( void ) ;
	void pushButtonClose( void ) ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void walletOpened( bool ) ;
	void walletOpened_1( bool );
	void editEntry( void ) ;
	void addAccount( QString,QString,QString,QString ) ;
	void editAccount( int,QString,QString,QString,QString ) ;
private:
	void buildGUI( void ) ;
	void deleteRow( int ) ;
	void selectRow( int row,bool highlight ) ;
	QString getPassWordFromAccount( QString ) ;
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::kwalletmanager * m_ui ;

	QVector<accounts> m_accounts ;
	KWallet::Wallet * m_wallet ;

	int m_deleteRow ;
	QTableWidget * m_table ;
	QString m_walletName ;
	QString m_passwordFolder ;
};

#endif // CONFIGURATIONDIALOG_H
