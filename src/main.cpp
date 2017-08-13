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

#include "qcheckgmail.h"

int main( int argc,char * argv[] )
{
	QApplication a( argc,argv ) ;
	QStringList l = QCoreApplication::arguments() ;

	auto _setProfile = [&]()->QString{
		QString arg( "-p" ) ;
		int j = l.size() ;
		for( int i = 0 ; i < j ; i++ ){
			if( l.at( i ) == arg ){
				if( i + 1 < j ){
					return l.at( i + 1 ) ;
				}else{
					return QString() ;
				}
			}
		}

		return QString() ;
	} ;

	if( l.contains( "-a" ) ){
		if( configurationoptionsdialog::autoStartEnabled() ){
			qCheckGMail w( _setProfile() ) ;
			w.start() ;
			return a.exec() ;
		}else{
			return qCheckGMail::autoStartDisabled() ;
		}
	}else{
		qCheckGMail w( _setProfile() ) ;
		w.start() ;
		return a.exec() ;
	}
}
