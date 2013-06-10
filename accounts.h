/*
 *
 *  Copyright (c) 2013
 *  name : mhogo mchungu
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

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QString>
#include <QStringList>

class accounts
{
public:
	accounts( QString userName = QString(),QString password = QString(),QString displayName = QString(),QStringList list = QStringList() );
	accounts( const accounts& acc ) ;
	accounts& operator= ( const accounts& acc ) ;

	const QString& userName( void )  const ;
	const QString& passWord( void )  const ;
	const QString& defaultLabelUrl( void )  const ;
	const QString& displayName( void )  const ;
	const QStringList& LabelUrls( void ) const;
private:
	QString m_userName ;
	QString m_passWord ;
	QString m_displayName ;
	QStringList m_labels ;
};

#endif // ACCOUNTS_H
