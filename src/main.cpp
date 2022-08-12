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
		QApplication& app ;
	} ;
	qCheckGMailInit( const qCheckGMailInit::args& args ) :
		m_qApp( args.app ),
		m_args( m_qApp.arguments() ),
		m_mainApp( this->setProfile() )
	{
	}
	QString setProfile()
	{
		int j = m_args.size() ;

		for( int i = 0 ; i < j ; i++ ){

			if( m_args.at( i ) == "-p" ){

				if( i + 1 < j ){

					return m_args.at( i + 1 ) ;
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
		if( m_args.contains( "-a" ) ){

			if( configurationoptionsdialog::autoStartEnabled() ){

				m_mainApp.start() ;
			}else{
				m_qApp.exit( qCheckGMail::autoStartDisabled() ) ;
			}
		}else{
			m_mainApp.start() ;
		}
	}
private:
	QApplication& m_qApp ;
	QStringList m_args ;
	qCheckGMail m_mainApp ;
} ;

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;

	auto instanceArgs = util::make_oneinstance_args( [](){

		std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
	},[](){
		std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
	} ) ;

	using type = decltype( instanceArgs ) ;

	using singleInstance = util::oneinstance< qCheckGMailInit,qCheckGMailInit::args,type > ;

	QString spath = QDir::homePath() + "/.qCheckGMail.socket" ;

	return singleInstance( spath,{},a,{ a },std::move( instanceArgs ) ).exec() ;
}
