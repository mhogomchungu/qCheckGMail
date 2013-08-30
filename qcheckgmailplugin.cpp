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

#include "qcheckgmailplugin.h"

Q_EXPORT_PLUGIN2( qChechGMailPlugIn,qCheckGMailPluginLibrary )

qChechGMailPlugIn::qChechGMailPlugIn( const IRazorPanelPluginStartupInfo& startupInfo ) : IRazorPanelPlugin( startupInfo )
{
	m_gmail = new qCheckGMail() ;
	m_gmail->start() ;
}

qChechGMailPlugIn::~qChechGMailPlugIn()
{
	m_gmail->deleteLater() ;
}

QWidget * qChechGMailPlugIn::widget()
{
	return m_gmail->widget() ;
}

QString qChechGMailPlugIn::themeId() const
{
	return "qCheckGMail" ;
}

void qChechGMailPlugIn::activated( IRazorPanelPlugin::ActivationReason reason )
{
	qDebug() << int( reason ) ;

	if( reason == IRazorPanelPlugin::MiddleClick ){
		const QVector<accounts>& acc = m_gmail->configuredAccounts() ;
		if( acc.size() > 0 ){
			QString url = acc.at( 0 ).defaultLabelUrl() ;
			int index = url.size() - QString( "/feed/atom/" ).size() ;
			url.truncate( index ) ;
			QDesktopServices::openUrl( QUrl( url ) ) ;
		}else{
			QDesktopServices::openUrl( QUrl( "https://mail.google.com/" ) ) ;
		}
	}else{
		;
	}
}

IRazorPanelPlugin::Flags qChechGMailPlugIn::flags() const
{
	return IRazorPanelPlugin::PreferRightAlignment ;
}
