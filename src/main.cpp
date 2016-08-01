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

#include "tray_application_type.h"

#include <QDebug>
#include "qcheckgmail.h"

#if USE_KDE_STATUS_NOTIFIER

#if KF5
#include <QStringList>
#include <QApplication>
#include <kaboutdata.h>
#include <iostream>

int main( int argc,char * argv[] )
{
	KAboutData aboutData( 	"qCheckGMail",
				"qCheckGMail",
				"1.2.7",
				"a qt based gmail checker",
				KAboutLicense::GPL,
				"(c)2013-2015,ink Francis\nemail:mhogomchungu@gmail.com" ) ;

	QApplication a( argc,argv ) ;

	QStringList l = QCoreApplication::arguments() ;

	auto _startApp = [&](){

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

		qCheckGMail w( _setProfile() ) ;
		w.start() ;
		return a.exec() ;
	} ;

	if( l.contains( "-a" ) ){
		if( configurationoptionsdialog::autoStartEnabled() ){
			return _startApp() ;
		}else{
			return qCheckGMail::autoStartDisabled() ;
		}
	}else{
		return _startApp() ;
	}
}
#endif //end if KF5

#if KDE4

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include <klocalizedstring.h>
#include <kuniqueapplication.h>

int main( int argc,char * argv[] )
{
	KAboutData aboutData( 	"qCheckGMail",
				0,
				ki18n( "qCheckGMail" ),
				"1.2.7",
				ki18n( "a qt based gmail checker" ),
				KAboutData::License_GPL_V2,
				ki18n( "(c)2013-2015,ink Francis\nemail:mhogomchungu@gmail.com" ),
				ki18n( "mhogomchungu@gmail.com" ),
				"(c)2013,ink Francis\nemail:mhogomchungu@gmail.com",
				"https://github.com/mhogomchungu/qCheckGMail/issues" );

	KCmdLineArgs::init( argc,argv,&aboutData ) ;

	KCmdLineOptions options ;
	options.add( "a",ki18n( "auto start application" ) ) ;
	options.add( "d",ki18n( "show debug output on the terminal" ) ) ;
	options.add( "i",ki18n( "allow multiple instances" ) ) ;
	options.add( "p <profile>",ki18n( "select profile" ) ) ;

	KCmdLineArgs::addCmdLineOptions( options ) ;
	KUniqueApplication::addCmdLineOptions() ;

	QStringList l = KCmdLineArgs::allArguments() ;

	auto _startApp = [&](){

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

		if( l.contains( "-i" ) ){
			KApplication a ;
			qCheckGMail w( _setProfile() ) ;
			w.start() ;
			return a.exec() ;
		}else{
			if( KUniqueApplication::start() ){
				KUniqueApplication a ;
				qCheckGMail w( _setProfile() ) ;
				w.start() ;
				return a.exec() ;
			}else{
				return qCheckGMail::instanceAlreadyRunning() ;
			}
		}
	} ;

	if( l.contains( "-a" ) ){
		if( configurationoptionsdialog::autoStartEnabled() ){
			return _startApp() ;
		}else{
			return qCheckGMail::autoStartDisabled() ;
		}
	}else{
		return _startApp() ;
	}
}

#endif //end if KDE4

#elif USE_LXQT_PLUGIN

/*
 * we dont get here
 */
int main( void )
{
	return 0 ;
}

#else

/*
 * A pure Qt5 application
 *
 */
#include <QApplication>
#include "qcheckgmail.h"
#include <QStringList>

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

#endif
