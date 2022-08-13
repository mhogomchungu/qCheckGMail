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

int main( int argc,char * argv[] )
{
	QApplication qGMail( argc,argv ) ;

	auto appType = util::type_identity< qCheckGMail >() ;
	auto appArgs = qCheckGMail::args{ qGMail } ;
	auto socketPath = QDir::homePath() + "/.qCheckGMail.socket" ;

	return util::runOneInstance( appType,appArgs,socketPath,QByteArray(),qGMail ) ;
}
