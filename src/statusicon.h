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

#ifndef STATUSICON_H
#define STATUSICON_H

#include <QString>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QPainter>

#include <functional>

#include "tray_application_type.h"
#include "accounts.h"
#include "configurationoptionsdialog.h"

#if USE_KDE_STATUS_NOTIFIER
#if KF5
#include <QMenu>
#include <kstatusnotifieritem.h>
#include <knotification.h>
#include <ktoolinvocation.h>
#else
#include <knotification.h>
#include <kstatusnotifieritem.h>
#include <knotification.h>
#include <kmenu.h>
#include <ktoolinvocation.h>
#include <kcmdlineargs.h>
#endif

class statusicon : public KStatusNotifierItem
{

#elif USE_LXQT_PLUGIN
#include <QToolButton>
class statusicon : public QObject
{
#else
#include <QMenu>
class statusicon : public QObject
{
#endif
	Q_OBJECT
public:
	typedef struct{
		std::function< void( void ) > onLeftClick   = [](){} ;
		std::function< void( void ) > onRightClick  = [](){} ;
		std::function< void( void ) > onMiddleClick = [](){} ;
	}clickActions;

	enum ItemCategory {
		ApplicationStatus = 1,
		Communications = 2,
		SystemServices = 3,
		Hardware = 4,
		Reserved = 129
	};
	enum ItemStatus{
		Passive = 1,
		Active = 2,
		NeedsAttention = 3
	};
	statusicon() ;
	virtual ~statusicon() ;
	static void newEmailNotify( void ) ;
	static bool enableDebug( void ) ;
	void setAttentionIcon( const QString& name ) ;
	void setCategory( const ItemCategory category ) ;
	void setIcon( const QString& name ) ;
	void setIcon( const QString& name,int count ) ;
	void setOverlayIcon( const QString& name ) ;
	void setStatus( const statusicon::ItemStatus status ) ;
	void setToolTip( const QString& iconName,const QString& title,const QString& subTitle ) ;
	void setIconClickedActions( const statusicon::clickActions& ) ;
	QAction * getAction( const QString& title = QString() ) ;
	void addAction( QAction * ) ;
	QWidget * widget( void ) ;
	QObject * statusQObject( void ) ;
	QList< QAction * > getMenuActions( void ) ;
	void addQuitAction( void ) ;
private slots:
	void quit( void ) ;
	void activateRequested( bool,const QPoint& ) ;
	void trayIconClicked( QSystemTrayIcon::ActivationReason reason ) ;
private:
	QString m_defaultApplication ;
	statusicon::clickActions m_clickActions ;
#if USE_KDE_STATUS_NOTIFIER
#if KF5
        QMenu * m_menu ;
#else
	KMenu * m_menu ;
#endif
#elif USE_LXQT_PLUGIN
	QToolButton m_toolButton ;
#else
	QMenu * m_menu ;
	QSystemTrayIcon * m_trayIcon ;
#endif
};

#endif // STATUSICON_H
