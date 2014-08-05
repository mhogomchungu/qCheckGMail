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


#include "walletmanager.h"
#include "ui_walletmanager.h"

#include "../lxqt_wallet/frontend/task.h"

namespace Task = LxQt::Wallet::Task ;

#include <utility>

#define LABEL_IDENTIFIER        "-qCheckGMail-LABEL_ID"
#define DISPLAY_NAME_IDENTIFIER "-qCheckGMail-DISPLAY_NAME_ID"

walletmanager::walletmanager( const QString& icon,QDialog * parent ) :QDialog( parent ),m_ui( 0 ),m_wallet( 0 )
{
	m_icon = QString( ":/%1" ).arg( icon ) ;
}

walletmanager::~walletmanager()
{
	emit walletmanagerClosed() ;
	m_wallet->deleteLater() ;
	delete m_ui ;
}

void walletmanager::buildGUI()
{
	m_ui = new Ui::walletmanager ;
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setWindowIcon( QIcon( m_icon ) ) ;

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

	m_ui->groupBox->setTitle( QString() ) ;

	this->installEventFilter( this ) ;
}

void walletmanager::ShowUI()
{
	m_action = walletmanager::showAccountInfo ;
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setInterfaceObject( this ) ;
	m_wallet->setImage( m_icon ) ;
	QString s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
	m_wallet->open( s,"qCheckGMail" ) ;
}

void walletmanager::getAccounts( void )
{
	m_action = walletmanager::getAccountInfo ;
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setInterfaceObject( this ) ;
	QString s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
	m_wallet->open( s,"qCheckGMail" ) ;
}

void walletmanager::changeWalletPassword()
{
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setInterfaceObject( this ) ;
	m_wallet->setImage( m_icon ) ;
	QString s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
	m_wallet->changeWalletPassWord( s,"qCheckGMail" ) ;
}

void walletmanager::walletpassWordChanged( bool passwordChanged )
{
	Q_UNUSED( passwordChanged ) ;
	this->deleteLater() ;
}

void walletmanager::addEntry( const accounts& acc )
{
	QTableWidgetItem * item ;

	int row = m_table->rowCount() ;

	m_table->insertRow( row ) ;

	item = new QTableWidgetItem() ;
	item->setText( acc.accountName() ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,0,item ) ;

	item = new QTableWidgetItem() ;
	item->setText( acc.displayName() ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,1,item ) ;

	item = new QTableWidgetItem() ;
	item->setText( acc.labels() ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,2,item ) ;
}

void walletmanager::readAccountInfo()
{
	using wallet = QVector<LxQt::Wallet::walletKeyValues> ;

	m_accounts.clear() ;

	auto _getAccEntry = []( const QString& acc,const wallet& entries ){

		for( const auto& it : entries ){
			if( it.getKey() == acc ){
				return it.getValue() ;
			}
		}
		static QByteArray shouldNotGetHere ;
		return shouldNotGetHere ;
	} ;

	QString labels_id  = LABEL_IDENTIFIER ;
	QString display_id = DISPLAY_NAME_IDENTIFIER ;

	auto entries = Task::await<wallet>( [ this ](){ return m_wallet->readAllKeyValues() ; } ) ;

	for( const auto& it : entries ){

		const QString& accName = it.getKey() ;
		bool r = accName.endsWith( labels_id ) || accName.endsWith( display_id ) ;

		if( r == false ){

			const auto& passWord    = _getAccEntry( accName,entries ) ;
			const auto& labels      = _getAccEntry( accName + labels_id,entries ) ;
			const auto& displayName = _getAccEntry( accName + display_id,entries ) ;

			m_accounts.append( accounts( accName,passWord,displayName,labels ) ) ;
		}
	}
}

void walletmanager::walletIsOpen( bool walletOpened )
{
	if( walletOpened ){

		switch( m_action ){

		case walletmanager::showAccountInfo :

			this->buildGUI() ;
			this->disableAll() ;
			this->show() ;

			this->readAccountInfo() ;

			for( const auto& it : m_accounts ){

				this->addEntry( it ) ;
			}

			this->selectLastRow() ;
			this->enableAll() ;

			break ;
		case walletmanager::getAccountInfo :

			this->readAccountInfo() ;

			emit getAccountsInfo( m_accounts ) ;
			this->deleteLater() ;

			break ;
		default:
			/*
			 * we dont get here
			 */
			this->deleteLater() ;
		}
	}else{
		this->deleteLater() ;
	}
}

void walletmanager::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

bool walletmanager::eventFilter( QObject * watched,QEvent * event )
{
	if( watched == this ){
		if( event->type() == QEvent::KeyPress ){
			QKeyEvent * keyEvent = static_cast< QKeyEvent* >( event ) ;
			if( keyEvent->key() == Qt::Key_Escape ){
				this->HideUI() ;
				return true ;
			}
		}
	}

	return false ;
}

void walletmanager::enableAll()
{
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->pushButtonAccountAdd->setEnabled( true ) ;
	m_ui->pushButtonClose->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_table->setFocus() ;
}

void walletmanager::disableAll()
{
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->pushButtonAccountAdd->setEnabled( false ) ;
	m_ui->pushButtonClose->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
}

void walletmanager::pushButtonClose()
{
	this->HideUI() ;
}

void walletmanager::HideUI()
{
	emit getAccountsInfo( m_accounts ) ;
	this->hide() ;
	this->deleteLater() ;
}

void walletmanager::pushButtonAdd()
{
	this->disableAll() ;
	auto ac = new addaccount( this ) ;
	connect( ac,SIGNAL( addAccount( QString,QString,QString,QString ) ),
		 this,SLOT( addAccount( QString,QString,QString,QString ) ) ) ;
	connect( ac,SIGNAL( cancelSignal() ),this,SLOT( enableAll() ) ) ;
	ac->ShowUI() ;
}

void walletmanager::addAccount( QString accName,QString accPassword,
				QString accDisplayName,QString accLabels )
{
	m_accName        = std::move( accName ) ;
	m_accPassWord    = std::move( accPassword ) ;
	m_accLabels      = std::move( accLabels ) ;
	m_accDisplayName = std::move( accDisplayName ) ;

	Task::run( [ this ](){

		QString labels_id  = m_accName + LABEL_IDENTIFIER ;
		QString display_id = m_accName + DISPLAY_NAME_IDENTIFIER ;

		m_wallet->addKey( m_accName,m_accPassWord.toLatin1() ) ;
		m_wallet->addKey( labels_id,m_accLabels.toLatin1() ) ;
		m_wallet->addKey( display_id,m_accDisplayName.toLatin1() ) ;

	} ).then( [ this ](){

		accounts acc( m_accName,m_accPassWord,m_accDisplayName,m_accLabels ) ;

		m_accounts.append( acc ) ;

		this->addEntry( acc ) ;

		this->selectLastRow() ;
		this->enableAll() ;
	} ) ;
}

void walletmanager::tableItemClicked( QTableWidgetItem * item )
{
	m_deleteRow = item->row() ;

	QMenu m ;

	auto ac = new QAction( &m ) ;
	ac->setText( tr( "delete entry" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( deleteAccount() ) ) ;

	m.addAction( ac ) ;

	ac = new QAction( &m ) ;
	ac->setText( tr( "edit entry" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( editAccount() ) ) ;

	m.addAction( ac ) ;

	m.exec( QCursor::pos() ) ;
}

void walletmanager::deleteAccount()
{
	auto item = m_table->currentItem() ;
	m_row = item->row() ;
	m_accName = m_table->item( m_row,0 )->text() ;

	QMessageBox msg( this ) ;
	msg.setText( tr( "are you sure you want to delete \"%1\" account?" ).arg( m_accName ) ) ;
	msg.addButton( tr( "yes" ),QMessageBox::YesRole ) ;
	QPushButton * no_button = msg.addButton( tr( "no" ),QMessageBox::NoRole ) ;
	msg.setDefaultButton( no_button ) ;

	this->disableAll() ;

	msg.exec() ;

	if( msg.clickedButton() != no_button ){

		if( m_row < m_accounts.size() && m_row < m_table->rowCount() ){

			Task::run( [ this ](){

				QString labels_id  = m_accName + LABEL_IDENTIFIER ;
				QString display_id = m_accName + DISPLAY_NAME_IDENTIFIER ;

				m_wallet->deleteKey( m_accName ) ;
				m_wallet->deleteKey( labels_id ) ;
				m_wallet->deleteKey( display_id ) ;

			} ).then( [ this ](){

				m_accounts.remove( m_row ) ;
				m_table->removeRow( m_row ) ;
				this->enableAll() ;
			} ) ;

		}else{
			this->enableAll() ;
		}
	}else{
		this->enableAll() ;
	}
}

void walletmanager::editAccount()
{
	int row = m_table->currentRow() ;

	auto _getPassWord = [ this ]( const QString& accName ){

		for( const auto& it : m_accounts ){
			if( it.accountName() == accName ){
				return it.passWord() ;
			}
		}
		static QString shouldNotGetHere ;
		return shouldNotGetHere ;
	} ;

	QString accName        = m_table->item( row,0 )->text() ;
	QString accPassword    = _getPassWord( accName ) ;
	QString accDisplayName = m_table->item( row,1 )->text() ;
	QString accLabels      = m_table->item( row,2 )->text() ;

	this->disableAll() ;

	auto ac = new addaccount( row,accName,accPassword,accDisplayName,accLabels,this ) ;
	connect( ac,SIGNAL( editAccount( int,QString,QString,QString,QString ) ),
		 this,SLOT( editAccount( int,QString,QString,QString,QString ) ) ) ;
	connect( ac,SIGNAL( cancelSignal() ),this,SLOT( enableAll() ) ) ;
	ac->ShowUI() ;
}

void walletmanager::editAccount( int row,QString accName,QString accPassword,
				  QString accDisplayName,QString accLabels )
{
	m_row            = std::move( row ) ;
	m_accName        = std::move( accName ) ;
	m_accPassWord    = std::move( accPassword ) ;
	m_accLabels      = std::move( accLabels ) ;
	m_accDisplayName = std::move( accDisplayName ) ;

	Task::run( [ this ](){

		QString labels_id  = m_accName + LABEL_IDENTIFIER ;
		QString display_id = m_accName + DISPLAY_NAME_IDENTIFIER ;

		m_wallet->deleteKey( m_accName ) ;
		m_wallet->deleteKey( labels_id ) ;
		m_wallet->deleteKey( display_id ) ;

		m_wallet->addKey( m_accName,m_accPassWord.toLatin1() ) ;
		m_wallet->addKey( labels_id,m_accLabels.toLatin1() ) ;
		m_wallet->addKey( display_id,m_accDisplayName.toLatin1() ) ;

	} ).then( [ this ](){

		accounts acc( m_accName,m_accPassWord,m_accDisplayName,m_accLabels ) ;

		m_accounts.replace( m_row,acc ) ;

		m_table->item( m_row,0 )->setText( m_accName ) ;
		m_table->item( m_row,1 )->setText( m_accDisplayName ) ;
		m_table->item( m_row,2 )->setText( m_accLabels ) ;

		this->enableAll() ;
	} ) ;
}

void walletmanager::selectRow( int row,bool highlight )
{
	if( row >= 0 ){
		int j = m_table->columnCount() ;
		for( int i = 0 ; i < j ; i++ ){
			m_table->item( row,i )->setSelected( highlight ) ;
		}

		if( highlight && j > 0 ){
			m_table->setCurrentCell( row,j - 1 ) ;
		}
	}
}

void walletmanager::selectLastRow()
{
	this->selectRow( m_table->rowCount() - 1,true ) ;
}

void walletmanager::tableItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
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
