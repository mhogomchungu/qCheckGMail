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


#include "addaccount.h"
#include "ui_addaccount.h"

addaccount::addaccount( QWidget * parent,
                        std::function< void() >&& e,
                        std::function< void( const accounts::entry& e ) > && f ) :
        QDialog( parent ),
        m_ui( new Ui::addaccount ),
        m_cancel( std::move( e ) ),
        m_result( std::move( f ) )
{
	m_ui->setupUi( this ) ;

        this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

        m_edit = false ;

        connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;

        this->ShowUI() ;
}

addaccount::addaccount( QWidget * parent,
                        const accounts::entry& e,
                        std::function< void() >&& r,
                        std::function< void( const accounts::entry& e ) > && f ) :
        QDialog( parent ),
        m_ui( new Ui::addaccount ),
        m_cancel( std::move( r ) ),
        m_result( std::move( f ) )
{
	m_ui->setupUi( this );
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

        m_edit = true ;

        m_ui->lineEditName->setText( e.accName ) ;
	m_ui->lineEditName->setEnabled( false ) ;
	m_ui->lineEditName->setToolTip( QString() ) ;
        m_ui->lineEditPassword->setText( e.accPassword ) ;
        m_ui->lineEditLabel->setText( e.accLabels ) ;
        m_ui->lineEditOptionalName->setText( e.accDisplayName ) ;

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;

        this->ShowUI() ;
}

void addaccount::ShowUI()
{
        if( m_edit ){

		m_ui->pushButtonAdd->setText( tr( "edit" ) ) ;
		this->setWindowTitle( tr( "edit account" ) ) ;
	}

	this->show();
}

void addaccount::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

addaccount::~addaccount()
{
	delete m_ui ;
}

void addaccount::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->cancel() ;
}

void addaccount::add()
{
        auto accName  = m_ui->lineEditName->text() ;
        auto accPassword = m_ui->lineEditPassword->text() ;
        auto accDisplayName = m_ui->lineEditOptionalName->text() ;
        auto accLabels   = m_ui->lineEditLabel->text() ;

        if( accName.isEmpty() || accPassword.isEmpty() ){

		QMessageBox msg( this ) ;
		msg.setText( tr( "ERROR: one or more required field is empty" ) ) ;
		msg.exec() ;
	}else{
                m_result( { accName,accPassword,accDisplayName,accLabels } ) ;

		this->HideUI() ;
	}
}

void addaccount::cancel()
{
        m_cancel() ;
	this->HideUI() ;
}
