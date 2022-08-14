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

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QString>
#include <QVector>

class accountLabel
{
public:
	accountLabel( const QString& accountLabel = QString(),int accountEmailCount = -1 ) ;

	const QString& labelUrl( void )     const  ;
	const QString& labelName( void )    const  ;
	const QString& lastModified( void ) const  ;

	int emailCount( void ) const  ;

	void setLastModifiedTime( const QString& ) ;
	void setEmailCount( int ) ;
private:
	int m_emailCount ;
	QString m_labelUrl ;
	QString m_labelName ;
	QString m_lastModifiedTime ;
};

class accounts
{
public:
	struct entry
	{
		QString accName ;
		QString accPassword ;
		QString accLabels ;
		QString accRefreshToken ;
	};

	accounts( const accounts::entry& ) ;
	accounts() ;

	const accounts::entry& data( void ) const ;
	const QString& refreshToken( void ) const ;
	const QString& accountName( void ) const ;
	const QString& passWord( void )	 const ;
	const QString& defaultLabelUrl( void ) const ;
	const QString& labels( void ) const ;
	const QString& labelUrlAt( int ) const ;
	const QString& accessToken( void ) const ;

	int   numberOfLabels( void ) const ;

	void setAccessToken( const QString& ) const ;

	accountLabel& getAccountLabel( int ) ;
private:
	mutable QString m_accessToken ;
	accounts::entry m_entry ;
	QVector< accountLabel > m_labelUrls ;
};

#endif // ACCOUNTS_H
