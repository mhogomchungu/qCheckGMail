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
#include "gmailauthorization.h"

#include "lxqt_wallet.h"

#include <functional>

namespace Ui {
class walletmanager;
}

class walletmanager : public QDialog
{
	typedef enum{
		getAccountInfo,
		showAccountInfo,
	}accountOperation ;

	Q_OBJECT
public:
	using function_t = std::function< void( QVector< accounts >&& ) > ;

        static walletmanager& instance( const QString& icon )
        {
		return *( new walletmanager( icon ) ) ;
        }

	static walletmanager& instance( walletmanager::function_t&& f )
	{
		return *( new walletmanager( std::move( f ) ) ) ;
        }

        static walletmanager& instance( const QString& icon,
                                        std::function< void() >&& e,
					gmailauthorization::function_t&& k,
					walletmanager::function_t&& f )
        {
		return *( new walletmanager( icon,std::move( e ),
					     std::move( k ),std::move( f ) ) ) ;
        }

	walletmanager( const QString& icon ) ;
	walletmanager( walletmanager::function_t&& ) ;

        walletmanager( const QString& icon,
                       std::function< void() >&&,
		       gmailauthorization::function_t&&,
		       walletmanager::function_t&& ) ;

	void changeWalletPassword( void ) ;
	void ShowUI( void ) ;
	void getAccounts( void ) ;
	~walletmanager();
signals:
	void walletmanagerClosed( void ) ;
private slots:
	void deleteAccount() ;
	void pushButtonAdd( void ) ;
	void pushButtonClose( void ) ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void editAccount( void ) ;
	void enableAll( void ) ;
private:
	void openWallet( void ) ;
	const accounts& addEntry( const accounts& ) ;
	void disableAll( void ) ;
	void changePassword( bool ) ;
	void buildGUI( void ) ;
	void selectRow( int row,bool highlight = true ) ;
	void selectLastRow( void ) ;
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void readAccountInfo() ;

	Ui::walletmanager * m_ui = nullptr ;

        QVector< accounts > m_accounts ;
	LXQt::Wallet::Wallet * m_wallet = nullptr ;

	int m_deleteRow ;
	QTableWidget * m_table ;
	bool m_getAccInfo ;
	accountOperation m_action ;
	accounts::entry m_accEntry ;
	QString m_icon ;
	int m_row ;

	std::function< void() > m_walletClosed = [](){} ;

	gmailauthorization::function_t m_getAuthorization = []( const QString& e,
			std::function< void( const QString& ) > f ){

		Q_UNUSED( e ) ;
		Q_UNUSED( f ) ;
	} ;

	walletmanager::function_t m_getAccountInfo = []( QVector< accounts >&& e ){

		Q_UNUSED( e ) ;
	} ;
};

#endif // CONFIGURATIONDIALOG_H
