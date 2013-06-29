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

#include <QApplication>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <kuniqueapplication.h>
#include <QDebug>
#include "qcheckgmail.h"

int startApp( void )
{
	if( KUniqueApplication::start() ){
		KUniqueApplication a ;
		qCheckGMail w;
		w.start();
		return a.exec();
	}else{
		return qCheckGMail::instanceAlreadyRunning() ;
	}
}

int main(int argc, char *argv[])
{
	KAboutData aboutData( 	"qCheckGMail",
				0,
				ki18n( "qCheckGMail" ),
				"1.2.0",
				ki18n( "a qt based gmail checker" ),
				KAboutData::License_GPL_V2,
				ki18n( "(c)2013,ink Francis\nemail:mhogomchungu@gmail.com" ),
				ki18n( "mhogomchungu@gmail.com" ),
				"(c)2013,ink Francis\nemail:mhogomchungu@gmail.com",
				"https://github.com/mhogomchungu/qCheckGMail/issues" );

	KCmdLineArgs::init( argc,argv,&aboutData );

	KCmdLineOptions options;
	options.add( "a",ki18n( "auto start application" ) ) ;
	options.add( "d",ki18n( "show debug output on the terminal" ) ) ;

	KCmdLineArgs::addCmdLineOptions( options ) ;
	KUniqueApplication::addCmdLineOptions();

	if( KCmdLineArgs::allArguments().contains( "-a" ) ){
		if( configurationoptionsdialog::autoStartEnabled() ){
			return startApp() ;
		}else{
			return qCheckGMail::autoStartDisabled() ;
		}
	}else{
		return startApp() ;
	}
}
