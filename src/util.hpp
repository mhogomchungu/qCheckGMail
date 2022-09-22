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
#include <QJsonArray>
#include <QJsonDocument>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QLockFile>
#include <QThread>
#include <QEventLoop>

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
		return e.split( token,QString::SkipEmptyParts ) ;
#else
		return e.split( token,Qt::SkipEmptyParts ) ;
#endif
	}
	template< typename Labels >
	QString labelsToJson( const QString& userLabels,const Labels& labels )
	{
		auto autoaccLabelOrgList = util::split( userLabels ) ;

		QJsonArray arr ;

		for( const auto& it : autoaccLabelOrgList ){

			for( const auto& xt : labels ){

				if( it == xt.name ){

					QJsonObject obj ;

					obj.insert( "id",xt.id ) ;
					obj.insert( "name",it ) ;

					arr.append( obj ) ;
				}
			}
		}

		return QJsonDocument( arr ).toJson( QJsonDocument::JsonFormat::Compact ) ;
	}
	struct idAndName
	{
		QString id ;
		QString name ;
	} ;
	static inline std::vector< util::idAndName > idsAndNamesFromJson( const QString& e )
	{
		std::vector< util::idAndName > s ;

		for( const auto& it : QJsonDocument::fromJson( e.toUtf8() ).array() ){

			auto obj = it.toObject() ;

			auto id   = obj.value( "id" ).toString() ;
			auto name = obj.value( "name" ).toString() ;

			s.emplace_back( util::idAndName{ std::move( id ),std::move( name ) } ) ;
		}

		return s ;
	}
	static inline QString namesFromJson( const QString& e )
	{
		auto m = util::idsAndNamesFromJson( e ) ;

		auto it = m.begin() ;

		if( it == m.end() ){

			return {} ;
		}else{
			auto s = it->name ;

			it++ ;

			while( it != m.end() ){

				s += "," + it->name ;

				it++ ;
			}

			return s ;
		}
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
}

#endif
