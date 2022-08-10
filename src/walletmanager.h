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
	class wallet
	{
	public:
		virtual void closed()
		{
		}
		virtual void data( QVector< accounts >&& )
		{
		}
		virtual ~wallet() ;
	private:
	};

	class Wallet
	{
	public:
		Wallet() : m_handle( std::make_unique< wallet >() )
		{
		}
		template< typename Type,typename ... Args >
		Wallet( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void closed()
		{
			m_handle->closed() ;
		}
		void data( QVector< accounts >&& e )
		{
			m_handle->data( std::move( e ) ) ;
		}
	private:
		std::unique_ptr< walletmanager::wallet > m_handle ;
	} ;

	static walletmanager& instance( const QString& icon )
        {
		return *( new walletmanager( icon ) ) ;
        }

	static walletmanager& instance( walletmanager::Wallet f )
	{
		return *( new walletmanager( std::move( f ) ) ) ;
	}

        static walletmanager& instance( const QString& icon,
					walletmanager::Wallet e,
					gmailauthorization::getAuth k )
        {
		return *( new walletmanager( icon,std::move( e ),std::move( k ) ) ) ;
        }

	walletmanager( const QString& icon ) ;
	walletmanager( walletmanager::Wallet ) ;

        walletmanager( const QString& icon,
		       walletmanager::Wallet,
		       gmailauthorization::getAuth ) ;

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
	util::unique_wallet_ptr m_wallet ;

	int m_deleteRow ;
	QTableWidget * m_table ;
	bool m_getAccInfo ;
	accountOperation m_action ;
	accounts::entry m_accEntry ;
	QString m_icon ;
	int m_row ;

	walletmanager::Wallet m_walletData ;

	gmailauthorization::getAuth m_getAuthorization ;
};

#endif // CONFIGURATIONDIALOG_H
