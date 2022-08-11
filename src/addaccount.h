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

#include <functional>

namespace Ui {
class addaccount;
}

class addaccount : public QDialog
{
	Q_OBJECT
public:
	struct actions
	{
		virtual void cancel()
		{
		}
		virtual void results( accounts::entry&& )
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
	private:
		std::unique_ptr< addaccount::actions > m_handle ;
	} ;

        static addaccount& instance( QDialog * parent,
                                     const accounts::entry& e,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions r )
        {
		return *( new addaccount( parent,e,k,std::move( r ) ) ) ;
        }

        static addaccount& instance( QDialog * parent,
				     gmailauthorization::getAuth& k,
				     addaccount::Actions e )
        {
		return *( new addaccount( parent,k,std::move( e ) ) ) ;
        }

        addaccount( QDialog *,
                    const accounts::entry&,
		    gmailauthorization::getAuth&,
		    addaccount::Actions ) ;

        addaccount( QDialog *,
		    gmailauthorization::getAuth&,
		    addaccount::Actions ) ;

	~addaccount() ;
private slots:
	void add( void ) ;
	void cancel( void ) ;
private:        
        void HideUI( void ) ;

	void closeEvent( QCloseEvent * ) ;
	Ui::addaccount * m_ui ;
        bool m_edit ;
	gmailauthorization::getAuth& m_getAuthorization ;
	addaccount::Actions m_actions ;
};

#endif // ADDACCOUNT_H
