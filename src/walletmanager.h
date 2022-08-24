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
#include "settings.h"

namespace Ui {
class walletmanager;
}

class walletmanager : public QDialog
{
	Q_OBJECT
	enum class accountOperation {
		getAccountInfo,
		showAccountInfo
	} ;
public:
	struct wallet
	{
		virtual void closed()
		{
		}
		virtual void data( QVector< accounts >&& )
		{
		}
		virtual ~wallet() ;
	} ;

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

	static walletmanager& instance( const QString& icon,settings& s )
        {
		return *( new walletmanager( icon,s ) ) ;
        }

	static walletmanager& instance( walletmanager::Wallet f,settings& s )
	{
		return *( new walletmanager( std::move( f ),s ) ) ;
	}

        static walletmanager& instance( const QString& icon,
					settings& s,
					walletmanager::Wallet e,
					gmailauthorization::getAuth k,
					addaccount::GMailInfo n )
        {
		return *( new walletmanager( icon,s,std::move( e ),std::move( k ),std::move( n ) ) ) ;
        }

	walletmanager( const QString& icon,settings& ) ;
	walletmanager( walletmanager::Wallet,settings& ) ;

        walletmanager( const QString& icon,
		       settings&,
		       walletmanager::Wallet,
		       gmailauthorization::getAuth,
		       addaccount::GMailInfo ) ;

	void changeWalletPassword() ;
	void ShowUI() ;
	void getAccounts() ;
	~walletmanager() override ;
private:
	void deleteAccount( bool = false ) ;
	void editEntryLabels() ;
	void pushButtonAdd() ;
	void pushButtonClose() ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll() ;
	void pushButtonAdd( accounts::entry&& ) ;
	void editAccount( const QString& accName,const QString& labels,addaccount::labels&&,int row ) ;
	void editAccount( int row,addaccount::labels&& ) ;
	void openWallet() ;
	const accounts& addEntry( const accounts& ) ;
	void disableAll() ;
	void changePassword( bool ) ;
	void buildGUI() ;
	void selectRow( int row,bool highlight = true ) ;
	void selectLastRow() ;
	void HideUI() ;
	void closeEvent( QCloseEvent * ) override ;
	bool eventFilter( QObject * watched,QEvent * event ) override ;
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

	addaccount::GMailInfo m_getAccountInfo ;

	settings& m_settings ;
};

#endif // CONFIGURATIONDIALOG_H
