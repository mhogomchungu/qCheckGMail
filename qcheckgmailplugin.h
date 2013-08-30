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

#ifndef QCHECKGMAILPLUGIN_H
#define QCHECKGMAILPLUGIN_H
#include <QObject>
#include <razorqt/irazorpanel.h>
#include <razorqt/irazorpanelplugin.h>
#include <razorqt/razorplugininfo.h>

#include <QToolButton>
#include <QVector>
#include <QDebug>
#include "qcheckgmail.h"
#include "accounts.h"

class qChechGMailPlugIn : public QObject,public IRazorPanelPlugin
{
	Q_OBJECT
public:
	qChechGMailPlugIn( const IRazorPanelPluginStartupInfo& startupInfo ) ;
	~qChechGMailPlugIn() ;
	QWidget * widget() ;
	QString themeId() const ;
	void activated( IRazorPanelPlugin::ActivationReason reason ) ;
	IRazorPanelPlugin::Flags flags() const ;
private:
	qCheckGMail * m_gmail ;
};

class qCheckGMailPluginLibrary: public QObject,public IRazorPanelPluginLibrary
{
	Q_OBJECT
	Q_INTERFACES( IRazorPanelPluginLibrary )
public:
	IRazorPanelPlugin * instance( const IRazorPanelPluginStartupInfo& startupInfo )
	{
		return new qChechGMailPlugIn( startupInfo ) ;
	}
};


#endif // QCHECKGMAILPLUGIN_H
