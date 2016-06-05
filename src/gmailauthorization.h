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

#include "gmailauthorization.h"

#include <functional>

namespace Ui {
class gmailauthorization;
}

class gmailauthorization : public QDialog
{
        Q_OBJECT
public:
        static void instance( QDialog * parent,
                              std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) >& r,
                              std::function< void() >&& e,
                              std::function< void( const QByteArray& ) >&& f )
        {
                new gmailauthorization( parent,r,std::move( e ),std::move( f ) ) ;
        }

        gmailauthorization( QDialog * parent,
                            std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) >&,
                            std::function< void() >&&,
                            std::function< void( const QByteArray& ) >&& ) ;
private slots:
        void cancel() ;
        void setCode() ;
private:
        void hideUI() ;

        void enableAll() ;
        void disableAll() ;

        void closeEvent( QCloseEvent * ) ;

        Ui::gmailauthorization * m_ui ;

        std::function< void( const QByteArray&,std::function< void( const QByteArray& ) > ) >& m_getAuthorizationCode ;
        std::function< void() > m_cancel ;
        std::function< void( const QByteArray& ) > m_getAuthorization ;
};

#endif
