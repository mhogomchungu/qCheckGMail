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
#include <lxqt/ilxqtpanel.h>
#include <lxqt/ilxqtpanelplugin.h>

#include <QToolButton>
#include <QVector>
#include <QDebug>
#include "qcheckgmail.h"
#include "accounts.h"

class qChechGMailPlugIn : public QObject,public ILxQtPanelPlugin
{
	Q_OBJECT
public:
	qChechGMailPlugIn( const ILxQtPanelPluginStartupInfo& startupInfo ) ;
	~qChechGMailPlugIn() ;
	QWidget * widget() ;
	QString themeId() const ;
	void activated( ILxQtPanelPlugin::ActivationReason reason ) ;
	ILxQtPanelPlugin::Flags flags() const ;
private:
	qCheckGMail * m_gmail ;
};

class qCheckGMailPluginLibrary: public QObject,public ILxQtPanelPluginLibrary
{
	Q_OBJECT
	Q_INTERFACES( ILxQtPanelPluginLibrary )
public:
	ILxQtPanelPlugin * instance( const ILxQtPanelPluginStartupInfo& startupInfo )
	{
		return new qChechGMailPlugIn( startupInfo ) ;
	}
};

#endif // QCHECKGMAILPLUGIN_H
