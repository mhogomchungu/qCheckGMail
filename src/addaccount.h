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

#ifndef ADDACCOUNT_H
#define ADDACCOUNT_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>

#include "accounts.h"
#include "gmailauthorization.h"
#include "util.hpp"
#include "../../NetworkAccessManager/network_access_manager.hpp"

namespace Ui {
class addaccount;
}

class addaccount : public QDialog
{
	Q_OBJECT
public:
	struct labels
	{
		QString emailAddress ;
		struct entry
		{
			QString name ;
			QString id ;
		} ;
		QVector< entry > entries ;
	} ;

	struct gmailAccountInfo
	{
		virtual void operator()( addaccount::labels )
		{
		}
		virtual ~gmailAccountInfo() ;
	} ;

	class GmailAccountInfo
	{
	public:
		template< typename Type,typename ... Args >
		GmailAccountInfo( Type,Args&& ... args ) :
			m_handle( std::make_shared< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void operator()( addaccount::labels e ) const
		{
			( *m_handle )( std::move( e ) ) ;
		}
	private:
		std::shared_ptr< addaccount::gmailAccountInfo > m_handle ;
	} ;

	struct gMailInfo
	{
		virtual void operator()( const QString& authoCode,addaccount::GmailAccountInfo )
		{
			Q_UNUSED( authoCode )
		}
		virtual void operator()( const QByteArray& accountName,addaccount::GmailAccountInfo )
		{
			Q_UNUSED( accountName )
		}
		virtual ~gMailInfo() ;
	} ;

	class GMailInfo
	{
	public:
		GMailInfo() : m_handle( std::make_unique< addaccount::gMailInfo >() )
		{
		}
		template< typename Type,typename ... Args >
		GMailInfo( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void operator()( const QString& authocode,addaccount::GmailAccountInfo result )
		{
			( *m_handle )( authocode,std::move( result ) ) ;
		}
		virtual void operator()( const QByteArray& accountName,addaccount::GmailAccountInfo result )
		{
			( *m_handle )( accountName,std::move( result ) ) ;
		}
	private:
		std::unique_ptr< addaccount::gMailInfo > m_handle ;
	} ;

	struct actions
	{
		virtual void cancel()
		{
		}
		virtual void results( accounts::entry&& )
		{
		}
		virtual void edit( accounts::entry )
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
		void results( accounts::entry&& e )
		{
			m_handle->results( std::move( e ) ) ;
		}
		void edit( accounts::entry e )
		{
			m_handle->edit( std::move( e ) ) ;
		}
	private:
		std::unique_ptr< addaccount::actions > m_handle ;
	} ;

	static addaccount& instance( QDialog * parent,
				     accounts::entry e,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions r,
				     addaccount::GMailInfo& n )
	{
		return *( new addaccount( parent,e,k,std::move( r ),n ) ) ;
	}

	static addaccount& instance( QDialog * parent,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions e,
				     addaccount::GMailInfo& n )
	{
		return *( new addaccount( parent,k,std::move( e ),n ) ) ;
	}

	addaccount( QDialog *,
		    accounts::entry,
		    gmailauthorization::getAuth&,
		    addaccount::Actions,
		    addaccount::GMailInfo& ) ;

	addaccount( QDialog *,
		    gmailauthorization::getAuth&,
		    addaccount::Actions,
		    addaccount::GMailInfo& ) ;

	~addaccount() ;
private:	
	void add( void ) ;
	void cancel( void ) ;
	void HideUI( void ) ;
	void Show( const QString& ) ;
	void getLabels( const QString& ) ;

	void closeEvent( QCloseEvent * ) ;
	Ui::addaccount * m_ui ;
	bool m_edit ;
	accounts::entry m_entry ;
	QString m_key ;
	gmailauthorization::getAuth& m_getAuthorization ;
	addaccount::Actions m_actions ;
	addaccount::GMailInfo& m_gmailAccountInfo ;
	addaccount::labels m_labels ;
};

#endif // ADDACCOUNT_H
