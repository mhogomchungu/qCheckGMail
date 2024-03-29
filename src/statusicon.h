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
#include <QMenu>

#include <functional>

#include "tray_application_type.h"
#include "audio_file_path.h"
#include "accounts.h"
#include "configurationoptionsdialog.h"
#include "settings.h"

#if KF5
#include <kstatusnotifieritem.h>
#include <knotification.h>

class statusicon : public KStatusNotifierItem
{
#else
class statusicon : public QObject
{
#endif
	Q_OBJECT
public:
	struct clickActionsInterface
	{
		virtual void onLeftClick() const
		{
		}
		virtual void onRightClick() const
		{
		}
		virtual void onMiddleClick() const
		{
		}
		virtual void onDoubleClick() const
		{
		}
		virtual ~clickActionsInterface() ;
	};

	class clickActions
	{
	public:
		template< typename Type,typename ... Args >
		clickActions( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void onLeftClick() const
		{
			m_handle->onLeftClick() ;
		}
		void onRightClick() const
		{
			m_handle->onRightClick() ;
		}
		void onDoubleClick() const
		{
			m_handle->onDoubleClick() ;
		}
		void onMiddleClick() const
		{
			m_handle->onMiddleClick() ;
		}
	private:
		std::unique_ptr< statusicon::clickActionsInterface > m_handle ;
	} ;

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
	statusicon( settings&,statusicon::clickActions ) ;
	~statusicon() override ;
	static bool enableDebug() ;
	void newEmailNotify() ;
	void setAttentionIcon( const QString& name ) ;
	void setCategory( const ItemCategory category ) ;
	void setIcon( const QString& name ) ;
	void setIcon( const QString& name,int count ) ;
	void setOverlayIcon( const QString& name ) ;
	void setStatus( const statusicon::ItemStatus status ) ;
	void setToolTip( const QString& iconName,const QString& title,const QString& subTitle ) ;
	QAction * getAction( const QString& title = QString() ) ;
	const statusicon::clickActions& getClickActions() ;
	QMenu * getMenu( const QString& ) ;
	QMenu * getOGMenu() ;
	statusicon::ItemStatus getStatus() ;
	void addAction( QAction * ) ;
	QList< QAction * > getMenuActions() ;
	void addQuitAction() ;
	QSystemTrayIcon& trayIcon()
	{
		return m_trayIcon ;
	}
private slots:
	void quit() ;
	void activateRequested( bool,const QPoint& ) ;
	void trayIconClicked( QSystemTrayIcon::ActivationReason reason ) ;
private:
#if KF5
	QMenu * m_menu ;
#else
	QMenu m_menu ;
#endif
	statusicon::ItemStatus m_status ;
	QString m_defaultApplication ;
	QSystemTrayIcon m_trayIcon ;
	settings& m_settings ;
	statusicon::clickActions m_clickActions ;
};

#endif // STATUSICON_H
