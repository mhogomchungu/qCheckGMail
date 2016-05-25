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

#include "qcheckgmailplugin.h"

#if NEW_LXQT_API
qChechGMailPlugIn::qChechGMailPlugIn( const ILXQtPanelPluginStartupInfo& startupInfo ) : ILXQtPanelPlugin( startupInfo )
#else
Q_EXPORT_PLUGIN2( qChechGMailPlugIn,qCheckGMailPluginLibrary )
qChechGMailPlugIn::qChechGMailPlugIn( const ILxQtPanelPluginStartupInfo& startupInfo ) : ILxQtPanelPlugin( startupInfo )
#endif
{
	m_gmail = new qCheckGMail() ;
	m_gmail->start() ;
}

qChechGMailPlugIn::~qChechGMailPlugIn()
{
	delete m_gmail ;
}

QWidget * qChechGMailPlugIn::widget()
{
	return m_gmail->widget() ;
}

QString qChechGMailPlugIn::themeId() const
{
	return "qCheckGMail" ;
}

#if NEW_LXQT_API

void qChechGMailPlugIn::activated( ILXQtPanelPlugin::ActivationReason reason )
{
	if( reason == ILXQtPanelPlugin::MiddleClick ){
		m_gmail->iconClicked() ;
	}
}

ILXQtPanelPlugin::Flags qChechGMailPlugIn::flags() const
{
	return ILXQtPanelPlugin::PreferRightAlignment ;
}

#else

void qChechGMailPlugIn::activated( ILxQtPanelPlugin::ActivationReason reason )
{
	if( reason == ILxQtPanelPlugin::MiddleClick ){
		m_gmail->iconClicked() ;
	}
}

ILxQtPanelPlugin::Flags qChechGMailPlugIn::flags() const
{
	return ILxQtPanelPlugin::PreferRightAlignment ;
}

#endif
