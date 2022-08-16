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

#include <cstring>

int main( int argc,char * argv[] )
{
	QApplication qGMail( argc,argv ) ;

	auto socketPath = QDir::homePath() + "/.qCheckGMail.socket" ;

	util::appInfo< qCheckGMail,qCheckGMail::args > m( { qGMail },socketPath,qGMail ) ;

	for( int it = 0 ; it < argc ; it++ ){

		if( std::strcmp( argv[ it ],"-m" ) == 0 ){

			return util::runMultiInstances( std::move( m ) ) ;
		}
	}

	return util::runOneInstance( std::move( m ) ) ;
}
