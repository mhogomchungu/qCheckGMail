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

#include <QJsonDocument>
#include <QJsonObject>

class account
{
public:
	static void add( LXQt::Wallet::Wallet * w,const accounts::entry& e )
	{
		QJsonObject obj ;

		obj.insert( "accName",e.accName ) ;
		obj.insert( "labels",e.accLabels ) ;
		obj.insert( "refreshToken",e.accRefreshToken ) ;

		auto id = e.accName + account::identifier() ;

		w->addKey( id,QJsonDocument( obj ).toJson( QJsonDocument::JsonFormat::Compact ) ) ;
	}

	static void replace( LXQt::Wallet::Wallet * w,const accounts::entry& e )
	{
		account::remove( e.accName,w ) ;
		account::add( w,e ) ;
	}

	static void remove( const QString& accName,LXQt::Wallet::Wallet * w )
	{
		w->deleteKey( accName + account::identifier() ) ;
	}

	static void readAll( LXQt::Wallet::Wallet * w,QVector< accounts >& acc )
	{
		auto m = Task::await( [ & ](){ return w->readAllKeyValues() ; } ) ;

		QJsonParseError err ;

		QString id = account::identifier() ;

		for( const auto& it : m ){

			if( it.first.endsWith( id ) ){

				auto m = QJsonDocument::fromJson( it.second,&err ) ;

				if( err.error == QJsonParseError::NoError ){

					QJsonObject obj = m.object() ;

					accounts::entry m{ obj.value( "accName" ).toString(),
							   obj.value( "labels" ).toString(),
							   obj.value( "refreshToken" ).toString() } ;

					acc.append( std::move( m ) ) ;
				}
			}
		}

		std::sort( acc.begin(),acc.end(),[]( const accounts& acc,const accounts& xcc ){

			return acc.accountName().length() < xcc.accountName().length() ;
		} ) ;
	}
private:
	static const char * identifier()
	{
		return "-qCheckGMail2-ID" ;
	}
} ;

walletmanager::walletmanager( const QString& icon,settings& s ) :
	m_icon( QString( ":/%1" ).arg( icon ) ),m_settings( s )
{
}

walletmanager::walletmanager( walletmanager::Wallet f,settings& s ) :
	m_walletData( std::move( f ) ),m_settings( s )
{
}

walletmanager::walletmanager( const QString& icon,
			      settings& s,
			      walletmanager::Wallet e,
			      gmailauthorization::getAuth k,
			      addaccount::GMailInfo a ) :
	m_icon( QString( ":/%1" ).arg( icon ) ),
	m_walletData( std::move( e ) ),
	m_getAuthorization( std::move( k ) ),
	m_getAccountInfo( std::move( a ) ),
	m_settings( s )
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

	m_ui->labelNetworkWarning->setVisible( false ) ;

	this->installEventFilter( this ) ;
}

void walletmanager::ShowUI()
{
	m_action = walletmanager::accountOperation::showAccountInfo ;
	m_wallet = m_settings.secureStorageSystem() ;
	m_wallet->setImage( QIcon( m_icon ) ) ;

	this->openWallet() ;
}

void walletmanager::getAccounts( void )
{
	m_action = walletmanager::accountOperation::getAccountInfo ;
	m_wallet = m_settings.secureStorageSystem() ;

	this->openWallet() ;
}

void walletmanager::changeWalletPassword()
{
	m_wallet = m_settings.secureStorageSystem() ;
	m_wallet->setImage( QIcon( m_icon ) ) ;
	m_wallet->setParent( this ) ;

	auto walletName = m_settings.walletName( m_wallet->backEnd() ) ;
	auto appName = m_settings.applictionName() ;

	if( m_wallet->open( walletName,appName,this ) ){

		m_wallet->changeWalletPassWord( walletName,appName,[ this ]( bool ){

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
	account::readAll( m_wallet.get(),m_accounts ) ;
}

void walletmanager::openWallet()
{
	auto s = m_settings.walletName( m_wallet->backEnd() ) ;

	m_wallet->setParent( this ) ;

	m_wallet->open( s,m_settings.applictionName(),[ this ]( bool walletOpened ){

		if( walletOpened ){

			switch( m_action ){

			case walletmanager::accountOperation::showAccountInfo :

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
			case walletmanager::accountOperation::getAccountInfo :

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

		account::add( m_wallet.get(),m_accEntry ) ;

	} ).then( [ this ](){

		m_accounts.append( this->addEntry( m_accEntry ) ) ;

		this->selectLastRow() ;
		this->enableAll() ;
	} ) ;
}

void walletmanager::editAccount( const QString& accName,const QString& labels,addaccount::labels&& l,int row )
{
	if( !l.entries.isEmpty() && row < m_accounts.size() ){

		m_accounts[ row ].updateAccountInfo( accName,util::labelsToJson( labels,l.entries ) ) ;

		Task::run( [ this,row ](){

			account::replace( m_wallet.get(),m_accounts[ row ].data() ) ;

		} ).then( [ this,row,accName,labels ](){

			m_table->item( row,0 )->setText( accName ) ;
			m_table->item( row,1 )->setText( labels ) ;

			this->enableAll() ;
		} ) ;
	}else{
		this->enableAll() ;
	}
}

void walletmanager::editAccount( int row,addaccount::labels&& l )
{
	class meaw : public addaccount::actions
	{
	public:
		meaw( walletmanager * m,addaccount::labels&& l,int row ) :
			m_parent( m ),m_labels( std::move( l ) ),m_row( row )
		{
		}
		void cancel() override
		{
			m_parent->enableAll() ;
		}
		void edit( const QString& accName,const QString& labels ) override
		{
			m_parent->editAccount( accName,labels,std::move( m_labels ),m_row ) ;
		}
	private:
		walletmanager * m_parent ;
		addaccount::labels m_labels ;
		int m_row ;
	};

	m_ui->labelNetworkWarning->setVisible( false ) ;

	addaccount::instance( this,
			      m_settings,
			      m_accounts[ row ].data(),
			      m_getAuthorization,
			      { util::type_identity< meaw >(),this,std::move( l ),row },
			      m_getAccountInfo ) ;
}

void walletmanager::editEntryLabels()
{
	m_ui->labelNetworkWarning->setVisible( true ) ;

	this->disableAll() ;
	auto item = m_table->currentItem() ;
	auto row = item->row() ;
	auto accName = m_table->item( row,0 )->text().toUtf8() ;

	class meaw : public addaccount::gmailAccountInfo
	{
	public:
		meaw( walletmanager * w,int row ) : m_parent( w ),m_row( row )
		{
		}
		void operator()( addaccount::labels l ) override
		{
			m_parent->editAccount( m_row,std::move( l ) ) ;
		}
	private:
		walletmanager * m_parent ;
		int m_row ;
	} ;

	m_getAccountInfo( accName,{ util::type_identity< meaw >(),this,row } ) ;
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

	addaccount::instance( this,m_settings,m_getAuthorization,{ util::type_identity< meaw >(),this },m_getAccountInfo ) ;
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

	m.addAction( [ & ](){

		auto ac = new QAction( &m ) ;
		ac->setText( tr( "Edit Entry" ) ) ;
		connect( ac,&QAction::triggered,this,&walletmanager::editEntryLabels ) ;

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

				account::remove( accName,m_wallet.get() ) ;

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
