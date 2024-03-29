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

#include <QJsonDocument>
#include <QJsonObject>

addaccount::addaccount( QDialog * parent,
			logWindow& l,
			settings& e,
			gmailauthorization::getAuth& k,
			addaccount::Actions s,
			addaccount::GMailInfo& n ) :
	QDialog( parent ),
	m_ui( new Ui::addaccount ),
	m_getAuthorization( k ),
	m_actions( std::move( s ) ),
	m_gmailAccountInfo( n ),
	m_logWindow( l ),
	m_setting( e )
{
	m_ui->setupUi( this ) ;

	//this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	this->setUpMenu() ;

	m_ui->labelNetworkError->setVisible( false ) ;

	m_ui->lineEditLabel->setText( "INBOX" ) ;

	m_edit = false ;

	connect( m_ui->pushButtonAdd,&QPushButton::clicked,this,&addaccount::add ) ;
	connect( m_ui->pushButtonCancel,&QPushButton::clicked,this,&addaccount::cancel ) ;

	m_ui->pushButtonAdd->setMinimumHeight( 31 ) ;
	m_ui->pushButtonCancel->setMinimumHeight( 31 ) ;

	class accs : public gmailauthorization::actions
	{
	public:
		accs( addaccount& acc ) : m_parent( acc )
		{
		}
		void cancel() override
		{
			m_parent.cancel() ;
		}
		void getToken( const QString& e,const QByteArray& s ) override
		{
			if( e.isEmpty() ){

				m_parent.HideUI() ;

				auto m = "Failed To Generate Token\n" + s ;

				m_parent.m_logWindow.update( logWindow::TYPE::INFO,m,true ) ;
			}else{
				m_parent.getLabels( e ) ;
			}
		}
	private:
		addaccount& m_parent ;
	};

	gmailauthorization::instance( this,
				      m_setting,
				      m_getAuthorization,
				      { util::type_identity< accs >(),*this } ) ;
}

addaccount::addaccount( QDialog * parent,
			logWindow& l,
			settings& m,
			const accounts::entry& e,
			gmailauthorization::getAuth& k,
			addaccount::Actions s,
			addaccount::GMailInfo& n ) :
	QDialog( parent ),
	m_ui( new Ui::addaccount ),
	m_getAuthorization( k ),
	m_actions( std::move( s ) ),
	m_gmailAccountInfo( n ),
	m_logWindow( l ),
	m_setting( m )
{
	m_ui->setupUi( this );
	//this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	m_ui->labelNetworkError->setVisible( false ) ;

	m_edit = true ;

	m_ui->lineEditName->setText( e.accName ) ;

	m_ui->lineEditLabel->setText( util::namesFromJson( e.accLabels ) ) ;

	m_ui->lineEditName->setToolTip( QString() ) ;

	connect( m_ui->pushButtonAdd,&QPushButton::clicked,this,&addaccount::add ) ;
	connect( m_ui->pushButtonCancel,&QPushButton::clicked,this,&addaccount::cancel ) ;

	if( m_edit ){

		m_ui->pushButtonAdd->setText( tr( "Edit" ) ) ;
		this->setWindowTitle( tr( "Edit Account" ) ) ;
	}

	this->setUpMenu() ;

	this->showLabelMenu( m_actions.labels() ) ;

	this->show() ;
}

void addaccount::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void addaccount::getLabels( const QString& e )
{
	m_key = e ;

	class meaw : public addaccount::gmailAccountInfo
	{
	public:
		meaw( addaccount& acc ) : m_parent( acc )
		{
		}
		void operator()( addaccount::labels s ) override
		{
			m_parent.showLabels( std::move( s ) ) ;
		}
		void operator()( const QString& e ) override
		{
			m_parent.showError( e ) ;
		}
	private:
		addaccount& m_parent ;
	};

	m_gmailAccountInfo.withoutToken( e,{ util::type_identity< meaw >(),*this } ) ;
}

void addaccount::showLabels( addaccount::labels&& s )
{
	m_actions.setLabels( std::move( s ) ) ;

	this->showLabelMenu( m_actions.labels() ) ;
	this->show() ;
}

void addaccount::showError( const QString& e )
{
	auto txt = tr( "Network Error: " ) + e ;

	m_ui->labelNetworkError->setText( txt ) ;

	m_ui->labelNetworkError->setVisible( true ) ;
	m_ui->pushButtonAdd->setEnabled( false ) ;

	this->show() ;
}

void addaccount::showLabelMenu( const addaccount::labels& e )
{
	auto m = util::split( m_ui->lineEditLabel->text() ) ;

	auto _contains = [ & ]( const QString& e ){

		for( const auto& it : m ){

			if( it == e ){

				return true ;
			}
		}

		return false ;
	} ;

	for( const auto& it : e.entries ){

		auto ac = m_menu.addAction( it.name ) ;

		ac->setCheckable( true ) ;
		ac->setObjectName( it.name ) ;

		if( it.name == "INBOX" ){

			ac->setChecked( true ) ;
		}else{
			ac->setChecked( _contains( it.name ) ) ;
		}
	}
}

void addaccount::setUpMenu()
{
	m_ui->lineEditLabel->setEnabled( false ) ;

	connect( &m_menu,&QMenu::triggered,[ this ]( QAction * ac ){

		auto s = util::split( m_ui->lineEditLabel->text() ) ;

		auto m = ac->objectName() ;

		if( m == "INBOX" ){

			ac->setChecked( true ) ;
		}else{
			if( ac->isChecked() ){

				if( !s.contains( m ) ){

					s.append( m ) ;
				}
			}else{
				s.removeAll( m ) ;
			}
		}

		m_ui->lineEditLabel->setText( s.join( ',' ) ) ;
	} ) ;

	m_ui->pbLabelMenu->setMenu( &m_menu ) ;
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
	if( m_edit ){

		m_actions.edit( m_ui->lineEditName->text(),m_ui->lineEditLabel->text() ) ;

		this->HideUI() ;
	}else{
		auto accName  = this->m_ui->lineEditName->text() ;

		if( accName.isEmpty() ){

			QMessageBox msg( this ) ;

			msg.setText( tr( "ERROR: One Or More Required Field Is Empty" ) ) ;
			msg.exec() ;
		}else{
			auto lbs = util::labelsToJson( this->m_ui->lineEditLabel->text(),
						       m_actions.labels().entries ) ;

			m_actions.results( { accName,std::move( lbs ),m_key } ) ;

			this->HideUI() ;
		}
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

addaccount::gmailAccountInfo::~gmailAccountInfo()
{
}

addaccount::gMailInfo::~gMailInfo()
{
}
