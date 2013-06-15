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


#include "addaccount.h"
#include "ui_addaccount.h"

addaccount::addaccount( QWidget * parent ) :QDialog( parent ),m_ui( new Ui::addaccount )
{
	m_ui->setupUi( this );

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
}

void addaccount::ShowUI()
{
	this->show();
}

void addaccount::HideUI()
{
	this->hide();
	this->deleteLater();
}

addaccount::~addaccount()
{
	delete m_ui;
}

void addaccount::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void addaccount::add()
{
	QString name     = m_ui->lineEditName->text() ;
	QString password = m_ui->lineEditPassword->text() ;
	QString labels   = m_ui->lineEditLabel->text() ;
	QString displayN = m_ui->lineEditOptionalName->text() ;

	if( name.isEmpty() || password.isEmpty() ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "ERROR: one or more reguired field is missing" ) ) ;
		msg.exec() ;
	}else{
		emit addAccount( name,password,displayN,labels ) ;
		this->HideUI();
	}
}

void addaccount::cancel()
{
	this->HideUI();
}
