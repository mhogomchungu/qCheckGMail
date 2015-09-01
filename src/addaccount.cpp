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

addaccount::addaccount( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::addaccount )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	m_row = -1 ;
	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
}

addaccount::addaccount( int row,QString accName,QString accPassword,QString accDisplayName,
			QString accLabels,QWidget * parent ) :QDialog( parent ),m_ui( new Ui::addaccount )
{
	m_ui->setupUi( this );
	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	m_row = row ;

	m_ui->lineEditName->setText( accName ) ;
	m_ui->lineEditName->setEnabled( false ) ;
	m_ui->lineEditName->setToolTip( QString() ) ;
	m_ui->lineEditPassword->setText( accPassword ) ;
	m_ui->lineEditLabel->setText( accLabels ) ;
	m_ui->lineEditOptionalName->setText( accDisplayName ) ;

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
}

void addaccount::ShowUI()
{
	if( m_row != -1 ){
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
	QString accName  = m_ui->lineEditName->text() ;
	QString password = m_ui->lineEditPassword->text() ;
	QString labels   = m_ui->lineEditLabel->text() ;
	QString displayN = m_ui->lineEditOptionalName->text() ;

	if( accName.isEmpty() || password.isEmpty() ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "ERROR: one or more required field is empty" ) ) ;
		msg.exec() ;
	}else{
		if( m_row == -1 ){
			emit addAccount( accName,password,displayN,labels ) ;
		}else{
			emit editAccount( m_row,accName,password,displayN,labels ) ;
		}
		this->HideUI() ;
	}
}

void addaccount::cancel()
{
	emit cancelSignal() ;
	this->HideUI() ;
}
