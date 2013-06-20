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

kwalletmanager::kwalletmanager( KWallet::Wallet ** wallet,QString walletName,QWidget * parent ) : QDialog( parent ),
	m_ui( new Ui::kwalletmanager ),m_wallet( 0 ),m_wallet_p( wallet ),m_walletName( walletName )
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

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( tableItemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		 SLOT( tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	m_ui->tableWidget->setColumnWidth( 0,171 ) ;
	m_ui->tableWidget->setColumnWidth( 1,171 ) ;
	m_ui->tableWidget->setColumnWidth( 2,171 ) ;

	m_table = m_ui->tableWidget ;

	m_ui->pushButtonAccountAdd->setFocus() ;
}

void kwalletmanager::ShowUI()
{
	if( *m_wallet_p ){
		m_wallet = *m_wallet_p ;
	}else{
		*m_wallet_p = m_wallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;
		connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
	}
}

QVector<accounts> kwalletmanager::getAccounts( KWallet::Wallet * wallet )
{
	QString passWord ;
	QString name ;
	QString labels ;
	QString displayName ;
	QString labels_id   = QString( LABEL_IDENTIFIER ) ;
	QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	QVector<accounts> acc ;

	wallet->setFolder( wallet->PasswordFolder() ) ;

	QStringList userNames = wallet->entryList() ;

	int j = userNames.size() ;

	for( int i = 0 ; i < j ; i++ ){
		name = userNames.at( i ) ;
		if( name.endsWith( labels_id ) || name.endsWith( displayName_id ) ){
			;
		}else{
			wallet->readPassword( name,passWord ) ;
			wallet->readPassword( name + labels_id,labels ) ;
			wallet->readPassword( name + displayName_id,displayName ) ;

			acc.append( accounts( name,passWord,displayName,labels ) ) ;
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

	QStringList userNames = m_wallet->entryList() ;

	int j = userNames.size() ;
	QString passWord ;
	QString name ;

	m_accounts.clear();

	QTableWidgetItem * item ;
	QString labels ;
	QString labels_id = QString( LABEL_IDENTIFIER ) ;

	QString displayName ;
	QString displayName_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	int row ;

	for( int i = 0 ; i < j ; i++ ){
		name = userNames.at( i ) ;
		if( name.endsWith( labels_id ) || name.endsWith( displayName_id ) ){
			;
		}else{
			row = m_table->rowCount() ;

			m_table->insertRow( row ) ;

			item = new QTableWidgetItem() ;
			item->setText( name ) ;
			item->setTextAlignment( Qt::AlignCenter ) ;
			m_table->setItem( row,0,item ) ;

			m_wallet->readPassword( name + displayName_id,displayName ) ;

			item = new QTableWidgetItem() ;
			item->setText( displayName ) ;
			item->setTextAlignment( Qt::AlignCenter ) ;
			m_table->setItem( row,1,item ) ;

			m_wallet->readPassword( name + labels_id,labels ) ;

			item = new QTableWidgetItem() ;
			item->setText( labels ) ;
			item->setTextAlignment( Qt::AlignCenter ) ;
			m_table->setItem( row,2,item ) ;

			m_wallet->readPassword( name,passWord ) ;

			m_accounts.append( accounts( name,passWord,displayName,labels ) ) ;
		}
	}

	if( m_table->rowCount() > 0 ){
		int row = m_table->rowCount() - 1 ;
		m_table->item( row,0 )->setSelected( true ) ;
		m_table->item( row,1 )->setSelected( true ) ;
		m_table->item( row,2 )->setSelected( true ) ;

		m_table->setCurrentCell( row,2 ) ;
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
	QString FolderName = m_wallet->PasswordFolder() ;

	m_wallet->setFolder( FolderName ) ;

	m_wallet->removeFolder( FolderName ) ;
	m_wallet->createFolder( FolderName ) ;
	m_wallet->setFolder( m_wallet->PasswordFolder() ) ;

	QString user ;
	QString labels_id  = QString( LABEL_IDENTIFIER ) ;
	QString display_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

	int j = m_accounts.size() ;

	for( int i = 0 ; i < j ; i++ ){

		user = m_accounts.at( i ).accountName() ;

		m_wallet->writePassword( user,m_accounts.at( i ).passWord() ) ;

		m_wallet->writePassword( user + display_id,m_accounts.at( i ).displayName() ) ;

		m_wallet->writePassword( user + labels_id,m_accounts.at( i ).labels() ) ;
	}

	this->hide();
	this->deleteLater();
}

void kwalletmanager::pushButtonAdd()
{
	addaccount * ac = new addaccount( this ) ;
	connect( ac,SIGNAL( addAccount( QString,QString,QString,QString ) ),this,SLOT( addAccount( QString,QString,QString,QString ) ) ) ;
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

void kwalletmanager::addAccount( QString accountName,QString accountPassword,QString displayName,QString accountLabels )
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

void kwalletmanager::tableItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	int k ;
	if( current && previous ){
		if( current->row() == previous->row() ){
			return ;
		}
	}
	if( current ){
		k = current->row() ;
		m_table->item( k,0 )->setSelected( true ) ;
		m_table->item( k,1 )->setSelected( true ) ;
		m_table->item( k,2 )->setSelected( true ) ;

		m_table->setCurrentCell( k,2 ) ;
	}
	if( previous ){
		k = previous->row() ;
		m_table->item( k,0 )->setSelected( false ) ;
		m_table->item( k,1 )->setSelected( false ) ;
		m_table->item( k,2 )->setSelected( false ) ;
	}
}
