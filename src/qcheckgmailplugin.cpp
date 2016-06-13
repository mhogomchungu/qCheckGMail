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

#ifdef USE_LXQT_PLUGIN
#if USE_LXQT_PLUGIN

#include "qcheckgmailplugin.h"

#if NEW_LXQT_API
qCheckGMailPlugIn::qCheckGMailPlugIn( const ILXQtPanelPluginStartupInfo& startupInfo ) : ILXQtPanelPlugin( startupInfo )
#else
Q_EXPORT_PLUGIN2( qCheckGMailPlugIn,qCheckGMailPluginLibrary )
qCheckGMailPlugIn::qCheckGMailPlugIn( const ILxQtPanelPluginStartupInfo& startupInfo ) : ILxQtPanelPlugin( startupInfo )
#endif
{
	m_gmail = new qCheckGMail() ;
	m_gmail->start() ;
}

qCheckGMailPlugIn::~qCheckGMailPlugIn()
{
	delete m_gmail ;
}

QWidget * qCheckGMailPlugIn::widget()
{
	return m_gmail->widget() ;
}

QString qCheckGMailPlugIn::themeId() const
{
	return "qCheckGMail" ;
}

#if NEW_LXQT_API

void qCheckGMailPlugIn::activated( ILXQtPanelPlugin::ActivationReason reason )
{
	if( reason == ILXQtPanelPlugin::MiddleClick ){
		m_gmail->iconClicked() ;
	}
}

ILXQtPanelPlugin::Flags qCheckGMailPlugIn::flags() const
{
	return ILXQtPanelPlugin::PreferRightAlignment ;
}

#else

void qCheckGMailPlugIn::activated( ILxQtPanelPlugin::ActivationReason reason )
{
	if( reason == ILxQtPanelPlugin::MiddleClick ){
		m_gmail->iconClicked() ;
	}
}

ILxQtPanelPlugin::Flags qCheckGMailPlugIn::flags() const
{
	return ILxQtPanelPlugin::PreferRightAlignment ;
}

#endif
#endif
#endif
