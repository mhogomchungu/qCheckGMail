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
#include "util.hpp"

#include <QStringList>

accounts::accounts()
{
}

accounts::accounts( const accounts::entry& e ) : m_entry( e )
{
	QString baseLabel = "https://gmail.googleapis.com/gmail/v1/users/me/labels/" ;

	m_labelUrls.append( { baseLabel + "INBOX","INBOX" } ) ;

	if( !m_entry.accLabels.isEmpty() ){

		auto m = util::idsAndNamesFromJson( m_entry.accLabels ) ;

		for( const auto& it : m ){

			m_labelUrls.append( accountLabel( baseLabel + it.id,it.name ) ) ;
		}
	}
}

const accounts::entry& accounts::data() const
{
	return m_entry ;
}

const QString& accounts::accountName() const
{
	return m_entry.accName ;
}

const QString& accounts::labels() const
{
	return m_entry.accLabels ;
}

const QString& accounts::defaultLabelUrl() const
{
	return m_labelUrls.first().labelUrl() ;
}

int accounts::numberOfLabels() const
{
	return m_labelUrls.size() ;
}

void accounts::setAccessToken( const QString& e ) const
{
	m_accessToken = e ;
}

const QString& accounts::refreshToken() const
{
	return m_entry.accRefreshToken ;
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

const QString& accounts::nameUiAt( int i ) const
{
	if( i < m_labelUrls.size() ){

		return m_labelUrls.at( i ).labelUiName() ;
	}else{
		static QString ShouldNotGetHere ;
		return ShouldNotGetHere ;
	}
}

const QString& accounts::accessToken() const
{
	return m_accessToken ;
}

accountLabel::accountLabel( const QString& labelUrl,
			    const QString& accountLabelUi ) :
	m_labelUrl( labelUrl ),m_labelName( accountLabelUi )
{
}

const QString& accountLabel::labelUrl() const
{
	return m_labelUrl ;
}

const QString& accountLabel::labelUiName() const
{
	return m_labelName ;
}
