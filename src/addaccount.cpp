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

#include "gmailauthorization.h"

#include <iostream>

addaccount::addaccount( QDialog * parent,
			gmailauthorization::getAutho& k,
			addaccount::Actions s ) :
        QDialog( parent ),
        m_ui( new Ui::addaccount ),
        m_getAuthorization( k ),
	m_actions( std::move( s ) )
{
	m_ui->setupUi( this ) ;

        //this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

        m_edit = false ;

        connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;

	m_ui->pushButtonAdd->setMinimumHeight( 31 ) ;
	m_ui->pushButtonCancel->setMinimumHeight( 31 ) ;

	m_ui->lineEditPassword->setEnabled( false ) ;

	class accs : public gmailauthorization::actions
	{
	public:
		accs( addaccount * acc ) : m_acc( acc )
		{
		}
		void cancel() override
		{
			m_acc->cancel() ;
		}
		void getToken( const QString& e,const QByteArray& s ) override
		{
			if( e.isEmpty() ){

				m_acc->HideUI() ;

				std::cout << "ERROR: Failed To Generate Token\n" ;
				std::cout << s.constData() << std::endl ;
			}else{
				m_acc->show() ;
				m_acc->m_ui->lineEditPassword->setText( e ) ;
			}
		}
	private:
		addaccount * m_acc ;
	};

	gmailauthorization::instance( this,
				      m_getAuthorization,
				      { gmailauthorization::type_identity< accs >(),this } ) ;
}

addaccount::addaccount( QDialog * parent,
                        const accounts::entry& e,
			gmailauthorization::getAutho& k,
			addaccount::Actions s ) :
        QDialog( parent ),
        m_ui( new Ui::addaccount ),
	m_getAuthorization( k ),
	m_actions( std::move( s ) )
{
	m_ui->setupUi( this );
        //this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

        m_edit = true ;

        m_ui->lineEditName->setText( e.accName ) ;
	m_ui->lineEditName->setEnabled( false ) ;
	m_ui->lineEditName->setToolTip( QString() ) ;
        m_ui->lineEditLabel->setText( e.accLabels ) ;
        m_ui->lineEditOptionalName->setText( e.accDisplayName ) ;

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;

	m_ui->lineEditPassword->setEnabled( false ) ;

        if( m_edit ){

		m_ui->pushButtonAdd->setText( tr( "Edit" ) ) ;
		this->setWindowTitle( tr( "Edit Account" ) ) ;
        }

        if( e.accRefreshToken.isEmpty() ){

                m_ui->lineEditPassword->setText( e.accPassword ) ;
        }else{
                m_ui->lineEditPassword->setText( e.accRefreshToken ) ;
        }

	this->show() ;
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
        auto accDisplayName = m_ui->lineEditOptionalName->text() ;
        auto accLabels   = m_ui->lineEditLabel->text() ;

        auto key = m_ui->lineEditPassword->text() ;

        if( accName.isEmpty() || key.isEmpty() ){

		QMessageBox msg( this ) ;

		msg.setText( tr( "ERROR: One Or More Required Field Is Empty" ) ) ;
		msg.exec() ;
	}else{
		m_actions.results( { accName,QString(),accDisplayName,accLabels,key } ) ;
		this->HideUI() ;
	}
}

void addaccount::cancel()
{
	m_actions.cancel() ;
	this->HideUI() ;
}

addaccount::actions::~actions()
{
}
