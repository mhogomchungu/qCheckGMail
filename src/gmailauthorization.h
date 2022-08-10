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
#include <QtNetwork/QNetworkRequest>

#include <QJsonDocument>
#include <QJsonObject>

#include <functional>

#include "../../3rdParty/NetworkAccessManager/network_access_manager.hpp"

#include "configurationoptionsdialog.h"

#include <memory>

namespace Ui {
class gmailauthorization;
}

class gmailauthorization : public QDialog
{
        Q_OBJECT
public:
	class getAutho
	{
	public:
		getAutho()
		{
		}
		getAutho( NetworkAccessManager& nm,
			  QNetworkRequest& r,
			  const QString& id,
			  const QString& secret ) :
			m_networkManager( &nm ),
			m_networkRequest( &r ),
			m_clientID( &id ),
			m_clientSecret( &secret )
		{
		}
		template< typename Function >
		void operator()( const QString& authocode,Function function )
		{
			if( m_networkManager ){

				m_networkManager->post( -1,*m_networkRequest,[ & ](){

					auto s = configurationoptionsdialog::stringRunTimePortNumber() ;

					urlOpts opts ;

					opts.add( "client_id",*m_clientID ) ;
					opts.add( "client_secret",*m_clientSecret ) ;
					opts.add( "code",authocode ) ;
					opts.add( "grant_type","authorization_code" ) ;
					opts.add( "redirect_uri","http://127.0.0.1:" + s ) ;

					return opts.toUtf8() ;

				 }(),[ funct = std::move( function ) ]( QNetworkReply& e ){

					auto m = e.readAll() ;

					auto s = QJsonDocument::fromJson( m ).object() ;

					funct( s.value( "refresh_token" ).toString(),m ) ;
				 } ) ;
			}
		}
	private:
		NetworkAccessManager * m_networkManager = nullptr ;
		QNetworkRequest * m_networkRequest ;
		const QString * m_clientID ;
		const QString * m_clientSecret ;
	};

	template< typename T >
	struct type_identity{
		using type = T ;
	} ;

	class actions
	{
	public:
		virtual void cancel()
		{
		}
		virtual void getToken( const QString&,const QByteArray& )
		{
		}
		virtual ~actions() ;
	private:
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
			      gmailauthorization::getAutho& r,
			      gmailauthorization::Actions e )
        {
		new gmailauthorization( parent,r,std::move( e ) ) ;
        }

        gmailauthorization( QDialog * parent,
			    gmailauthorization::getAutho&,
			    gmailauthorization::Actions ) ;
private:
	void cancel() ;
	void setCode( const QString& ) ;

        void hideUI() ;

        void enableAll() ;
        void disableAll() ;

        void closeEvent( QCloseEvent * ) ;

        Ui::gmailauthorization * m_ui ;

	gmailauthorization::getAutho& m_getAuthorizationCode ;
	gmailauthorization::Actions m_gmailAuthorization ;
	QTcpServer m_server ;
	bool m_firstConnection = true ;
};

#endif
