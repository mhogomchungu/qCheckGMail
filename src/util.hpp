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

#include <QApplication>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include "lxqt_wallet.h"

namespace util
{
	template< typename T >
	struct type_identity{
		using type = T ;
	} ;

	static inline QString labelsToJson( const QString& ids,const QString& names )
	{
		QJsonObject obj ;

		obj.insert( "ids",ids ) ;
		obj.insert( "names",names ) ;

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

	static inline QStringList split( const QString& e )
	{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
		return.split( ",",QString::SkipEmptyParts ) ;
#else
		return e.split( ",",Qt::SkipEmptyParts ) ;
#endif
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

	template< typename MainApp,typename MainAppArgs,typename InstanceArgs >
	class oneinstance
	{
	public:
		oneinstance( const QString& socketPath,
			     const QByteArray& argument,
			     QApplication& app,
			     MainAppArgs args,
			     InstanceArgs iargs ) :
			m_serverPath( socketPath ),
			m_argument( argument ),
			m_qApp( app ),
			m_args( std::move( args ) ),
			m_iargs( std::move( iargs ) ),
			m_exec( [ this ](){ this->run() ; } )
		{
		}
		~oneinstance()
		{
			if( m_localServer.isListening() ){

				m_localServer.close() ;
				QFile::remove( m_serverPath ) ;
			}
		}
		int exec()
		{
			return m_qApp.exec() ;
		}
	private:
		void run()
		{
			if( QFile::exists( m_serverPath ) ){

				QObject::connect( &m_localSocket,&QLocalSocket::connected,[ this ](){

					if( !m_argument.isEmpty() ){

						m_localSocket.write( m_argument ) ;
						m_localSocket.waitForBytesWritten() ;
					}

					m_localSocket.close() ;

					m_iargs.otherInstanceRunning() ;
					m_qApp.quit() ;
				} ) ;

			#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
				using cs = void( QLocalSocket::* )( QLocalSocket::LocalSocketError ) ;

				QObject::connect( &m_localSocket,static_cast< cs >( &QLocalSocket::error ),[ this ]( QLocalSocket::LocalSocketError ){

					m_iargs.otherInstanceCrashed() ;
					QFile::remove( m_serverPath ) ;
					this->start() ;
				} ) ;
			#else
				QObject::connect( &m_localSocket,&QLocalSocket::errorOccurred,[ this ]( QLocalSocket::LocalSocketError ){

					m_iargs.otherInstanceCrashed() ;
					QFile::remove( m_serverPath ) ;
					this->start() ;
				} ) ;
			#endif
				m_localSocket.connectToServer( m_serverPath ) ;
			}else{
				this->start() ;
			}
		}
		void start( void )
		{
			m_mainApp = std::make_unique< MainApp >( std::move( m_args ) ) ;

			m_mainApp->start( m_argument ) ;

			QObject::connect( &m_localServer,&QLocalServer::newConnection,[ this ](){

				auto s = m_localServer.nextPendingConnection() ;

				QObject::connect( s,&QLocalSocket::readyRead,[ this,s ]{

					m_mainApp->event( s->readAll() ) ;
					s->deleteLater() ;
				} ) ;
			} ) ;

			m_localServer.listen( m_serverPath ) ;
		}
		QLocalServer m_localServer ;
		QLocalSocket m_localSocket ;
		QString m_serverPath ;
		const QByteArray& m_argument ;
		QApplication& m_qApp ;
		std::unique_ptr< MainApp > m_mainApp ;
		MainAppArgs m_args ;
		InstanceArgs m_iargs ;
		util::exec m_exec ;
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
		void event( const QByteArray& )
		{
			//This method is called with data from another instance that failed
			//to start because this instance prevented it from starting
			//Argument should be the same type as Args below
		}
		void start( const QByteArray& )
		{
			//This method is called when the first instance is started
			//Argument should be the same type as Args below
		}
	} ;

	template< typename AppType,typename AppConstructorArgs,typename Args,typename Err >
	int runOneInstance( AppType,
			    AppConstructorArgs cargs,
			    const QString& spath,
			    Args opts,
			    QApplication& qapp,
			    Err err )
	{
		using singleInstance = util::oneinstance< typename AppType::type,AppConstructorArgs,Err > ;

		return singleInstance( spath,std::move( opts ),qapp,std::move( cargs ),std::move( err ) ).exec() ;
	}

	template< typename AppType,typename AppConstructorArgs,typename Args >
	int runOneInstance( AppType appType,
			    AppConstructorArgs cargs,
			    const QString& spath,
			    Args opts,
			    QApplication& qapp )
	{
		auto err = util::make_oneinstance_args( [](){

			std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
		},[](){
			std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
		} ) ;

		return util::runOneInstance( appType,std::move( cargs ),spath,std::move( opts ),qapp,std::move( err ) ) ;
	}
}

#endif
