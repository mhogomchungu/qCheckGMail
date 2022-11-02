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

#include "statusicon.h"

#include <QProcess>
#include <iostream>

static QIcon _getIcon( settings&,const QString& name )
{
	return QIcon( ":/" + name ) ;

	//return QIcon::fromTheme( name,QIcon( ":/" + name ) ) ;
}

static QPixmap _icon( const QIcon& icon,int count,settings& s )
{
	QPixmap pixmap = icon.pixmap( QSize( 152,152 ),QIcon::Normal,QIcon::On ) ;
	int size = pixmap.height() * static_cast< int >( 0.01 * s.fontSize() ) ;
	QPainter paint( &pixmap ) ;
	QFont font( s.fontFamily() ) ;
	QFontMetrics fm( font ) ;
	QString number = QString::number( count ) ;

	paint.setRenderHint( QPainter::SmoothPixmapTransform ) ;
	paint.setRenderHint( QPainter::Antialiasing ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )

	int width = static_cast< int >( pixmap.width() * 0.8 ) ;

	if( fm.width( number ) > width ){

		while( fm.width( number ) > width && size > 0 ){

			size = size - 1 ;
			font.setPointSize( size ) ;
		}
	}

#else
	font.setPointSize( size ) ;
	size = s.fontSize() ;
#endif
	font.setPixelSize( size ) ;
	font.setBold( true ) ;
	paint.setFont( font ) ;
	paint.setPen( QColor( s.fontColor() ) ) ;
	paint.drawText( pixmap.rect(),Qt::AlignVCenter | Qt::AlignHCenter,number ) ;
	paint.end() ;

	return pixmap ;
}

#if KF5

statusicon::statusicon( settings& s,statusicon::clickActions ac ) :
	m_menu( new QMenu() ),
	m_settings( s ),
	m_clickActions( std::move( ac ) )
{
	m_menu->clear() ;

	KStatusNotifierItem::setContextMenu( m_menu ) ;
	KStatusNotifierItem::setStandardActionsEnabled( false ) ;

	connect( this,SIGNAL( activateRequested( bool,const QPoint& ) ),
		 this,SLOT( activateRequested( bool,const QPoint& ) ) ) ;
}

statusicon::~statusicon()
{
}

void statusicon::setAttentionIcon( const QString& name )
{
	KStatusNotifierItem::setAttentionIconByPixmap( _getIcon( m_settings,name ) ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	KStatusNotifierItem::setCategory( KStatusNotifierItem::ItemCategory( category ) ) ;
}

void statusicon::setIcon( const QString& name )
{
	auto icon = _getIcon( m_settings,name ) ;
	KStatusNotifierItem::setIconByPixmap( icon ) ;
	statusicon::setAttentionIcon( name ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	auto icon = _getIcon( m_settings,name ) ;
	QPixmap pixmap = _icon( icon,count,m_settings ) ;
	KStatusNotifierItem::setIconByPixmap( pixmap ) ;
	KStatusNotifierItem::setAttentionIconByPixmap( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	KStatusNotifierItem::setOverlayIconByPixmap( QIcon( ":/" + name ) ) ;
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
	m_status = status ;
	KStatusNotifierItem::setStatus( KStatusNotifierItem::ItemStatus( status ) ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	KStatusNotifierItem::setToolTip( QString(),title,subTitle ) ;
	KStatusNotifierItem::setToolTipIconByPixmap( QIcon( ":/" + iconName ) ) ;
}

void statusicon::addAction( QAction * ac )
{
	ac->setParent( m_menu ) ;
	m_menu->addAction( ac ) ;
}

QAction * statusicon::getAction( const QString& title )
{
	auto ac = new QAction( m_menu ) ;
	ac->setText( title ) ;
	m_menu->addAction( ac ) ;
	return ac ;
}

const statusicon::clickActions& statusicon::getClickActions()
{
	return m_clickActions ;
}

QMenu * statusicon::getMenu( const QString& title )
{
	return m_menu->addMenu( title ) ;
}

QMenu * statusicon::getOGMenu()
{
	return m_menu ;
}

statusicon::ItemStatus statusicon::getStatus()
{
	return m_status ;
}

void statusicon::newEmailNotify()
{
	KNotification::event( "qCheckGMail-NewMail",
			      "",
			      QPixmap(),
			      nullptr,
			      KNotification::CloseOnTimeout,
			      "qCheckGMail" ) ;
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-d" ) ;
}

void statusicon::quit()
{
	QCoreApplication::exit() ;
}

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x )
	Q_UNUSED( y )
	m_clickActions.onLeftClick() ;
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason )
}

void statusicon::addQuitAction()
{
	m_menu->addAction( [ this ](){

		auto ac = new QAction( m_menu ) ;

		ac->setText( tr( "Quit" ) ) ;
		connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;

		return ac ;
	}() ) ;
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

#else

statusicon::statusicon( settings& s,statusicon::clickActions ac ) :
	m_settings( s ),
	m_clickActions( std::move( ac ) )
{
	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		this,SLOT( trayIconClicked(QSystemTrayIcon::ActivationReason ) ) ) ;

	m_trayIcon.setContextMenu( &m_menu ) ;
}

statusicon::~statusicon()
{
}

const statusicon::clickActions& statusicon::getClickActions()
{
	return m_clickActions ;
}

statusicon::ItemStatus statusicon::getStatus()
{
	return m_status ;
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_menu.actions() ;
}

void statusicon::setAttentionIcon( const QString& name )
{
	auto icon = _getIcon( m_settings,name ) ;
	m_trayIcon.setIcon( icon ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	Q_UNUSED( category )
}

void statusicon::quit()
{
	QCoreApplication::quit() ;
}

void statusicon::setIcon( const QString& name )
{
	auto icon = _getIcon( m_settings,name ) ;
	m_trayIcon.setIcon( icon ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	auto icon = _getIcon( m_settings,name ) ;
	QPixmap pixmap = _icon( icon,count,m_settings ) ;
	m_trayIcon.setIcon( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	Q_UNUSED( name )
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
	m_status = status ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	Q_UNUSED( iconName )
	Q_UNUSED( title )
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )

	auto r = QString( "<table><tr><td><b>%1<br></b></td></tr><tr><td>%2</td></tr></table>" ).arg( title,subTitle ) ;
	m_trayIcon.setToolTip( r ) ;
#else
	auto r = QString( "%1\n%2" ).arg( title,subTitle ) ;
	r.replace( "<table>","" ) ;
	r.replace( "<b>","" ) ;
	r.replace( "<br>","\n" ) ;
	r.replace( "</table>","" ) ;
	r.replace( "</b>","" ) ;
	m_trayIcon.setToolTip( r ) ;
#endif
}

void statusicon::addQuitAction()
{
	m_menu.addAction( [ this ](){

		auto ac = new QAction( &m_menu ) ;
		ac->setText( tr( "Quit" ) ) ;
		connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;

		return ac ;
	}() ) ;

	m_trayIcon.show() ;
}

void statusicon::newEmailNotify()
{
	QProcess::startDetached( m_settings.audioPlayer(),{ AUDIO_NOTIFY_FILE } ) ;
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-d" ) ;
}

void statusicon::addAction( QAction * ac )
{
	ac->setParent( &m_menu ) ;
	m_menu.addAction( ac ) ;
}

QAction * statusicon::getAction( const QString& title )
{
	auto ac = new QAction( &m_menu ) ;
	ac->setText( title ) ;
	m_menu.addAction( ac ) ;
	return ac ;
}

QMenu * statusicon::getMenu( const QString& title )
{
	return m_menu.addMenu( title ) ;
}

QMenu * statusicon::getOGMenu()
{
	return &m_menu ;
}

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x )
	Q_UNUSED( y )
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	if( reason == QSystemTrayIcon::Context ){

		m_clickActions.onRightClick() ;

	}else if( reason == QSystemTrayIcon::Trigger ){

		m_clickActions.onLeftClick() ;

	}else if( reason == QSystemTrayIcon::MiddleClick ){

		m_clickActions.onMiddleClick() ;
	}else{
		m_clickActions.onRightClick() ;
	}
}

#endif

statusicon::clickActionsInterface::~clickActionsInterface()
{
}
