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
#include <QString>
#include <QByteArray>
#include <QObject>

#include "lxqt_wallet.h"

namespace util
{
	template< typename T >
	struct type_identity{
		using type = T ;
	} ;

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
