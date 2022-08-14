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


#include "walletmanager.h"
#include "ui_walletmanager.h"

#include "task.hpp"

#include <utility>

static const auto LABEL_IDENTIFIER        = "-qCheckGMail-LABEL_ID" ;
static const auto DISPLAY_NAME_IDENTIFIER = "-qCheckGMail-DISPLAY_NAME_ID" ;
static const auto TOKEN_IDENTIFIER        = "-qCheckGMail-TOKEN_KEY_ID" ;

class account
{
public:
	static bool main( const QString& e )
	{
		const auto ids = { LABEL_IDENTIFIER,
				   DISPLAY_NAME_IDENTIFIER,
				   TOKEN_IDENTIFIER } ;

		for( const auto& it : ids ){

			if( e.endsWith( it ) ){

				return false ;
			}
		}

		return true ;
	}

	void add()
	{
		m_wallet->addKey( m_name       ,m_acc.accPassword ) ;
		m_wallet->addKey( m_labels     ,m_acc.accLabels ) ;
		m_wallet->addKey( m_token      ,m_acc.accRefreshToken ) ;
	}

	void remove()
	{
		m_wallet->deleteKey( m_name ) ;
		m_wallet->deleteKey( m_labels ) ;
		m_wallet->deleteKey( m_token ) ;
	}

	void replace()
	{
		this->remove() ;
		this->add() ;
	}

	accounts::entry entry( const QVector< std::pair< QString,QByteArray > >& e )
	{
		auto _entry = [ &e ]( const QString& acc )->const QByteArray&{

			for( const auto& it : e ){

				if( it.first == acc ){

					return it.second ;
				}
			}

			static QByteArray shouldNotGetHere ;
			return shouldNotGetHere ;
		} ;

		return { m_name,
			_entry( m_name ),
			_entry( m_labels ),
			_entry( m_token ) } ;
	}

	QVector< std::pair< QString,QByteArray > > readAll()
	{
		return Task::await( [ this ](){	return m_wallet->readAllKeyValues() ; } ) ;
	}

	account( LXQt::Wallet::Wallet * w,const accounts::entry& e = accounts::entry() ) :
		m_name( e.accName ),
		m_labels( m_name + LABEL_IDENTIFIER ),
		m_token( m_name + TOKEN_IDENTIFIER ),
		m_acc( e ),
		m_wallet( w )
	{
	}

	account( const QString& accName,
		 LXQt::Wallet::Wallet * w = nullptr,
		 const accounts::entry& e = accounts::entry() ) :
		m_name( accName ),
		m_labels( m_name + LABEL_IDENTIFIER ),
		m_token( m_name + TOKEN_IDENTIFIER ),
		m_acc( e ),
		m_wallet( w )
	{
	}
private:
	const QString m_name ;
	const QString m_labels ;
	const QString m_token ;

	const accounts::entry& m_acc ;

	LXQt::Wallet::Wallet * m_wallet ;
};

walletmanager::walletmanager( const QString& icon ) :
	m_icon( QString( ":/%1" ).arg( icon ) )
{
}

walletmanager::walletmanager( walletmanager::Wallet f ) :
	m_walletData( std::move( f ) )
{
}

walletmanager::walletmanager( const QString& icon,
			      walletmanager::Wallet e,
			      gmailauthorization::getAuth k,
			      addaccount::GMailInfo a ) :
	m_icon( QString( ":/%1" ).arg( icon ) ),
	m_walletData( std::move( e ) ),
	m_getAuthorization( std::move( k ) ),
	m_getAddr( std::move( a ) )
{
}

walletmanager::~walletmanager()
{
	m_walletData.closed() ;

        delete m_ui ;
}

void walletmanager::buildGUI()
{
	m_ui = new Ui::walletmanager ;
	m_ui->setupUi( this ) ;

        m_ui->pushButtonAccountAdd->setMinimumHeight( 31 ) ;
        m_ui->pushButtonClose->setMaximumHeight( 31 ) ;

	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setWindowIcon( QIcon( m_icon ) ) ;

	auto cm = static_cast< void( walletmanager::* )() >( &walletmanager::pushButtonAdd ) ;
	auto dm = static_cast< void( walletmanager::* )( QTableWidgetItem *,QTableWidgetItem * ) >( &walletmanager::tableItemChanged ) ;

	connect( m_ui->pushButtonAccountAdd,&QPushButton::clicked,this,cm ) ;
	connect( m_ui->pushButtonClose,&QPushButton::clicked,this,&walletmanager::pushButtonClose ) ;

	connect( m_ui->tableWidget,&QTableWidget::itemClicked,this,&walletmanager::tableItemClicked ) ;
	connect( m_ui->tableWidget,&QTableWidget::currentItemChanged,this,dm ) ;

	m_ui->tableWidget->setColumnWidth( 0,300 ) ;
	m_ui->tableWidget->horizontalHeader()->setStretchLastSection( true ) ;

	m_table = m_ui->tableWidget ;

	m_ui->pushButtonAccountAdd->setFocus() ;

	m_ui->groupBox->setTitle( QString() ) ;

	this->installEventFilter( this ) ;
}

void walletmanager::ShowUI()
{
	m_action = walletmanager::showAccountInfo ;
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setImage( QIcon( m_icon ) ) ;

	this->openWallet() ;
}

void walletmanager::getAccounts( void )
{
	m_action = walletmanager::getAccountInfo ;
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;

	this->openWallet() ;
}

void walletmanager::changeWalletPassword()
{
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setImage( QIcon( m_icon ) ) ;
	m_wallet->setParent( this ) ;

	auto s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;

	if( m_wallet->open( s,"qCheckGMail",this ) ){

		m_wallet->changeWalletPassWord( s,"qCheckGMail",[ this ]( bool e ){

			Q_UNUSED( e )
			this->hide() ;
			this->deleteLater() ;
		} ) ;
	}else{
		this->hide() ;
		this->deleteLater() ;
	}
}

const accounts& walletmanager::addEntry( const accounts& acc )
{
        auto row = m_table->rowCount() ;

	m_table->insertRow( row ) ;

	auto _add_item = []( const QString& e ){

		auto item = new QTableWidgetItem() ;

		item->setText( e ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;

		return item ;
	} ;

	m_table->setItem( row,0,_add_item( acc.accountName() ) ) ;
	m_table->setItem( row,1,_add_item( util::namesFromJson( acc.labels() ) ) ) ;

	return acc ;
}

void walletmanager::readAccountInfo()
{
	m_accounts.clear() ;

	auto e = account( m_wallet.get() ).readAll() ;

	for( const auto& it : e ){

		const auto& r = it.first ;

		if( account::main( r ) ){

			m_accounts.append( account( r ).entry( e ) ) ;
		}
	}
}

void walletmanager::openWallet()
{
	auto s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;

	m_wallet->setParent( this ) ;

	m_wallet->open( s,"qCheckGMailv2",[ this ]( bool walletOpened ){

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

				m_walletData.data( std::move( m_accounts ) ) ;
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
	} ) ;
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

                        auto keyEvent = static_cast< QKeyEvent* >( event ) ;

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
	m_walletData.data( std::move( m_accounts ) ) ;
	this->hide() ;
	this->deleteLater() ;
}

void walletmanager::pushButtonAdd( accounts::entry&& e )
{
	m_accEntry = std::move( e ) ;

	Task::run( [ this ](){

		account( m_wallet.get(),m_accEntry ).add() ;

	} ).then( [ this ](){

		m_accounts.append( this->addEntry( m_accEntry ) ) ;

		this->selectLastRow() ;
		this->enableAll() ;
	} ) ;
}

void walletmanager::pushButtonAdd()
{
	this->disableAll() ;

	class meaw : public addaccount::actions
	{
	public:
		meaw( walletmanager * m ) : m_parent( m )
		{
		}
		void cancel() override
		{
			m_parent->enableAll() ;
		}
		void results( accounts::entry&& e ) override
		{
			m_parent->pushButtonAdd( std::move( e ) ) ;
		}
	private:
		walletmanager * m_parent ;
	};

	addaccount::instance( this,m_getAuthorization,{ util::type_identity< meaw >(),this },m_getAddr ) ;
}

void walletmanager::tableItemClicked( QTableWidgetItem * item )
{
	m_deleteRow = item->row() ;

	QMenu m ;

        m.addAction( [ & ](){

                auto ac = new QAction( &m ) ;
		ac->setText( tr( "Delete Entry" ) ) ;
		connect( ac,&QAction::triggered,this,&walletmanager::deleteAccount ) ;

                return ac ;
	}() ) ;

	m.exec( QCursor::pos() ) ;
}

void walletmanager::deleteAccount( bool )
{
	auto item = m_table->currentItem() ;
	m_row = item->row() ;
        auto accName = m_table->item( m_row,0 )->text() ;

	QMessageBox msg( this ) ;
	msg.setText( tr( "Are You Sure You Want To Delete \"%1\" Account?" ).arg( accName ) ) ;
	msg.addButton( tr( "&Yes" ),QMessageBox::YesRole ) ;
	auto no_button = msg.addButton( tr( "&No" ),QMessageBox::NoRole ) ;
	msg.setDefaultButton( no_button ) ;

	this->disableAll() ;

	msg.exec() ;

	if( msg.clickedButton() != no_button ){

		if( m_row < m_accounts.size() && m_row < m_table->rowCount() ){

                        Task::run( [ & ](){

				account( accName,m_wallet.get() ).remove() ;

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

void walletmanager::selectRow( int row,bool highlight )
{
	if( row >= 0 ){

		int j = m_table->columnCount() ;

                for( decltype( j ) i = 0 ; i < j ; i++ ){

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
	if( current && previous && current->row() == previous->row() ){

		this->selectRow( current->row(),true ) ;
	}else{
		if( current ){

			this->selectRow( current->row(),true ) ;
		}

		if( previous ){

			this->selectRow( previous->row(),false ) ;
		}
	}
}

walletmanager::wallet::~wallet()
{
}
