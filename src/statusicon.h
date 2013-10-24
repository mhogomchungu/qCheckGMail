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

#ifndef STATUSICON_H
#define STATUSICON_H

#include <QString>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

#include "tray_application_type.h"
#include "accounts.h"

#if USE_KDE_STATUS_NOTIFIER
#include <knotification.h>
#include <kstatusnotifieritem.h>
#include <knotification.h>
#include <kmenu.h>
#include <ktoolinvocation.h>
#include <kcmdlineargs.h>

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
	statusicon(  const QVector<accounts>& ) ;
	virtual ~statusicon() ;
	static void newEmailNotify( void ) ;
	static bool enableDebug( void ) ;
	void setAttentionIcon( const QString& name ) ;
	void setCategory( const ItemCategory category ) ;
	void setIcon( const QString& name ) ;
	void setOverlayIcon( const QString& name ) ;
	void setStatus( const statusicon::ItemStatus status ) ;
	void setToolTip( const QString& iconName,const QString& title,const QString& subTitle ) ;
	void addAction( QAction * ) ;
	QWidget * widget( void ) ;
	QObject * statusQObject( void ) ;
	QList< QAction * > getMenuActions( void ) ;
	void addQuitAction( void ) ;
private slots:
	void quit( void ) ;
	void activateRequested_1( bool,const QPoint & ) ;
	void trayIconClicked( QSystemTrayIcon::ActivationReason reason ) ;
private:
#if USE_KDE_STATUS_NOTIFIER
	KMenu * m_menu ;
	const QVector<accounts>& m_accounts ;
#elif USE_LXQT_PLUGIN
	QToolButton m_toolButton ;
	const QVector<accounts>& m_accounts ;
#else
	QMenu * m_menu ;
	QSystemTrayIcon * m_trayIcon ;
	const QVector<accounts>& m_accounts ;
#endif
};

#endif // STATUSICON_H
