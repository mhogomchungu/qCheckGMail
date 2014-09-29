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

#include <QDebug>
#include "accounts.h"
#include <QStringList>

accounts::accounts( const QString& accountName,const QString& password,const QString& displayName,const QString& labels ) :
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

	m_labelUrls.append( accountLabel( baseLabel ) ) ;

	if( m_labels.endsWith( "," ) ){
		m_labels.truncate( m_labels.size() - 1 ) ;
	}

	if( !m_labels.isEmpty() ){
		QStringList l = m_labels.split( "," ) ;
		for( const auto& it : l ){
			m_labelUrls.append( accountLabel( baseLabel + it ) ) ;
		}
	}
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
	return m_labelUrls.first().labelUrl() ;
}

int accounts::numberOfLabels() const
{
	return m_labelUrls.size() ;
}

accountLabel& accounts::getAccountLabel( int i )
{
	if( i < m_labelUrls.size() ){
		accountLabel * acc = m_labelUrls.data() + i ;
		return *acc ;
	}else{
		static accountLabel ShouldNotGetHere ;
		return ShouldNotGetHere ;
	}
}

const QString& accounts::labelUrlAt( int i ) const
{
	if( i < m_labelUrls.size() ){
		return m_labelUrls.at( i ).labelUrl() ;
	}else{
		static QString ShouldNotGetHere ;
		return ShouldNotGetHere ;
	}
}

accountLabel::accountLabel( const QString& labelUrl,int emailCount ) :
	m_emailCount( emailCount ),m_labelUrl( labelUrl )
{
	m_labelName = labelUrl.split( "/" ).last() ;
}

int accountLabel::emailCount() const
{
	return m_emailCount ;
}

const QString& accountLabel::labelUrl() const
{
	return m_labelUrl ;
}

const QString& accountLabel::labelName() const
{
	return m_labelName ;
}

void accountLabel::setEmailCount( int count )
{
	m_emailCount = count ;
}

const QString& accountLabel::lastModified() const
{
	return m_lastModifiedTime ;
}

void accountLabel::setLastModifiedTime( const QString& time )
{
	m_lastModifiedTime = time ;
}
