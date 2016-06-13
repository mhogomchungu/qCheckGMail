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

#ifndef QCHECKGMAILPLUGIN_H
#define QCHECKGMAILPLUGIN_H

#include <QObject>

/*
 * Bogus class to silence a build warning
 */
class foo : public QObject
{
	Q_OBJECT
public:
	foo()
	{
	}
};

#ifdef USE_LXQT_PLUGIN
#if USE_LXQT_PLUGIN

#include <QToolButton>
#include <QVector>
#include <QDebug>
#include <QProcess>
#include <QString>
#include "qcheckgmail.h"
#include "accounts.h"
#include "lxqt_api.h"

#if NEW_LXQT_API

#include <ilxqtpanel.h>
#include <ilxqtpanelplugin.h>

class qCheckGMailPlugIn : public QObject,public ILXQtPanelPlugin
{
	Q_OBJECT
public:
	qCheckGMailPlugIn( const ILXQtPanelPluginStartupInfo& startupInfo ) ;
	~qCheckGMailPlugIn() ;
	QWidget * widget() ;
	QString themeId() const ;
	void activated( ILXQtPanelPlugin::ActivationReason reason ) ;
	ILXQtPanelPlugin::Flags flags() const ;
private:
	qCheckGMail * m_gmail ;
};

class qCheckGMailPluginLibrary: public QObject,public ILXQtPanelPluginLibrary
{
	Q_OBJECT
	Q_INTERFACES( ILXQtPanelPluginLibrary )
public:
	ILXQtPanelPlugin * instance( const ILXQtPanelPluginStartupInfo& startupInfo )
	{
		return new qCheckGMailPlugIn( startupInfo ) ;
	}
};

#else

#include <ilxqtpanel.h>
#include <ilxqtpanelplugin.h>

class qCheckGMailPlugIn : public QObject,public ILxQtPanelPlugin
{
	Q_OBJECT
public:
	qCheckGMailPlugIn( const ILxQtPanelPluginStartupInfo& startupInfo ) ;
	~qCheckGMailPlugIn() ;
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
		return new qCheckGMailPlugIn( startupInfo ) ;
	}
};

#endif

#endif // QCHECKGMAILPLUGIN_H

#endif
#endif
