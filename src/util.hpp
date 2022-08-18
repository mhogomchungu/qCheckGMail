/*
 *
 *  Copyright (c) 2022
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

#ifndef UTIL_H
#define UTIL_H

#include <memory>
#include <iostream>
#include <cstring>

#include <QApplication>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QLockFile>

#include "lxqt_wallet.h"

static inline std::ostream& operator<<( std::ostream& cout,const QString& s )
{
	cout << s.toUtf8().constData() ;
	return cout ;
}

static inline std::ostream& operator<<( std::ostream& cout,const QByteArray& s )
{
	cout << s.constData() ;
	return cout ;
}

static inline std::ostream& operator<<( std::ostream& cout,const QStringList& s )
{
	std::cout << "(" ;

	for( const auto& it : s ){

		std::cout << it.toUtf8().constData() ;
	}

	std::cout << ")" ;

	return cout ;
}

namespace util
{
	template< typename T >
	struct type_identity{
		using type = T ;
	} ;
	static inline QStringList split( const QString& e,char token = ',' )
	{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
		return.split( token,QString::SkipEmptyParts ) ;
#else
		return e.split( token,Qt::SkipEmptyParts ) ;
#endif
	}
	template< typename Labels >
	QString labelsToJson( const QString& userLabels,const Labels& labels )
	{
		auto autoaccLabelOrgList = util::split( userLabels ) ;

		QStringList labelIds ;

		for( const auto& it : autoaccLabelOrgList ){

			for( const auto& xt : labels ){

				if( it == xt.name ){

					labelIds.append( xt.id ) ;
				}
			}
		}

		QJsonObject obj ;

		obj.insert( "ids",labelIds.join( "," ) ) ;
		obj.insert( "names",userLabels ) ;

		return QJsonDocument( obj ).toJson( QJsonDocument::Compact ) ;
	}
	static inline QString namesFromJson( const QString& e )
	{
		auto m = QJsonDocument::fromJson( e.toUtf8() ).object() ;

		return m.find( "names" )->toString() ;
	}

	static inline QString idsFromJson( const QString& e )
	{
		auto m = QJsonDocument::fromJson( e.toUtf8() ).object() ;

		return m.find( "ids" )->toString() ;
	}
	template< typename Signal,typename Slot,typename QObj,typename SignalSource >
	void connect( Signal s,Slot l,QObj obj,SignalSource ac )
	{
		obj->connect( ac,s,obj,l ) ;
	}

	class urlOpts
	{
	public:
		urlOpts( const QString& url ) : m_values( url + "?" )
		{
		}
		urlOpts()
		{
		}
		urlOpts& add( const QString& key,const QString& value )
		{
			m_values += key + "=" + value + "&" ;
			return *this ;
		}
		QByteArray toUtf8() const
		{
			auto m = m_values.toUtf8() ;
			m.truncate( m.size() - 1 ) ;
			return m ;
		}
		QString toString() const
		{
			auto m = m_values ;
			m.truncate( m.size() - 1 ) ;
			return m ;
		}
	private:
		QString m_values ;
	};

	class unique_wallet_ptr
	{
	public:
		unique_wallet_ptr() : m_handle( nullptr,[]( QObject * e ){ e->deleteLater() ; } )
		{
		}
		unique_wallet_ptr( LXQt::Wallet::Wallet * w ) :
			m_handle( std::move( w ),[]( QObject * e ){ e->deleteLater() ; } )
		{
		}
		LXQt::Wallet::Wallet * get()
		{
			return m_handle.get() ;
		}
		LXQt::Wallet::Wallet * operator->()
		{
			return m_handle.operator->() ;
		}
		LXQt::Wallet::Wallet * release()
		{
			return m_handle.release() ;
		}
		operator bool()
		{
			return this->get() ;
		}
	private:
		std::unique_ptr< LXQt::Wallet::Wallet,void( * )( QObject * ) > m_handle ;
	} ;

	#if QT_VERSION < QT_VERSION_CHECK( 5,4,0 )
		class exec : public QObject
		{
			Q_OBJECT
		public:
			exec( std::function< void() > function ) : m_function( std::move( function ) )
			{
				QTimer::singleShot( 0,this,SLOT( run() ) ) ;
			}
		private slots:
			void run()
			{
				m_function() ;
			}
		private:
			std::function< void() > m_function ;
		} ;
	#else
		class exec
		{
		public:
			template< typename Function >
			exec( Function function )
			{
				QTimer::singleShot( 0,[ function = std::move( function ) ]{

					function() ;
				} ) ;
			}
		private:
		} ;
	#endif

	template< typename Type,typename TypeArgs >
	struct appInfo
	{
		appInfo( TypeArgs t,const QString& s,QApplication& a ) :
			args( std::move( t ) ),socketPath( s ),app( a )
		{
		}
		using appType = Type ;
		TypeArgs args ;
		QString socketPath ;
		QApplication& app ;
		QByteArray data ;
	} ;

	template< typename AppInfo >
	class multipleInstance
	{
	public:
		multipleInstance( AppInfo info ) :
			m_info( std::move( info ) ),
			m_exec( [ this ](){ this->run() ; } )
		{
		}
		void run()
		{
			m_mainApp = std::make_unique< typename AppInfo::appType >( std::move( m_info.args ) ) ;
			m_mainApp->start( m_info.data ) ;
		}
		int exec()
		{
			return m_info.app.exec() ;
		}
	private:
		AppInfo m_info ;
		std::unique_ptr< typename AppInfo::appType > m_mainApp ;
		util::exec m_exec ;
	} ;

	template< typename AppInfo >
	int runMultiInstances( AppInfo info )
	{
		return multipleInstance< AppInfo >( std::move( info ) ).exec() ;
	}

	template< typename OIR,typename PIC >
	struct instanceArgs
	{
		OIR otherInstanceRunning ;
		PIC otherInstanceCrashed ;
	} ;

	template< typename OIR,typename PIC >
	auto make_oneinstance_args( OIR r,PIC c )
	{
		return instanceArgs< OIR,PIC >{ std::move( r ),std::move( c ) } ;
	}

	template< typename AppInfo,typename InstanceArgs >
	class oneinstance
	{
	public:
		oneinstance( AppInfo info,InstanceArgs iargs ) :
			m_info( std::move( info ) ),
			m_iargs( std::move( iargs ) ),
			m_exec( [ this ](){ this->run() ; } ),
			m_lockFile( m_info.socketPath + ".lock" )
		{
			m_lockFile.lock() ;
		}
		~oneinstance()
		{
			if( m_localServer.isListening() ){

				m_localServer.close() ;
				QFile::remove( m_info.socketPath ) ;
			}
		}
		int exec()
		{
			return m_info.app.exec() ;
		}
	private:
		void run()
		{
			if( QFile::exists( m_info.socketPath ) ){

				QObject::connect( &m_localSocket,&QLocalSocket::connected,[ this ](){

					if( !m_info.data.isEmpty() ){

						m_localSocket.write( m_info.data ) ;
						m_localSocket.waitForBytesWritten() ;
					}

					m_localSocket.close() ;

					m_iargs.otherInstanceRunning() ;

					m_lockFile.unlock() ;

					m_info.app.quit() ;
				} ) ;

			#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
				using cs = void( QLocalSocket::* )( QLocalSocket::LocalSocketError ) ;

				QObject::connect( &m_localSocket,static_cast< cs >( &QLocalSocket::error ),[ this ]( QLocalSocket::LocalSocketError ){

					m_iargs.otherInstanceCrashed() ;
					QFile::remove( m_info.socketPath ) ;
					this->start() ;
				} ) ;
			#else
				QObject::connect( &m_localSocket,&QLocalSocket::errorOccurred,[ this ]( QLocalSocket::LocalSocketError ){

					m_iargs.otherInstanceCrashed() ;
					QFile::remove( m_info.socketPath ) ;
					this->start() ;
				} ) ;
			#endif
				m_localSocket.connectToServer( m_info.socketPath ) ;
			}else{
				this->start() ;
			}
		}
		void start()
		{
			m_mainApp = std::make_unique< typename AppInfo::appType >( std::move( m_info.args ) ) ;

			m_mainApp->start( std::move( m_info.data ) ) ;

			QObject::connect( &m_localServer,&QLocalServer::newConnection,[ this ](){

				auto s = m_localServer.nextPendingConnection() ;

				QObject::connect( s,&QLocalSocket::readyRead,[ this,s ]{

					m_mainApp->event( s->readAll() ) ;
					s->deleteLater() ;
				} ) ;
			} ) ;

			m_localServer.listen( m_info.socketPath ) ;

			m_lockFile.unlock() ;
		}
		QLocalServer m_localServer ;
		QLocalSocket m_localSocket ;
		std::unique_ptr< typename AppInfo::appType > m_mainApp ;
		AppInfo m_info ;
		InstanceArgs m_iargs ;
		util::exec m_exec ;
		QLockFile m_lockFile ;
	} ;

	class AppTypeInterface
	{
	public:
		struct args
		{
			QApplication& app ;
		} ;
		AppTypeInterface( const AppTypeInterface::args& )
		{
		}
		void event( QByteArray )
		{
			//This method is called with data from another instance that failed
			//to start because this instance prevented it from starting
		}
		void start( QByteArray )
		{
			//This method is called when the first instance is started
		}
	} ;

	template< typename AppInfo,typename Err >
	int runOneInstance( AppInfo info,Err err )
	{
		return util::oneinstance< AppInfo,Err >( std::move( info ),std::move( err ) ).exec() ;
	}

	template< typename AppInfo >
	int runOneInstance( AppInfo info )
	{
		auto err = util::make_oneinstance_args( [](){

			std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
		},[](){
			std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
		} ) ;

		return util::runOneInstance( std::move( info ),std::move( err ) ) ;
	}
}

#endif
