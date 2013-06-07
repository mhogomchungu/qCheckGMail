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

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( pushButtonAdd() ) ) ;
	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;
	connect( m_ui->checkBoxUnMaskPassphrase,SIGNAL( toggled( bool ) ),this,SLOT( checkBoxUnMaskPassphrase( bool ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( tableItemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		 SLOT( tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	m_ui->tableWidget->setColumnWidth( 0,270 ) ;
	m_ui->tableWidget->setColumnWidth( 1,292 ) ;

	m_table = m_ui->tableWidget ;

	m_ui->lineEditAccountName->setFocus() ;
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
	for( int i = 0 ; i < j ; i++ ){
		name = userNames.at( i ) ;
		m_wallet->readPassword( name,passWord ) ;
		m_table->insertRow( i ) ;
		item = new QTableWidgetItem() ;
		item->setText( name ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		m_table->setItem( i,0,item ) ;
		item = new QTableWidgetItem() ;
		item->setText( "<redacted>" ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		m_table->setItem( i,1,item ) ;

		m_accounts.append( accounts( name,passWord ) ) ;
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

	for( int i = 0 ; i < j ; i++ ){
		m_wallet->writePassword( m_accounts.at( i ).userName(),m_accounts.at( i ).passWord() ) ;
	}

	this->hide();
	this->deleteLater();
}

void configurationDialog::pushButtonAdd()
{
	QString username = m_ui->lineEditAccountName->text() ;
	QString password = m_ui->lineEditAccountPassphrase->text() ;

	if( username.isEmpty() || password.isEmpty() ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "ERROR: one or more required field is empty" ) ) ;
		msg.exec() ;
	}

	int row = m_table->rowCount() ;
	m_table->insertRow( row ) ;
	QTableWidgetItem * item ;

	item = new QTableWidgetItem() ;
	item->setText( username ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,0,item ) ;
	item = new QTableWidgetItem() ;
	item->setText( "<redacted>" ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_table->setItem( row,1,item ) ;

	m_accounts.append( accounts( username,password ) ) ;

	m_ui->lineEditAccountName->clear();
	m_ui->lineEditAccountPassphrase->clear();
	m_ui->lineEditAccountName->setFocus();
}

void configurationDialog::pushButtonClose()
{
	this->HideUI();
}

void configurationDialog::pushButtonDeleteEntry()
{

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
