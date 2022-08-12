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

#include <QApplication>
#include <QStringList>
#include <QStandardPaths>

#include "qcheckgmail.h"
#include "util.hpp"

#include <iostream>

class qCheckGMailInit
{
public:
	struct args
	{
		const QStringList& args ;
		QApplication& app ;
	} ;
	qCheckGMailInit( const qCheckGMailInit::args& args ) :
		m_args( args ),
		m_mainApp( this->setProfile() )
	{
	}
	QString setProfile()
	{
		QString arg( "-p" ) ;

		int j = m_args.args.size() ;

		for( int i = 0 ; i < j ; i++ ){

			if( m_args.args.at( i ) == arg ){

				if( i + 1 < j ){

					return m_args.args.at( i + 1 ) ;
				}else{
					return QString() ;
				}
			}
		}

		return QString() ;
	}
	void event( const QByteArray& )
	{
	}
	void start( const QByteArray& )
	{
		if( m_args.args.contains( "-a" ) ){

			if( configurationoptionsdialog::autoStartEnabled() ){

				m_mainApp.start() ;
			}else{
				m_args.app.exit( qCheckGMail::autoStartDisabled() ) ;
			}
		}else{
			m_mainApp.start() ;
		}
	}
private:
	qCheckGMailInit::args m_args ;
	qCheckGMail m_mainApp ;
} ;

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	auto instanceArgs = util::make_oneinstance_args( [ & ](){

		std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
		a.exit() ;
	},[](){
		std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
	} ) ;

	using type = decltype( instanceArgs ) ;

	using singleInstance = util::oneinstance< qCheckGMailInit,qCheckGMailInit::args,type > ;

	QString spath = QDir::homePath() + "/.qCheckGMail.socket" ;

	auto args = a.arguments() ;

	qCheckGMailInit::args mArgs{ args,a } ;

	singleInstance instance( spath,QByteArray(),std::move( mArgs ),std::move( instanceArgs ) ) ;

	return a.exec() ;
}
