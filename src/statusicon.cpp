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

#include "statusicon.h"

#include <QDebug>

static QPixmap _icon( const QString& name,int count )
{
	QIcon icon( QString( ":/" ) + name ) ;
	QPixmap pixmap = icon.pixmap( QSize( 152,152 ),QIcon::Normal,QIcon::On ) ;
	int size = pixmap.height() * 0.01 * configurationoptionsdialog::fontSize() ;
	QPainter paint( &pixmap ) ;
	QFont font( configurationoptionsdialog::fontFamily() ) ;
	QFontMetrics fm( font ) ;
	QString number = QString::number( count ) ;

	paint.setRenderHint( QPainter::SmoothPixmapTransform ) ;
	paint.setRenderHint( QPainter::Antialiasing ) ;

	if( fm.width( number ) > pixmap.width() ) {
		while( fm.width( number ) > pixmap.width() && size > 0 ){
			size = size - 1 ;
			font.setPointSize( size ) ;
		}
	}

	font.setPixelSize( size ) ;
	font.setBold( true ) ;
	paint.setFont( font ) ;
	paint.setPen( QColor( configurationoptionsdialog::fontColor() ) ) ;
	paint.drawText( pixmap.rect(),Qt::AlignVCenter | Qt::AlignHCenter,number ) ;
	paint.end() ;
	return pixmap ;
}

#if USE_KDE_STATUS_NOTIFIER

statusicon::statusicon( const QVector<accounts>& acc ) : m_accounts( acc )
{
	m_menu = new KMenu() ;
	KStatusNotifierItem::setContextMenu( m_menu ) ;
	connect( this,SIGNAL( activateRequested( bool,QPoint ) ),this,SLOT( activateRequested_1( bool,QPoint ) ) ) ;
}

statusicon::~statusicon()
{
	m_menu->deleteLater() ;
}

QWidget * statusicon::widget()
{
	return 0 ;
}

void statusicon::setAttentionIcon( const QString& name )
{
	KStatusNotifierItem::setAttentionIconByPixmap( QIcon( QString( ":/" ) + name ) ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	KStatusNotifierItem::setCategory( KStatusNotifierItem::ItemCategory( category ) ) ;
}

void statusicon::setIcon( const QString& name )
{
	KStatusNotifierItem::setIconByPixmap( QIcon( QString( ":/" ) + name ) ) ;
	statusicon::setAttentionIcon( name ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	QPixmap pixmap = _icon( name,count ) ;
	KStatusNotifierItem::setIconByPixmap( pixmap ) ;
	KStatusNotifierItem::setAttentionIconByPixmap( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	KStatusNotifierItem::setOverlayIconByPixmap( QIcon( QString( ":/" ) + name ) ) ;
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
	KStatusNotifierItem::setStatus( KStatusNotifierItem::ItemStatus( status ) ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	KStatusNotifierItem::setToolTip( QString( "" ),title,subTitle ) ;
	KStatusNotifierItem::setToolTipIconByPixmap( QIcon( QString( ":/" ) + iconName ) ) ;
}

void statusicon::addAction( QAction * ac )
{
	m_menu->addAction( ac ) ;
}

QObject * statusicon::statusQObject()
{
	return this ;
}

void statusicon::newEmailNotify()
{
	QByteArray r( "qCheckGMail" ) ;
	KNotification::event( QString( "qCheckGMail-NewMail" ),QString( "" ),QPixmap(),0,0,
			      KComponentData( r,r,KComponentData::SkipMainComponentRegistration ) ) ;
}

bool statusicon::enableDebug()
{
	return KCmdLineArgs::allArguments().contains( "-d" ) ;
}

QList<QAction *> statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

void statusicon::quit()
{
}

void statusicon::activateRequested_1( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
	if( m_accounts.size() > 0 ){
		QString url = m_accounts.at( 0 ).defaultLabelUrl() ;
		int index = url.size() - QString( "/feed/atom/" ).size() ;
		url.truncate( index ) ;
		KToolInvocation::invokeBrowser( url ) ;
	}else{
		KToolInvocation::invokeBrowser( "https://mail.google.com/" ) ;
	}
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason ) ;
}

void statusicon::addQuitAction()
{
}

#elif USE_LXQT_PLUGIN

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

statusicon::statusicon( const QVector<accounts>& acc ) : m_accounts( acc )
{
	m_toolButton.setPopupMode( QToolButton::InstantPopup ) ;
}

statusicon::~statusicon()
{
}

void statusicon::setAttentionIcon( const QString& name )
{
	Q_UNUSED( name ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	Q_UNUSED( category ) ;
}

void statusicon::quit()
{
	QCoreApplication::quit() ;
}

void statusicon::setIcon( const QString& name )
{
	m_toolButton.setIcon( QIcon( QString( ":/" ) + name ) ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	QPixmap pixmap = _icon( name,count ) ;
	m_toolButton.setIcon( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	Q_UNUSED( name ) ;
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
	Q_UNUSED( status ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	Q_UNUSED( iconName ) ;
	Q_UNUSED( title ) ;
	m_toolButton.setToolTip( subTitle ) ;
}

QList<QAction *> statusicon::getMenuActions()
{
	QList<QAction *> l ;
	return l ;
}

void statusicon::addQuitAction()
{
}

void statusicon::newEmailNotify()
{
	Phonon::MediaObject * media = new Phonon::MediaObject() ;
	Phonon::AudioOutput * output = new Phonon::AudioOutput( Phonon::MusicCategory,media ) ;
	Phonon::createPath( media,output ) ;
	media->setCurrentSource( QString( AUDIO_NOTIFY_FILE ) ) ;
	media->play() ;
	connect( media,SIGNAL( finished() ),media,SLOT( deleteLater() ) ) ;
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason ) ;
}

bool statusicon::enableDebug()
{
	return false ;
}

void statusicon::addAction( QAction * ac )
{
	m_toolButton.addAction( ac ) ;
}

QWidget * statusicon::widget()
{
	return &m_toolButton ;
}

void statusicon::activateRequested_1( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
}

QObject * statusicon::statusQObject()
{
	return this ;
}

#else

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

statusicon::statusicon( const QVector<accounts>& acc ) : m_accounts( acc )
{
	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_menu = new QMenu() ;
	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		this,SLOT( trayIconClicked(QSystemTrayIcon::ActivationReason ) ) ) ;
	m_trayIcon->setContextMenu( m_menu ) ;
}

QWidget * statusicon::widget()
{
	return 0 ;
}

statusicon::~statusicon()
{
	m_menu->deleteLater() ;
}

void statusicon::setAttentionIcon( const QString& name )
{
	m_trayIcon->setIcon( QIcon( QString( ":/" ) + name ) ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	Q_UNUSED( category ) ;
}

void statusicon::quit()
{
	QCoreApplication::quit() ;
}

void statusicon::setIcon( const QString& name )
{
	m_trayIcon->setIcon( QIcon( QString( ":/" ) + name ) ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
	QPixmap pixmap = _icon( name,count ) ;
	m_trayIcon->setIcon( pixmap ) ;
}

void statusicon::setOverlayIcon( const QString& name )
{
	Q_UNUSED( name ) ;
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
	Q_UNUSED( status ) ;
}

void statusicon::setToolTip( const QString& iconName,const QString& title,const QString& subTitle )
{
	Q_UNUSED( iconName ) ;
	Q_UNUSED( title ) ;
	m_trayIcon->setToolTip( subTitle ) ;
}

QList<QAction *> statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

void statusicon::addQuitAction()
{
	QAction * ac = new QAction( m_menu ) ;
	ac->setText( tr( "quit" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;
	m_menu->addAction( ac ) ;
	m_trayIcon->show() ;
}

void statusicon::newEmailNotify()
{
	Phonon::MediaObject * media = new Phonon::MediaObject() ;
	Phonon::AudioOutput * output = new Phonon::AudioOutput( Phonon::MusicCategory,media ) ;
	Phonon::createPath( media,output ) ;
	media->setCurrentSource( QString( AUDIO_NOTIFY_FILE ) ) ;
	media->play() ;
	connect( media,SIGNAL( finished() ),media,SLOT( deleteLater() ) ) ;
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-a" ) ;
}

void statusicon::addAction( QAction * ac )
{
	m_menu->addAction( ac ) ;
}

void statusicon::activateRequested_1( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
}

QObject * statusicon::statusQObject()
{
	return this ;
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	if( reason != QSystemTrayIcon::Context ){
		if( m_accounts.size() > 0 ){
			QString url = m_accounts.at( 0 ).defaultLabelUrl() ;
			int index = url.size() - QString( "/feed/atom/" ).size() ;
			url.truncate( index ) ;
			QDesktopServices::openUrl( QUrl( url ) ) ;
		}else{
			QDesktopServices::openUrl( QUrl( "https://mail.google.com/" ) ) ;
		}
	}
}

#endif


