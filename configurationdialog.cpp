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


#include "configurationdialog.h"
#include "ui_configurationdialog.h"

configurationDialog::configurationDialog( KWallet::Wallet ** wallet,QWidget * parent ) : QDialog( parent ),
	m_ui( new Ui::configurationDialog ),m_wallet( 0 ),m_wallet_p( wallet )
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

	m_ui->tableWidget->setColumnWidth( 0,177 ) ;
	m_ui->tableWidget->setColumnWidth( 1,251 ) ;

	m_table = m_ui->tableWidget ;

	m_ui->pushButtonAccountAdd->setFocus() ;

	m_ui->lineEditInterval->setEnabled( false ) ;
}

void configurationDialog::ShowUI()
{
	if( *m_wallet_p ){
		m_wallet = *m_wallet_p ;
	}else{
		*m_wallet_p = m_wallet = KWallet::Wallet::openWallet( "qCheckGmail",0,KWallet::Wallet::Asynchronous ) ;
		connect( m_wallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
	}
}

void configurationDialog::walletOpened( bool b )
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
	QString labels_id = QString( "-labels" ) ;

	int row ;

	for( int i = 0 ; i < j ; i++ ){
		name = userNames.at( i ) ;

		if( name.endsWith( labels_id ) ){
			continue ;
		}

		row = m_table->rowCount() ;

		m_wallet->readPassword( name,passWord ) ;
		m_table->insertRow( row ) ;

		item = new QTableWidgetItem() ;
		item->setText( name ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		m_table->setItem( row,0,item ) ;

		m_wallet->readPassword( name + labels_id,labels ) ;

		item = new QTableWidgetItem() ;
		item->setTextAlignment( Qt::AlignCenter ) ;

		if( labels.isEmpty() ){
			item->setText( "" ) ;
			m_accounts.append( accounts( name,passWord ) ) ;
		}else{
			item->setText( labels ) ;
			m_accounts.append( accounts( name,passWord,labels.split( "," ) ) ) ;
		}

		m_table->setItem( row,1,item ) ;
	}

	if( m_table->rowCount() > 0 ){
		int row = m_table->rowCount() - 1 ;
		m_table->item( row,0 )->setSelected( true ) ;
		m_table->item( row,1 )->setSelected( true ) ;
		m_table->setCurrentCell( row,1 ) ;
	}

	this->show();
}

configurationDialog::~configurationDialog()
{
	emit configurationDialogClosed() ;
	delete m_ui ;
}

void configurationDialog::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void configurationDialog::HideUI()
{
	QString FolderName = m_wallet->PasswordFolder() ;

	m_wallet->setFolder( FolderName ) ;

	int j = m_accounts.size() ;

	m_wallet->removeFolder( FolderName ) ;
	m_wallet->createFolder( FolderName ) ;
	m_wallet->setFolder( m_wallet->PasswordFolder() ) ;

	QString user ;
	QString labels ;
	QString labels_id = QString( "-labels" ) ;
	QStringList list ;

	for( int i = 0 ; i < j ; i++ ){
		
		user = m_accounts.at( i ).userName() ;
		m_wallet->writePassword( user,m_accounts.at( i ).passWord() ) ;

		list = m_accounts.at( i ).LabelUrls() ;
		list.removeAt( 0 ) ;
		int j = list.size() ;
		
		if( j == 0 ){
			m_wallet->writePassword( user + labels_id,labels ) ;
		}else{
			labels = list.at( 0 ).split( "/" ).last() ;
			for( int i = 1 ; i < j ; i++ ){
				labels = labels + QString( "," ) + list.at( i ).split( "/" ).last() ;
			}
			m_wallet->writePassword( user + labels_id,labels ) ;
		}
	}

	//qDebug() << m_ui->tableWidget->columnWidth( 0 ) << m_ui->tableWidget->columnWidth( 1 ) ;
	this->hide();
	this->deleteLater();
}

void configurationDialog::pushButtonAdd()
{
	addaccount * ac = new addaccount( this ) ;
	connect( ac,SIGNAL( addAccount( QString, QString, QString ) ),this,SLOT( addAccount( QString,QString,QString ) ) ) ;
	ac->ShowUI() ;
}

void configurationDialog::pushButtonClose()
{
	this->HideUI();
}

void configurationDialog::pushButtonDeleteEntry()
{
	m_deleteRow = m_table->currentRow() ;
	this->deleteRow();
}

void configurationDialog::checkBoxUnMaskPassphrase( bool b )
{
	int j = m_accounts.size() ;
	
	if( j == m_table->rowCount() ){
		if( b ){
			for( int i = 0 ; i < j ; i++ ){
				m_table->item( i,1 )->setText( m_accounts.at( i ).passWord() ) ;
			}
		}else{
			for( int i = 0 ; i < j ; i++ ){
				m_table->item( i,1 )->setText( "<redacted>" ) ;
			}
		}
	}else{
		qDebug() << "error table size is different from number of accounts";
		return ;
	}
}

void configurationDialog::tableItemClicked( QTableWidgetItem * item )
{
	m_deleteRow = item->row() ;

	QMenu m ;

	QAction * ac = new QAction( &m ) ;
	ac->setText( tr( "delete entry" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( deleteRow() ) ) ;

	m.addAction( ac ) ;

	m.exec( QCursor::pos() ) ;
}

void configurationDialog::deleteRow()
{
	m_table->removeRow( m_deleteRow ) ;

	m_accounts.remove( m_deleteRow ) ;
}

void configurationDialog::addAccount( QString accountName,QString accountPassword,QString accountLabels )
{
	if( accountLabels.isEmpty() ){
		m_accounts.append( accounts( accountName,accountPassword ) ) ;
	}else{
		m_accounts.append( accounts( accountName,accountPassword,accountLabels.split( "," ) ) ) ;
	}

	int row = m_table->rowCount() ;
	m_table->insertRow( row ) ;
	QTableWidgetItem * item ;

	item = new QTableWidgetItem() ;
	item->setText( accountName ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,0,item ) ;

	item = new QTableWidgetItem() ;
	item->setText( accountLabels ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,1,item ) ;
}

void configurationDialog::tableItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
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
		m_table->setCurrentCell( k,1 ) ;
	}
	if( previous ){
		k = previous->row() ;
		m_table->item( k,0 )->setSelected( false ) ;
		m_table->item( k,1 )->setSelected( false ) ;
	}
}
