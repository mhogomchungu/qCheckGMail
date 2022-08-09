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

#include <QDesktopServices>
#include <QMessageBox>
#include <QtNetwork/QTcpSocket>

#include <QFile>

static auto header = R"R(HTTP/1.1 200 OK
Date: Thur, 04 Aug 2022 14:28:02 GMT
Server: qCheckGMail
Last-Modified: Thur, 04 Aug 2022 14:28:02 GMT
Content-Length: %1
Content-Type: text/html)R" ;

static auto responce = R"R(
<!DOCTYPE html>
<html>
<body>

<h1>Thank you using qCheckGmail, close this window and go back to qCheckGmail to complete adding an account</h1>

</body>
</html>)R" ;

gmailauthorization::gmailauthorization( QDialog * parent,
					gmailauthorization::function_t& k,
                                        std::function< void() >&& e,
					gmailauthorization::function_0_t&& f ) :
        QDialog( parent ),m_ui( new Ui::gmailauthorization ),
        m_getAuthorizationCode( k ),
        m_cancel( std::move( e ) ),
        m_getAuthorization( std::move( f ) )
{
        m_ui->setupUi( this ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){

		this->cancel() ;
	} ) ;

	connect( m_ui->pbTextBrowser,&QPushButton::clicked,[ this ](){

		QDesktopServices::openUrl( m_ui->textEdit->toPlainText() ) ;
	} ) ;

	QObject::connect( &m_server,&QTcpServer::newConnection,[ this ](){

		if( m_firstConnection ){

			m_firstConnection = false ;

			auto s = m_server.nextPendingConnection() ;

			QObject::connect( s,&QTcpSocket::readyRead,[ this,s ]{

				auto mm = s->readAll() ;

				auto a = mm.indexOf( "code=4/" ) ;

				if( a != -1 ){

					mm = mm.mid( a + 5 ) ;

					a = mm.indexOf( '&' ) ;

					if( a != -a ){

						mm.truncate( a ) ;
					}
				}

				auto h = QString( header ) ;
				auto r = QString( responce ) ;
				h = h.arg( QString::number( r.size() ) ) ;

				s->write( header + QByteArray( "\n\n" ) + responce ) ;

				s->waitForBytesWritten() ;

				s->close() ;

				s->deleteLater() ;

				this->setCode( mm ) ;
			} ) ;
		}
	} ) ;

	auto portNumber = configurationoptionsdialog::portNumber() ;

	while( true ){

		if( portNumber >= 60000 ){

			m_ui->textEdit->setText( "Error: Failed To Accure Port Number For A Localhost Address" ) ;
			break ;
		}

		auto s = m_server.listen( QHostAddress( "http://127.0.0.1" ),static_cast< unsigned short >( portNumber ) ) ;

		if( s ){

			urlOpts opts( "https://accounts.google.com/o/oauth2/auth" ) ;

			opts.add( "client_id",configurationoptionsdialog::clientID() ) ;
			opts.add( "redirect_uri","http://127.0.0.1:" + QString::number( portNumber ) ) ;
			opts.add( "response_type","code" ) ;
			opts.add( "scope","https%3A%2F%2Fmail.google.com%2F" ) ;

			m_ui->textEdit->setText( opts.toString() ) ;

			configurationoptionsdialog::setRuntimePortNumber( portNumber ) ;

			break ;
		}else{
			portNumber++ ;
		}
	}

	this->show() ;
}

void gmailauthorization::cancel()
{
	m_cancel() ;
	this->hideUI() ;
}

void gmailauthorization::setCode( const QString& r )
{
        this->disableAll() ;

        if( r.isEmpty() ){

                QMessageBox msg( this ) ;

		msg.setText( tr( "Authorization Code Field Is Empty" ) ) ;
                msg.addButton( tr( "&OK" ),QMessageBox::YesRole ) ;

                msg.exec() ;

                this->enableAll() ;
        }else{
                m_getAuthorizationCode( r,[ this ]( const QString& e ){

                        if( e.isEmpty() ){

                                QMessageBox msg( this ) ;

				msg.setText( tr( "Failed To Obtain Authorization Code" ) ) ;
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
        m_ui->pbCancel->setEnabled( true ) ;
        m_ui->groupBox->setEnabled( true ) ;
        m_ui->textEdit->setEnabled( true ) ;
}

void gmailauthorization::disableAll()
{
        m_ui->pbCancel->setEnabled( false ) ;
        m_ui->groupBox->setEnabled( false ) ;
        m_ui->textEdit->setEnabled( false ) ;
}

void gmailauthorization::closeEvent( QCloseEvent * e )
{
        e->ignore() ;
        this->cancel() ;
}
