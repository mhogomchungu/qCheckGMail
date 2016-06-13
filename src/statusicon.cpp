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

#include "../lxqt_wallet/frontend/task.h"

namespace Task = LxQt::Wallet::Task ;

#include <QProcess>

#if !USE_KDE_STATUS_NOTIFIER

static void _playAudioFile()
{
	QProcess exe ;
	exe.start( configurationoptionsdialog::audioPlayer() + " " + AUDIO_NOTIFY_FILE ) ;
	exe.waitForFinished() ;
}

#endif

static QPixmap _icon( const QString& name,int count )
{
	QIcon icon( ":/" + name ) ;
	QPixmap pixmap = icon.pixmap( QSize( 152,152 ),QIcon::Normal,QIcon::On ) ;
	int size = pixmap.height() * 0.01 * configurationoptionsdialog::fontSize() ;
	QPainter paint( &pixmap ) ;
	QFont font( configurationoptionsdialog::fontFamily() ) ;
	QFontMetrics fm( font ) ;
	QString number = QString::number( count ) ;

	paint.setRenderHint( QPainter::SmoothPixmapTransform ) ;
	paint.setRenderHint( QPainter::Antialiasing ) ;

	int width = pixmap.width() * 0.8 ;

	if( fm.width( number ) > width ){
		while( fm.width( number ) > width && size > 0 ){
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

void statusicon::setIconClickedActions( const statusicon::clickActions& actions )
{
	m_clickActions = actions ;
}

#if USE_KDE_STATUS_NOTIFIER

bool statusicon::KF5StatusIcon()
{
	return KF5 ;
}

statusicon::statusicon()
{
#if KF5
	m_menu = new QMenu() ;
#else
	m_menu = new KMenu() ;
#endif
	m_menu->clear() ;
	KStatusNotifierItem::setContextMenu( m_menu ) ;
	KStatusNotifierItem::setStandardActionsEnabled( false ) ;
	connect( this,SIGNAL( activateRequested( bool,const QPoint& ) ),this,SLOT( activateRequested( bool,const QPoint& ) ) ) ;
}

statusicon::~statusicon()
{
	m_menu->deleteLater() ;
}

QWidget * statusicon::widget()
{
        return nullptr ;
}

void statusicon::setAttentionIcon( const QString& name )
{
	KStatusNotifierItem::setAttentionIconByPixmap( QIcon( ":/" + name ) ) ;
}

void statusicon::setCategory( const statusicon::ItemCategory category )
{
	KStatusNotifierItem::setCategory( KStatusNotifierItem::ItemCategory( category ) ) ;
}

void statusicon::setIcon( const QString& name )
{
	KStatusNotifierItem::setIconByPixmap( QIcon( ":/" + name ) ) ;
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
	KStatusNotifierItem::setOverlayIconByPixmap( QIcon( ":/" + name ) ) ;
}

void statusicon::setStatus( const statusicon::ItemStatus status )
{
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

QObject * statusicon::statusQObject()
{
	return this ;
}

void statusicon::newEmailNotify()
{
	QByteArray r( "qCheckGMail" ) ;
#if KF5
	KNotification::event( "qCheckGMail-NewMail","",QPixmap(),0,0,r ) ;
#else
	KNotification::event( "qCheckGMail-NewMail","",QPixmap(),0,0,
			      KComponentData( r,r,KComponentData::SkipMainComponentRegistration ) ) ;
#endif
}

bool statusicon::enableDebug()
{
#if KF5
        return QCoreApplication::arguments().contains( "-d" ) ;
#else
	return KCmdLineArgs::allArguments().contains( "-d" ) ;
#endif
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

void statusicon::quit()
{
	QCoreApplication::exit() ;
}

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
	m_clickActions.onLeftClick() ;
}

void statusicon::trayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
	Q_UNUSED( reason ) ;
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

#elif USE_LXQT_PLUGIN

statusicon::statusicon()
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
	m_toolButton.setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
        auto pixmap = _icon( name,count ) ;
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
        auto r = QString( "<table><tr><td><b>%1</b></td></tr><tr><td>%2</td></tr></table>" ).arg( title,subTitle ) ;
	m_toolButton.setToolTip( r ) ;
}

QList< QAction * > statusicon::getMenuActions()
{
	return m_toolButton.actions() ;
}

QAction * statusicon::getAction( const QString& title )
{
        auto ac = new QAction( &m_toolButton ) ;
	ac->setText( title ) ;
	m_toolButton.addAction( ac ) ;
	return ac ;
}

void statusicon::addQuitAction()
{
}

void statusicon::newEmailNotify()
{
	Task::exec( [](){ _playAudioFile() ; } ) ;
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

void statusicon::activateRequested( bool x,const QPoint& y )
{
	Q_UNUSED( x ) ;
	Q_UNUSED( y ) ;
}

QObject * statusicon::statusQObject()
{
	return this ;
}

bool statusicon::KF5StatusIcon()
{
	return false ;
}

#else

statusicon::statusicon()
{
	m_trayIcon = new QSystemTrayIcon( this ) ;

        m_menu = new QMenu() ;

        connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		this,SLOT( trayIconClicked(QSystemTrayIcon::ActivationReason ) ) ) ;

        m_trayIcon->setContextMenu( m_menu ) ;
}

bool statusicon::KF5StatusIcon()
{
	return false ;
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
	m_trayIcon->setIcon( QIcon( ":/" + name ) ) ;
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
	m_trayIcon->setIcon( QIcon( ":/" + name ) ) ;
}

void statusicon::setIcon( const QString& name,int count )
{
        auto pixmap = _icon( name,count ) ;
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
        auto r = QString( "<table><tr><td><b>%1</b></td></tr><tr><td>%2</td></tr></table>" ).arg( title,subTitle ) ;
	m_trayIcon->setToolTip( r ) ;
}

QList<QAction *> statusicon::getMenuActions()
{
	return m_menu->actions() ;
}

void statusicon::addQuitAction()
{
        m_menu->addAction( [ this ](){

                auto ac = new QAction( m_menu ) ;
		ac->setText( tr( "Quit" ) ) ;
                connect( ac,SIGNAL( triggered() ),this,SLOT( quit() ) ) ;

                return ac ;
        }() ) ;

	m_trayIcon->show() ;
}

void statusicon::newEmailNotify()
{
	Task::exec( [](){ _playAudioFile() ; } ) ;
}

bool statusicon::enableDebug()
{
	return QCoreApplication::arguments().contains( "-a" ) ;
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

void statusicon::activateRequested( bool x,const QPoint& y )
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


