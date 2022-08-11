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

#ifndef GMAILAUTHORIZATION_H
#define GMAILAUTHORIZATION_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QtNetwork/QTcpServer>

namespace Ui {
class gmailauthorization;
}

class gmailauthorization : public QDialog
{
        Q_OBJECT
public:
	struct authResult
	{
		virtual void operator()( const QString&,const QByteArray& )
		{
		}
		virtual ~authResult() ;
	} ;

	class AuthResult
	{
	public:
		template< typename Type,typename ... Args >
		AuthResult( Type,Args&& ... args ) :
			m_handle( std::make_shared< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void operator()( const QString& e,const QByteArray& s ) const
		{
			( *m_handle )( e,s ) ;
		}
	private:
		std::shared_ptr< gmailauthorization::authResult > m_handle ;
	} ;

	struct authActions
	{
		virtual void operator()( const QString&,AuthResult )
		{
		}
		virtual ~authActions() ;
	} ;

	class getAuth
	{
	public:
		getAuth() : m_handle( std::make_unique< gmailauthorization::authActions >() )
		{
		}
		template< typename Type,typename ... Args >
		getAuth( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void operator()( const QString& authocode,AuthResult result )
		{
			( *m_handle )( authocode,std::move( result ) ) ;
		}
	private:
		std::unique_ptr< gmailauthorization::authActions > m_handle ;
	};

	struct actions
	{
		virtual void cancel()
		{
		}
		virtual void getToken( const QString&,const QByteArray& )
		{
		}
		virtual ~actions() ;
	} ;

	class Actions
	{
	public:
		template< typename Type,typename ... Args >
		Actions( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void cancel()
		{
			m_handle->cancel() ;
		}
		void getToken( const QString& e,const QByteArray& s )
		{
			m_handle->getToken( e,s ) ;
		}
	private:
		std::unique_ptr< gmailauthorization::actions > m_handle ;
	} ;

        static void instance( QDialog * parent,
			      gmailauthorization::getAuth& r,
			      gmailauthorization::Actions e )
        {
		new gmailauthorization( parent,r,std::move( e ) ) ;
        }

        gmailauthorization( QDialog * parent,
			    gmailauthorization::getAuth&,
			    gmailauthorization::Actions ) ;
private:
	void cancel() ;
	void setCode( const QString& ) ;

        void hideUI() ;

        void enableAll() ;
        void disableAll() ;

        void closeEvent( QCloseEvent * ) ;

        Ui::gmailauthorization * m_ui ;

	gmailauthorization::getAuth& m_getAuthorizationCode ;
	gmailauthorization::Actions m_gmailAuthorization ;
	QTcpServer m_server ;
	bool m_firstConnection = true ;
};

#endif
