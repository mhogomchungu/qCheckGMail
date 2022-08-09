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
#include <functional>

namespace Ui {
class gmailauthorization;
}

class gmailauthorization : public QDialog
{
        Q_OBJECT
public:
	using function_0_t = std::function< void( const QString& ) > ;

	using function_t = std::function< void( const QString&,function_0_t ) > ;

        static void instance( QDialog * parent,
			      gmailauthorization::function_t& r,
                              std::function< void() >&& e,
			      function_0_t&& f )
        {
                new gmailauthorization( parent,r,std::move( e ),std::move( f ) ) ;
        }

        gmailauthorization( QDialog * parent,
			    function_t&,
                            std::function< void() >&&,
			    function_0_t&& ) ;
private:
	void cancel() ;
	void setCode( const QString& ) ;

        void hideUI() ;

        void enableAll() ;
        void disableAll() ;

        void closeEvent( QCloseEvent * ) ;

        Ui::gmailauthorization * m_ui ;

	gmailauthorization::function_t& m_getAuthorizationCode ;
        std::function< void() > m_cancel ;
	function_0_t m_getAuthorization ;
	QTcpServer m_server ;
	bool m_firstConnection = true ;
};

#endif
