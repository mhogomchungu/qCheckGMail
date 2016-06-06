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

#include "../lxqt_wallet/frontend/lxqt_wallet.h"

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
        static walletmanager& instance( const QString& icon )
        {
                auto k = []( const QByteArray& e,std::function< void( const QByteArray& ) > f ){

                        Q_UNUSED( e ) ; Q_UNUSED( f ) ;
                } ;

                auto f = []( QVector< accounts >&& e ){ Q_UNUSED( e ) ; } ;

                return *( new walletmanager( icon,[](){},std::move( k ),std::move( f ) ) ) ;
        }

        static walletmanager& instance( std::function< void( QVector< accounts > ) >&& f )
        {
                auto k = []( const QByteArray& e,std::function< void( const QByteArray& ) > f ){

                        Q_UNUSED( e ) ; Q_UNUSED( f )
                } ;

                return *( new walletmanager( QString(),[](){},std::move( k ),std::move( f ) ) ) ;
        }

        static walletmanager& instance( const QString& icon,
                                        std::function< void() >&& e,
                                        std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) >&& k,
                                        std::function< void( QVector< accounts > && ) >&& f )
        {
                return *( new walletmanager( icon,std::move( e ),std::move( k ),std::move( f ) ) ) ;
        }

        walletmanager( const QString& icon,
                       std::function< void() >&&,
                       std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) >&&,
                       std::function< void( QVector< accounts > && ) >&& ) ;

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
	void deleteAccount() ;
	void pushButtonAdd( void ) ;
	void pushButtonClose( void ) ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void editAccount( void ) ;
	void enableAll( void ) ;
private:
	void addEntry( const accounts& ) ;
	void disableAll( void ) ;
	void changePassword( bool ) ;
	void buildGUI( void ) ;
	void selectRow( int row,bool highlight = true ) ;
	void selectLastRow( void ) ;
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void readAccountInfo() ;

	Ui::walletmanager * m_ui ;

        QVector< accounts > m_accounts ;
	LxQt::Wallet::Wallet * m_wallet ;

	int m_deleteRow ;
	QTableWidget * m_table ;
	bool m_getAccInfo ;
	accountOperation m_action ;
        accounts::entry m_accountEntry ;
	QString m_icon ;
	int m_row ;

        std::function< void() > m_walletClosed ;
        std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) > m_getAuthorization ;
        std::function< void( QVector< accounts >&& ) > m_getAccountInfo ;
};

#endif // CONFIGURATIONDIALOG_H
