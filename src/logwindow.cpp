/*
 *
 *  Copyright (c) 2021
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

#include "logwindow.h"
#include "ui_logwindow.h"

#include "settings.h"

#include <QDateTime>

logWindow::logWindow( settings& s,bool e ) :
	QWidget( nullptr ),
	m_ui( new Ui::logWindow ),
	m_settings( s ),
	m_alwaysAddLogs( e )
{
	m_ui->setupUi( this ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){		

		this->Hide() ;
	} ) ;

	connect( m_ui->pbClear,&QPushButton::clicked,[ this ](){

		m_ui->plainTextEdit->clear() ;
	} ) ;
}

logWindow::~logWindow()
{
	delete m_ui ;
}

void logWindow::update( logWindow::TYPE type,const QString& msg,bool force )
{
	if( this->isVisible() || m_alwaysAddLogs || force ){

		const auto bars = "*************************************************************************" ;

		auto title = [ & ]()->QString{

			if( type == logWindow::TYPE::REQUEST ){

				return "\n%1\nNETWORK REQUEST At %2\n\n" ;

			}else if( type == logWindow::TYPE::RESPONCE ){

				return "\n%1\nNETWORK RESPONCE At %2\n\n" ;

			}else if( type == logWindow::TYPE::INFO ){

				return "\n%1\nINFO At %2\n\n" ;
			}else{
				return "\n%1\nNETWORK ERROR At %2\n\n" ;
			}

		}().arg( bars,QDateTime::currentDateTime().toString() ) ;

		auto m = m_ui->plainTextEdit->toPlainText() ;

		m_ui->plainTextEdit->setPlainText( m + title + msg ) ;
		m_ui->plainTextEdit->moveCursor( QTextCursor::End ) ;
	}
}

void logWindow::Hide()
{
	const auto& r = this->window()->geometry() ;

	auto x = QString::number( r.x() ) ;
	auto y = QString::number( r.y() ) ;
	auto w = QString::number( r.width() ) ;
	auto h = QString::number( r.height() ) ;

	m_settings.setWindowDimensions( "LogWindow",x + "-" + y + "-" + w + "-" + h ) ;

	this->hide() ;
	m_ui->plainTextEdit->clear() ;
}

void logWindow::Show()
{
	auto w = m_settings.windowsDimensions( "LogWindow" ) ;

	if( !w.isEmpty() ){

		auto m = util::split( w,'-' ) ;

		if( m.size() == 4 ){

			QRect r ;

			r.setX( m.at( 0 ).toInt() ) ;
			r.setY( m.at( 1 ).toInt() ) ;
			r.setWidth( m.at( 2 ).toInt() ) ;
			r.setHeight( m.at( 3 ).toInt() ) ;

			this->window()->setGeometry( r ) ;
		}
	}

	this->show() ;
}

void logWindow::retranslateUi()
{
	m_ui->retranslateUi( this ) ;
}

void logWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->Hide() ;
}
