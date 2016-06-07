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

#include "gmailauthorization.h"
#include "ui_gmailauthorization.h"

#include "configurationoptionsdialog.h"

#include <QMessageBox>

gmailauthorization::gmailauthorization( QDialog * parent,
                                        std::function< void( const QString&,std::function< void( const QString& ) > ) >& k,
                                        std::function< void() >&& e,
                                        std::function< void( const QString& ) >&& f ) :
        QDialog( parent ),m_ui( new Ui::gmailauthorization ),
        m_getAuthorizationCode( k ),
        m_cancel( std::move( e ) ),
        m_getAuthorization( std::move( f ) )
{
        m_ui->setupUi( this ) ;

        connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
        connect( m_ui->pbSetCode,SIGNAL( clicked() ),this,SLOT( setCode() ) ) ;

        QString id = R"R(https://accounts.google.com/o/oauth2/auth?client_id=%1&redirect_uri=urn%3Aietf%3Awg%3Aoauth%3A2.0%3Aoob&response_type=code&scope=https%3A%2F%2Fmail.google.com%2F)R" ;

        m_ui->textEdit->setText( id.arg( configurationoptionsdialog::clientID() ) ) ;

        this->show() ;
}

void gmailauthorization::cancel()
{
        m_cancel() ;
        this->hideUI() ;
}

void gmailauthorization::setCode()
{
        this->disableAll() ;

        auto r = m_ui->lineEdit->text() ;

        if( r.isEmpty() ){

                QMessageBox msg( this ) ;

                msg.setText( tr( "authorization code field is empty" ) ) ;
                msg.addButton( tr( "&OK" ),QMessageBox::YesRole ) ;

                msg.exec() ;

                this->enableAll() ;
        }else{
                m_getAuthorizationCode( r,[ this ]( const QString& e ){

                        if( e.isEmpty() ){

                                QMessageBox msg( this ) ;

                                msg.setText( tr( "failed to obtain authorization code" ) ) ;
                                msg.addButton( tr( "&OK" ),QMessageBox::YesRole ) ;

                                msg.exec() ;

                                this->enableAll() ;
                        }else{
                                m_getAuthorization( e ) ;

                                this->hideUI() ;
                        }
                } ) ;
        }
}

void gmailauthorization::hideUI()
{
        this->hide() ;
        this->deleteLater() ;
}

void gmailauthorization::enableAll()
{
        m_ui->label->setEnabled( true ) ;
        m_ui->pbCancel->setEnabled( true ) ;
        m_ui->pbSetCode->setEnabled( true ) ;
        m_ui->lineEdit->setEnabled( true ) ;
        m_ui->groupBox->setEnabled( true ) ;
        m_ui->textEdit->setEnabled( true ) ;
}

void gmailauthorization::disableAll()
{
        m_ui->label->setEnabled( false ) ;
        m_ui->pbCancel->setEnabled( false ) ;
        m_ui->pbSetCode->setEnabled( false ) ;
        m_ui->lineEdit->setEnabled( false ) ;
        m_ui->groupBox->setEnabled( false ) ;
        m_ui->textEdit->setEnabled( false ) ;
}

void gmailauthorization::closeEvent( QCloseEvent * e )
{
        e->ignore() ;
        this->cancel() ;
}
