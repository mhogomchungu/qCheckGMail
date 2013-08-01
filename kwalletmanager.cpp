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


#include "kwalletmanager.h"
#include "ui_kwalletmanager.h"

#define LABEL_IDENTIFIER        "-qCheckGMail-LABEL_ID"
#define DISPLAY_NAME_IDENTIFIER "-qCheckGMail-DISPLAY_NAME_ID"

kwalletmanager::kwalletmanager( QWidget * parent ) :QDialog( parent ),m_ui( 0 ),m_wallet( 0 )
{
	m_walletName        = configurationoptionsdialog::walletName() ;
	m_defaultWalletName = configurationoptionsdialog::defaultWalletName() ;

	if( m_walletName == m_defaultWalletName ){
		/*
		 * We are using qCheckGMail wallet and hence we can store credentials in the default folder
		 * since nobody else will be using it as nobody else should be using our wallet
		 */
		m_passwordFolder = KWallet::Wallet::PasswordFolder() ;
	}else{
		/*
		 * We are not using qCheckGMail's wallet and hence could be using a shared one.
		 * Store credentials in a private folder
		 */
		m_passwordFolder = configurationoptionsdialog::passwordFolderName() ;
	}
}

void kwalletmanager::buildGUI()
{
	m_ui = new Ui::kwalletmanager ;
	m_ui->setupUi( this );
	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );

	connect( m_ui->pushButtonAccountAdd,SIGNAL( clicked() ),this,SLOT( pushButtonAdd() ) ) ;
	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( tableItemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		 SLOT( tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	m_ui->tableWidget->setColumnWidth( 0,171 ) ;
	m_ui->tableWidget->setColumnWidth( 1,171 ) ;
	m_ui->tableWidget->setColumnWidth( 2,171 ) ;

	m_table = m_ui->tableWidget ;

	m_ui->pushButtonAccountAdd->setFocus() ;

	m_ui->groupBox->setTitle( QString( "" ) ) ;
}

void kwalletmanager::ShowUI()
{
	m_wallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;
	connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
}

void kwalletmanager::walletOpened( bool walletOpened )
{
	if( walletOpened ){

		this->buildGUI();

		QString passWord ;

		QString labels ;
		QString labels_id = QString( LABEL_IDENTIFIER ) ;

		QString displayName ;
		QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

		QTableWidgetItem * item ;

		int row = 0 ;

		m_wallet->setFolder( m_passwordFolder ) ;

		QStringList acc = m_wallet->entryList() ;

		int j = acc.size() ;

		for( int i = 0 ; i < j ; i++ ){
			const QString& accName = acc.at( i ) ;
			if( accName.endsWith( labels_id ) || accName.endsWith( displayName_id ) ){
				;
			}else{
				row = m_table->rowCount() ;

				m_table->insertRow( row ) ;

				item = new QTableWidgetItem() ;
				item->setText( accName ) ;
				item->setTextAlignment( Qt::AlignCenter ) ;
				m_table->setItem( row,0,item ) ;

				m_wallet->readPassword( accName + displayName_id,displayName ) ;

				item = new QTableWidgetItem() ;
				item->setText( displayName ) ;
				item->setTextAlignment( Qt::AlignCenter ) ;
				m_table->setItem( row,1,item ) ;

				m_wallet->readPassword( accName + labels_id,labels ) ;

				item = new QTableWidgetItem() ;
				item->setText( labels ) ;
				item->setTextAlignment( Qt::AlignCenter ) ;
				m_table->setItem( row,2,item ) ;

				m_wallet->readPassword( accName,passWord ) ;

				m_accounts.append( accounts( accName,passWord,displayName,labels ) ) ;
			}
		}
		if( row > 0 ){
			this->selectRow( row,true ) ;
		}
		this->show() ;
	}else{
		this->deleteLater() ;
	}
}

void kwalletmanager::getAccounts( void )
{
	m_wallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;
	connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened_1( bool ) ) ) ;
}

void kwalletmanager::walletOpened_1( bool walletOpened )
{
	if( walletOpened ){

		QString passWord ;
		QString labels ;
		QString displayName ;
		QString labels_id      = QString( LABEL_IDENTIFIER ) ;
		QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

		m_wallet->setFolder( m_passwordFolder ) ;

		QStringList accountNames = m_wallet->entryList() ;

		int j = accountNames.size() ;

		QVector<accounts> acc ;
		
		for( int i = 0 ; i < j ; i++ ){
			const QString& accName = accountNames.at( i ) ;
			if( accName.endsWith( labels_id ) || accName.endsWith( displayName_id ) ){
				;
			}else{
				m_wallet->readPassword( accName,passWord ) ;
				m_wallet->readPassword( accName + labels_id,labels ) ;
				m_wallet->readPassword( accName + displayName_id,displayName ) ;

				acc.append( accounts( accName,passWord,displayName,labels ) ) ;
			}
		}

		emit getAccountsInfo( acc ) ;
	}

	this->deleteLater() ;
}

kwalletmanager::~kwalletmanager()
{
	emit kwalletmanagerClosed() ;

	if( m_ui ){
		delete m_ui ;
	}
	if( m_wallet ){
		if( m_walletName == m_defaultWalletName ){
			/*
			 * This is our personal wallet and hence we close it when done with it
			 */
			m_wallet->closeWallet( m_walletName,true ) ;
		}else{
			/*
			 * we dont force close the wallet since we could be using a shared wallet and others may be
			 * using it or expect it to be open
			 */
			m_wallet->closeWallet( m_walletName,false ) ;
		}
		m_wallet->deleteLater() ;
	}
}

void kwalletmanager::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void kwalletmanager::HideUI()
{
	this->hide();

	m_wallet->removeFolder( m_passwordFolder ) ;
	m_wallet->createFolder( m_passwordFolder ) ;
	m_wallet->setFolder( m_passwordFolder ) ;

	QString labels_id  = QString( LABEL_IDENTIFIER ) ;
	QString display_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	int j = m_accounts.size() ;

	for( int i = 0 ; i < j ; i++ ){

		const accounts& acc    = m_accounts.at( i ) ;
		const QString& accName = acc.accountName() ;

		m_wallet->writePassword( accName,acc.passWord() ) ;
		m_wallet->writePassword( accName + display_id,acc.displayName() ) ;
		m_wallet->writePassword( accName + labels_id,acc.labels() ) ;
	}

	emit getAccountsInfo( m_accounts ) ;
	this->deleteLater() ;
}

void kwalletmanager::pushButtonAdd()
{
	addaccount * ac = new addaccount( this ) ;
	connect( ac,SIGNAL( addAccount( QString,QString,QString,QString ) ),
		 this,SLOT( addAccount( QString,QString,QString,QString ) ) ) ;
	ac->ShowUI() ;
}

void kwalletmanager::pushButtonClose()
{
	this->HideUI();
}

void kwalletmanager::tableItemClicked( QTableWidgetItem * item )
{
	m_deleteRow = item->row() ;

	QMenu m ;

	QAction * ac = new QAction( &m ) ;
	ac->setText( tr( "delete entry" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( deleteRow() ) ) ;

	m.addAction( ac ) ;

	ac = new QAction( &m ) ;
	ac->setText( tr( "edit entry" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( editEntry() ) ) ;

	m.addAction( ac ) ;

	m.exec( QCursor::pos() ) ;
}

void kwalletmanager::deleteRow()
{
	QMessageBox msg( this ) ;
	QTableWidgetItem * item = m_table->currentItem() ;
	QString acc = m_table->item( item->row(),0 )->text() ;

	msg.setText( tr( "are you sure you want to delete \"%1\" account?" ).arg( acc ) ) ;

	msg.addButton( tr( "yes" ),QMessageBox::YesRole ) ;
	QPushButton * no_button = msg.addButton( tr( "no" ),QMessageBox::NoRole ) ;
	msg.setDefaultButton( no_button ) ;

	msg.exec() ;

	if( msg.clickedButton() != no_button ){
		this->deleteRow( item->row() ) ;
	}
}

void kwalletmanager::deleteRow( int row )
{
	if( row < m_accounts.size() ){
		m_accounts.remove( row ) ;
	}
	if( row < m_table->rowCount() ){
		m_table->removeRow( row ) ;
	}
}

void kwalletmanager::editEntry()
{
	int row = m_table->currentRow() ;

	QString accName        = m_table->item( row,0 )->text() ;
	QString accPassword    = this->getPassWordFromAccount( accName ) ;
	QString accDisplayName = m_table->item( row,1 )->text() ;
	QString accLabels      = m_table->item( row,2 )->text() ;

	addaccount * ac = new addaccount( row,accName,accPassword,accDisplayName,accLabels,this ) ;
	connect( ac,SIGNAL( editAccount( int,QString,QString,QString,QString ) ),
		 this,SLOT( editAccount( int,QString,QString,QString,QString ) ) ) ;
	ac->ShowUI() ;
}

void kwalletmanager::addAccount( QString accountName,QString accountPassword,
				 QString displayName,QString accountLabels )
{
	m_accounts.append( accounts( accountName,accountPassword,displayName,accountLabels ) ) ;

	int row = m_table->rowCount() ;
	m_table->insertRow( row ) ;

	QTableWidgetItem * item ;

	item = new QTableWidgetItem() ;
	item->setText( accountName ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,0,item ) ;

	item = new QTableWidgetItem() ;
	item->setText( displayName ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,1,item ) ;

	item = new QTableWidgetItem() ;
	item->setText( accountLabels ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,2,item ) ;

	m_table->setCurrentCell( m_table->rowCount() - 1,2 ) ;
}

void kwalletmanager::editAccount( int row,QString accName,QString accPassword,
				  QString accDisplayName,QString accLabels )
{
	m_accounts.replace( row,accounts( accName,accPassword,accDisplayName,accLabels ) ) ;
	m_table->item( row,0 )->setText( accName ) ;
	m_table->item( row,1 )->setText( accDisplayName ) ;
	m_table->item( row,2 )->setText( accLabels ) ;
}

void kwalletmanager::selectRow( int row,bool highlight )
{
	int j = m_table->columnCount() ;
	for( int i = 0 ; i < j ; i++ ){
		m_table->item( row,0 )->setSelected( highlight ) ;
		m_table->item( row,1 )->setSelected( highlight ) ;
		m_table->item( row,2 )->setSelected( highlight ) ;
	}

	if( highlight && j > 1 ){
		m_table->setCurrentCell( row,j - 1 ) ;
	}
}

QString kwalletmanager::getPassWordFromAccount( QString accName )
{
	QString p ;
	int j = m_accounts.size() ;
	for( int i = 0 ; i < j ; i++ ){
		if( m_accounts.at( i ).accountName() == accName ){
			p = m_accounts.at( i ).passWord() ;
			break ;
		}
	}
	return p ;
}

void kwalletmanager::tableItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	if( current && previous ){
		if( current->row() == previous->row() ){
			this->selectRow( current->row(),true ) ;
			return ;
		}
	}
	if( current ){
		this->selectRow( current->row(),true ) ;
	}
	if( previous ){
		this->selectRow( previous->row(),false ) ;
	}
}
