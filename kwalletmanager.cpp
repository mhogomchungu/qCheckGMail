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

#define LABEL_IDENTIFIER "-qCheckGMail-LABEL_ID"
#define DISPLAY_NAME_IDENTIFIER "-qCheckGMail-DISPLAY_NAME_ID"

kwalletmanager::kwalletmanager( KWallet::Wallet ** wallet,QString walletName,QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::kwalletmanager ),m_wallet( 0 ),
	m_wallet_p( wallet ),m_walletName( walletName )
{
	m_ui->setupUi( this );
	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );

	if( parent ){
		this->setFont( parent->font() );
	}

	connect( m_ui->pushButtonAccountAdd,SIGNAL( clicked() ),this,SLOT( pushButtonAdd() ) ) ;
	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;
	connect( m_ui->pushButtonAccountDelete,SIGNAL( clicked() ),this,SLOT( pushButtonDeleteEntry() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( tableItemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		 SLOT( tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	m_ui->tableWidget->setColumnWidth( 0,171 ) ;
	m_ui->tableWidget->setColumnWidth( 1,171 ) ;
	m_ui->tableWidget->setColumnWidth( 2,171 ) ;

	m_table = m_ui->tableWidget ;

	m_ui->pushButtonAccountDelete->setVisible( false ) ;
	m_ui->pushButtonAccountAdd->setFocus() ;
}

void kwalletmanager::ShowUI()
{
	if( *m_wallet_p ){
		m_wallet = *m_wallet_p ;
	}else{
		*m_wallet_p = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;
		m_wallet = *m_wallet_p ;
		connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
	}
}

QVector<accounts> kwalletmanager::getAccounts( KWallet::Wallet * wallet )
{
	QString passWord ;
	QString labels ;
	QString displayName ;
	QString labels_id      = QString( LABEL_IDENTIFIER ) ;
	QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	QVector<accounts> acc ;

	wallet->setFolder( wallet->PasswordFolder() ) ;

	QStringList accountNames = wallet->entryList() ;

	int j = accountNames.size() ;

	for( int i = 0 ; i < j ; i++ ){
		const QString& accName = accountNames.at( i ) ;
		if( accName.endsWith( labels_id ) || accName.endsWith( displayName_id ) ){
			;
		}else{
			wallet->readPassword( accName,passWord ) ;
			wallet->readPassword( accName + labels_id,labels ) ;
			wallet->readPassword( accName + displayName_id,displayName ) ;

			acc.append( accounts( accName,passWord,displayName,labels ) ) ;
		}
	}

	return acc ;
}

void kwalletmanager::walletOpened( bool b )
{
	if( !b ){
		this->HideUI();
		return ;
	}

	m_wallet->setFolder( m_wallet->PasswordFolder() ) ;

	QStringList accountNames = m_wallet->entryList() ;

	int j = accountNames.size() ;
	QString passWord ;

	m_accounts.clear();

	QTableWidgetItem * item ;
	QString labels ;
	QString labels_id = QString( LABEL_IDENTIFIER ) ;

	QString displayName ;
	QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	int row ;

	for( int i = 0 ; i < j ; i++ ){
		const QString& accName = accountNames.at( i ) ;
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

	if( m_table->rowCount() > 0 ){
		this->selectRow( m_table->rowCount() - 1,true ) ;
	}

	this->show();
}

kwalletmanager::~kwalletmanager()
{
	emit kwalletmanagerClosed() ;
	delete m_ui ;
}

void kwalletmanager::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void kwalletmanager::HideUI()
{
	this->hide();

	QString passwordFolder = m_wallet->PasswordFolder() ;

	m_wallet->removeFolder( passwordFolder ) ;
	m_wallet->createFolder( passwordFolder ) ;
	m_wallet->setFolder( passwordFolder ) ;

	QString labels_id  = QString( LABEL_IDENTIFIER ) ;
	QString display_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	int j = m_accounts.size() ;

	for( int i = 0 ; i < j ; i++ ){
		const accounts& acc = m_accounts.at( i ) ;
		const QString& accName = acc.accountName() ;
		m_wallet->writePassword( accName,acc.passWord() ) ;
		m_wallet->writePassword( accName + display_id,acc.displayName() ) ;
		m_wallet->writePassword( accName + labels_id,acc.labels() ) ;
	}

	this->deleteLater();
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

void kwalletmanager::pushButtonDeleteEntry()
{
	m_deleteRow = m_table->currentRow() ;
	this->deleteRow();
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
	if( m_table->rowCount() == -1 ){
		return ;
	}
	if( m_table->rowCount() >=  m_deleteRow ){
		m_table->removeRow( m_deleteRow ) ;
	}
	if( m_accounts.size() > 0 ){
		if( m_accounts.size() >= m_deleteRow ){
			m_accounts.remove( m_deleteRow ) ;
		}
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
	m_table->item( row,0 )->setText( accName ) ;
	m_table->item( row,1 )->setText( accDisplayName ) ;
	m_table->item( row,2 )->setText( accLabels ) ;

	int j = m_accounts.size() ;
	for( int i = 0 ; i < j ; i++ ){
		if( m_accounts.at( i ).accountName() == accName ){
			m_accounts.replace( i,accounts( accName,accPassword,accDisplayName,accLabels ) ) ;
			break ;
		}
	}
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
