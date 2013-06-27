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

accounts::accounts( QString accountName,QString password,QString displayName,QString labels ) :
	m_accountName( accountName ),m_passWord( password ),m_displayName( displayName ),m_labels( labels )
{
	QString baseLabel ;

	int index = accountName.indexOf( QString( "@" ) ) ;
	if( index == -1 ){
		baseLabel = QString( "https://mail.google.com/mail/feed/atom/" ) ;
	}else{
		index++ ;
		QString domain = QString( accountName.mid( index ) ) ;
		baseLabel = QString( "https://mail.google.com/a/%1/feed/atom/" ).arg( domain ) ;
	}

	m_labelUrls.append( baseLabel ) ;

	if( m_labels.endsWith( "," ) ){
		m_labels.truncate( m_labels.size() - 1 );
	}

	if( m_labels.isEmpty() ){
		;
	}else{
		QStringList l = m_labels.split( "," ) ;
		int j = l.size() ;
		for( int i = 0 ; i < j ; i++ ){
			m_labelUrls.append( baseLabel + l.at( i ) ) ;
		}
	}
}

accounts::accounts( const accounts& acc )
{
	m_accountName = acc.m_accountName ;
	m_passWord    = acc.m_passWord    ;
	m_labels      = acc.m_labels      ;
	m_labelUrls   = acc.m_labelUrls   ;
	m_displayName = acc.m_displayName ;
}

accounts& accounts::operator=( const accounts& acc )
{
	m_accountName = acc.m_accountName ;
	m_passWord    = acc.m_passWord    ;
	m_labels      = acc.m_labels      ;
	m_labelUrls   = acc.m_labelUrls   ;
	m_displayName = acc.m_displayName ;
	return *this ;
}

const QString& accounts::accountName() const
{
	return m_accountName ;
}

const QString& accounts::passWord() const
{
	return m_passWord ;
}

const QString& accounts::displayName() const
{
	return m_displayName ;
}

const QString& accounts::labels() const
{
	return m_labels ;
}

const QString& accounts::defaultLabelUrl() const
{
	return m_labelUrls.at( 0 ) ;
}

const QStringList& accounts::labelUrls() const
{
	return m_labelUrls ;
}

int accounts::numberOfLabels() const
{
	return m_labelUrls.size() ;
}

const QString &accounts::labelUrlAt( int i ) const
{
	return m_labelUrls.at( i ) ;
}

