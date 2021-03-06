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
#include <functional>

namespace Ui {
class addaccount;
}

class addaccount : public QDialog
{
	Q_OBJECT
public:

        static addaccount& instance( QDialog * parent,
                                     const accounts::entry& e,
				     gmailauthorization::function_t& k,
                                     std::function< void() >&& r,
                                     std::function< void( accounts::entry&& e ) >&& f )
        {
                return *( new addaccount( parent,e,k,std::move( r ),std::move( f ) ) ) ;
        }

        static addaccount& instance( QDialog * parent,
				     gmailauthorization::function_t& k,
                                     std::function< void() >&& e,
                                     std::function< void( accounts::entry&& e ) >&& f )
        {
                return *( new addaccount( parent,k,std::move( e ),std::move( f ) ) ) ;
        }

        addaccount( QDialog *,
                    const accounts::entry&,
		    gmailauthorization::function_t&,
                    std::function< void() >&&,
                    std::function< void( accounts::entry&& ) >&& ) ;

        addaccount( QDialog *,
		    gmailauthorization::function_t&,
                    std::function< void() >&&,
                    std::function< void( accounts::entry&& ) >&& ) ;

        ~addaccount();
private slots:
	void add( void ) ;
	void cancel( void ) ;
private:        
        void HideUI( void ) ;

	void closeEvent( QCloseEvent * ) ;
	Ui::addaccount * m_ui ;
        bool m_edit ;
        std::function< void( const QString&,std::function< void( const QString& ) > ) >& m_getAuthorization ;
        std::function< void() > m_cancel ;
        std::function< void( accounts::entry&& ) > m_result ;
};

#endif // ADDACCOUNT_H
