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

#ifndef CONFIGURATIONOPTIONSDIALOG_H
#define CONFIGURATIONOPTIONSDIALOG_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#include "language_path.h"
#include "tray_application_type.h"
#include "settings.h"
#include "util.hpp"

#include <memory>

namespace Ui {
class configurationoptionsdialog;
}

class configurationoptionsdialog : public QDialog
{
	Q_OBJECT
public:
	struct actions
	{
		virtual void configurationWindowClosed( int )
		{
		}
		virtual void enablePassWordChange( bool )
		{
		}
		virtual void audioNotify( bool )
		{
		}
		virtual void alwaysShowTrayIcon( bool )
		{
		}
		virtual ~actions() ;
	} ;

	class Actions
	{
	public:
		template< typename Type,typename ... Args >
		Actions( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void configurationWindowClosed( int s )
		{
			m_handle->configurationWindowClosed( s ) ;
		}
		void enablePassWordChange( bool s )
		{
			m_handle->enablePassWordChange( s ) ;
		}
		void audioNotify( bool s )
		{
			m_handle->audioNotify( s ) ;
		}
		void alwaysShowTrayIcon( bool s )
		{
			m_handle->alwaysShowTrayIcon( s ) ;
		}
	private:
		std::unique_ptr< configurationoptionsdialog::actions > m_handle ;
	} ;

	static void instance( QObject * parent,settings& s,configurationoptionsdialog::Actions ac )
	{
		new configurationoptionsdialog( parent,s,std::move( ac ) ) ;
	}

	configurationoptionsdialog( QObject * parent,settings&,configurationoptionsdialog::Actions ) ;
	~configurationoptionsdialog() override ;
	void ShowUI() ;
	void HideUI() ;
private:
	void setSupportedLanguages() ;
	void pushButtonClose() ;
	bool eventFilter( QObject * watched,QEvent * event ) override ;
	void closeEvent( QCloseEvent * ) override ;
	Ui::configurationoptionsdialog * m_ui ;
	configurationoptionsdialog::Actions m_actions ;
	settings& m_settings ;
};

#endif // CONFIGURATIONOPTIONSDIALOG_H
