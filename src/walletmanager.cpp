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

#include "../lxqt_wallet/frontend/task.h"

namespace Task = LxQt::Wallet::Task ;

#include <utility>

#define LABEL_IDENTIFIER        "-qCheckGMail-LABEL_ID"
#define DISPLAY_NAME_IDENTIFIER "-qCheckGMail-DISPLAY_NAME_ID"
#define TOKEN_IDENTIFIER        "-qCheckGMail-TOKEN_KEY_ID"

walletmanager::walletmanager( const QString& icon,
                              std::function< void() >&& e,
                              std::function< void( const QString&,std::function< void( const QString& ) > ) >&& k,
                              std::function< void( QVector< accounts > && ) >&& f ) :
        m_ui( nullptr ),m_wallet( nullptr ),m_walletClosed( e ),m_getAuthorization( k ),m_getAccountInfo( f )
{
	m_icon = QString( ":/%1" ).arg( icon ) ;
}

walletmanager::~walletmanager()
{
        m_walletClosed() ;
	m_wallet->deleteLater() ;

        delete m_ui ;
}

void walletmanager::buildGUI()
{
	m_ui = new Ui::walletmanager ;
	m_ui->setupUi( this ) ;

        //this->setFixedSize( this->size() ) ;

        m_ui->pushButtonAccountAdd->setMinimumHeight( 31 ) ;
        m_ui->pushButtonClose->setMaximumHeight( 31 ) ;

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
        auto s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
        m_wallet->open( s,"qCheckGMail" ) ;
}

void walletmanager::getAccounts( void )
{
	m_action = walletmanager::getAccountInfo ;
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setInterfaceObject( this ) ;
        auto s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
        m_wallet->open( s,"qCheckGMail" ) ;
}

void walletmanager::changeWalletPassword()
{
	m_wallet = configurationoptionsdialog::secureStorageSystem() ;
	m_wallet->setInterfaceObject( this ) ;
	m_wallet->setImage( m_icon ) ;
        auto s = configurationoptionsdialog::walletName( m_wallet->backEnd() ) ;
        m_wallet->changeWalletPassWord( s,"qCheckGMail" ) ;
}

void walletmanager::walletpassWordChanged( bool passwordChanged )
{
	Q_UNUSED( passwordChanged ) ;
	this->deleteLater() ;
}

void walletmanager::addEntry( const accounts& acc )
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
	m_table->setItem( row,1,_add_item( acc.displayName() ) ) ;
	m_table->setItem( row,2,_add_item( acc.labels() ) ) ;
}

void walletmanager::readAccountInfo()
{
        using wallet = QVector< LxQt::Wallet::walletKeyValues > ;

	m_accounts.clear() ;

	auto _getAccEntry = []( const QString& acc,const wallet& entries )->const QByteArray&{

		for( const auto& it : entries ){

			if( it.getKey() == acc ){

				return it.getValue() ;
			}
		}

		static QByteArray shouldNotGetHere ;
		return shouldNotGetHere ;
	} ;

        auto labels_id  = LABEL_IDENTIFIER ;
        auto display_id = DISPLAY_NAME_IDENTIFIER ;
        auto token_id   = TOKEN_IDENTIFIER ;

        auto entries = Task::await< wallet >( [ this ](){ return m_wallet->readAllKeyValues() ; } ) ;

	for( const auto& it : entries ){

                const auto& accName = it.getKey() ;

                bool r = accName.endsWith( labels_id ) ||
                                accName.endsWith( display_id ) ||
                                accName.endsWith( token_id ) ;

		if( r == false ){

			const auto& passWord    = _getAccEntry( accName,entries ) ;
			const auto& labels      = _getAccEntry( accName + labels_id,entries ) ;
			const auto& displayName = _getAccEntry( accName + display_id,entries ) ;
                        const auto& tokenKey    = _getAccEntry( accName + token_id,entries ) ;

                        m_accounts.append( accounts( { accName,passWord,displayName,labels,tokenKey } ) ) ;
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

                        m_getAccountInfo( std::move( m_accounts ) ) ;
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
        m_getAccountInfo( std::move( m_accounts ) ) ;
	this->hide() ;
	this->deleteLater() ;
}

void walletmanager::pushButtonAdd()
{
	this->disableAll() ;

        addaccount::instance( this,m_getAuthorization,[ this ](){

                this->enableAll() ;

        },[ this ]( accounts::entry&& e ){

                m_accountEntry = std::move( e ) ;

                Task::run( [ this ](){

                        auto labels_id  = m_accountEntry.accName + LABEL_IDENTIFIER ;
                        auto display_id = m_accountEntry.accName + DISPLAY_NAME_IDENTIFIER ;
                        auto token_id   = m_accountEntry.accName + TOKEN_IDENTIFIER ;

                        m_wallet->addKey( m_accountEntry.accName,m_accountEntry.accPassword.toLatin1() ) ;
                        m_wallet->addKey( labels_id,m_accountEntry.accLabels.toLatin1() ) ;
                        m_wallet->addKey( display_id,m_accountEntry.accDisplayName.toLatin1() ) ;
                        m_wallet->addKey( token_id,m_accountEntry.accRefreshToken.toLatin1() ) ;

                } ).then( [ this ](){

                        accounts acc( m_accountEntry ) ;

                        m_accounts.append( acc ) ;

                        this->addEntry( acc ) ;

                        this->selectLastRow() ;
                        this->enableAll() ;
                } ) ;
        } ) ;
}

void walletmanager::tableItemClicked( QTableWidgetItem * item )
{
	m_deleteRow = item->row() ;

	QMenu m ;

        m.addAction( [ & ](){

                auto ac = new QAction( &m ) ;
                ac->setText( tr( "delete entry" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( deleteAccount() ) ) ;

                return ac ;
        }() ) ;

        m.addAction( [ & ](){

                auto ac = new QAction( &m ) ;
                ac->setText( tr( "edit entry" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( editAccount() ) ) ;

                return ac ;
        }() ) ;

	m.exec( QCursor::pos() ) ;
}

void walletmanager::deleteAccount()
{
	auto item = m_table->currentItem() ;
	m_row = item->row() ;
        auto accName = m_table->item( m_row,0 )->text() ;

	QMessageBox msg( this ) ;
        msg.setText( tr( "are you sure you want to delete \"%1\" account?" ).arg( accName ) ) ;
	msg.addButton( tr( "yes" ),QMessageBox::YesRole ) ;
        auto no_button = msg.addButton( tr( "no" ),QMessageBox::NoRole ) ;
	msg.setDefaultButton( no_button ) ;

	this->disableAll() ;

	msg.exec() ;

	if( msg.clickedButton() != no_button ){

		if( m_row < m_accounts.size() && m_row < m_table->rowCount() ){

                        Task::run( [ & ](){

                                auto labels_id  = accName + LABEL_IDENTIFIER ;
                                auto display_id = accName + DISPLAY_NAME_IDENTIFIER ;
                                auto token_id   = accName + TOKEN_IDENTIFIER ;

                                m_wallet->deleteKey( accName ) ;
				m_wallet->deleteKey( labels_id ) ;
				m_wallet->deleteKey( display_id ) ;
                                m_wallet->deleteKey( token_id ) ;

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
        m_row = m_table->currentRow() ;

        auto _getPassWord = [ this ]( const QString& accName,QString& p,QString& t ){

		for( const auto& it : m_accounts ){

			if( it.accountName() == accName ){

                                p = it.passWord() ;
                                t = it.refreshToken() ;

                                break ;
			}
                }
	} ;

        auto accName        = m_table->item( m_row,0 )->text() ;
        auto accDisplayName = m_table->item( m_row,1 )->text() ;
        auto accLabels      = m_table->item( m_row,2 )->text() ;

        QString accPassword ;
        QString accToken ;

        _getPassWord( accName,accPassword,accToken ) ;

	this->disableAll() ;

        addaccount::instance( this,{ accName,accPassword,accDisplayName,accLabels,accToken },
                             m_getAuthorization,[ this ](){

                this->enableAll() ;

        },[ this ]( accounts::entry&& e ){

                m_accountEntry = std::move( e ) ;

                Task::run( [ this ](){

                        auto labels_id  = m_accountEntry.accName + LABEL_IDENTIFIER ;
                        auto display_id = m_accountEntry.accName + DISPLAY_NAME_IDENTIFIER ;
                        auto token_id   = m_accountEntry.accName + TOKEN_IDENTIFIER ;

                        m_wallet->deleteKey( m_accountEntry.accName ) ;
                        m_wallet->deleteKey( labels_id ) ;
                        m_wallet->deleteKey( display_id ) ;
                        m_wallet->deleteKey( token_id ) ;

                        m_wallet->addKey( m_accountEntry.accName,m_accountEntry.accPassword.toLatin1() ) ;
                        m_wallet->addKey( labels_id,m_accountEntry.accLabels.toLatin1() ) ;
                        m_wallet->addKey( display_id,m_accountEntry.accDisplayName.toLatin1() ) ;
                        m_wallet->addKey( token_id,m_accountEntry.accRefreshToken.toLatin1() ) ;

                } ).then( [ this ](){

                        m_accounts.replace( m_row,accounts( m_accountEntry ) ) ;

                        m_table->item( m_row,0 )->setText( m_accountEntry.accName ) ;
                        m_table->item( m_row,1 )->setText( m_accountEntry.accDisplayName ) ;
                        m_table->item( m_row,2 )->setText( m_accountEntry.accLabels ) ;

                        this->enableAll() ;
                } ) ;
        } ) ;
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
