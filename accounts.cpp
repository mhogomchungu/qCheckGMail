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

#include <QDebug>
#include "accounts.h"

accounts::accounts( QString userName,QString password,QStringList labels ) :
	m_userName( userName ),m_passWord( password )
{
	const char * baseLabel = "https://mail.google.com/mail/feed/atom/" ;

	m_labels.append( QString( baseLabel ) ) ;

	int j = labels.size() ;

	for( int i = 0 ; i < j ; i++ ){
		m_labels.append( QString( baseLabel ) + labels.at( i ) ) ;
	}
}

accounts::accounts( const accounts& acc )
{
	m_userName = acc.m_userName ;
	m_passWord = acc.m_passWord ;
	m_labels   = acc.m_labels   ;
}

accounts& accounts::operator=( const accounts& acc )
{
	m_userName = acc.m_userName ;
	m_passWord = acc.m_passWord ;
	m_labels   = acc.m_labels   ;
	return *this ;
}

const QString& accounts::userName() const
{
	return m_userName ;
}

const QString& accounts::passWord() const
{
	return m_passWord ;
}

const QString& accounts::defaultLabel() const
{
	return m_labels.at( 0 ) ;
}

const QStringList& accounts::LabelUrls() const
{
	return m_labels ;
}

