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
#include <QMenu>

#include "accounts.h"
#include "gmailauthorization.h"
#include "util.hpp"
#include "settings.h"
#include "utils/network_access_manager.hpp"
#include "logwindow.h"

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
		virtual void operator()( const QString& )
		{
		}
		virtual ~gmailAccountInfo() ;
	} ;

	class GmailAccountInfo
	{
	public:
		template< typename Type,typename ... Args >
		GmailAccountInfo( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void operator()( addaccount::labels e ) const
		{
			( *m_handle )( std::move( e ) ) ;
		}
		void operator()( const QString& e ) const
		{
			( *m_handle )( e ) ;
		}
	private:
		std::unique_ptr< addaccount::gmailAccountInfo > m_handle ;
	} ;

	struct gMailInfo
	{
		virtual void withoutToken( const QString&,addaccount::GmailAccountInfo )
		{
		}
		virtual void withToken( const QString&,addaccount::GmailAccountInfo )
		{
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
		void withoutToken( const QString& authocode,addaccount::GmailAccountInfo result )
		{
			m_handle->withoutToken( authocode,std::move( result ) ) ;
		}
		virtual void withToken( const QString& accountName,addaccount::GmailAccountInfo result )
		{
			m_handle->withToken( accountName,std::move( result ) ) ;
		}
	private:
		std::unique_ptr< addaccount::gMailInfo > m_handle ;
	} ;

	struct actions
	{
		virtual void setLabels( addaccount::labels&& )
		{
		}
		virtual void cancel()
		{
		}
		virtual void results( accounts::entry&& )
		{
		}
		virtual void edit( const QString&,const QString& )
		{
		}
		virtual const addaccount::labels& labels() = 0 ;
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
		void edit( const QString& accName,const QString& labels )
		{
			m_handle->edit( accName,labels ) ;
		}
		const addaccount::labels& labels()
		{
			return m_handle->labels() ;
		}
		void setLabels( addaccount::labels&& s )
		{
			m_handle->setLabels( std::move( s ) ) ;
		}
	private:
		std::unique_ptr< addaccount::actions > m_handle ;
	} ;

	static addaccount& instance( QDialog * parent,
				     settings& s,
				     logWindow& l,
				     const accounts::entry& e,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions r,
				     addaccount::GMailInfo& n )
	{
		return *( new addaccount( parent,l,s,e,k,std::move( r ),n ) ) ;
	}

	static addaccount& instance( QDialog * parent,
				     settings& s,
				     logWindow& l,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions e,
				     addaccount::GMailInfo& n )
	{
		return *( new addaccount( parent,l,s,k,std::move( e ),n ) ) ;
	}

	addaccount( QDialog *,
		    logWindow&,
		    settings&,
		    const accounts::entry&,
		    gmailauthorization::getAuth&,
		    addaccount::Actions,
		    addaccount::GMailInfo& ) ;

	addaccount( QDialog *,
		    logWindow&,
		    settings&,
		    gmailauthorization::getAuth&,
		    addaccount::Actions,
		    addaccount::GMailInfo& ) ;

	~addaccount() override ;
private:	
	void setUpMenu() ;
	void add() ;
	void cancel() ;
	void HideUI() ;
	void Show( const QString& ) ;
	void getLabels( const QString& ) ;
	void showLabels( addaccount::labels&& ) ;
	void showError( const QString& ) ;
	void showLabelMenu( const addaccount::labels& ) ;
	void closeEvent( QCloseEvent * ) override ;
	Ui::addaccount * m_ui ;
	bool m_edit ;
	QString m_key ;
	gmailauthorization::getAuth& m_getAuthorization ;
	addaccount::Actions m_actions ;
	addaccount::GMailInfo& m_gmailAccountInfo ;
	logWindow& m_logWindow ;
	settings& m_setting ;
	QMenu m_menu ;
};

#endif // ADDACCOUNT_H
