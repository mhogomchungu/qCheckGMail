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
	accountLabel()
	{
	}
	accountLabel( const QString& accountLabel,
		      const QString& accountLabelUi ) ;

	const QString& labelUrl() const  ;
	const QString& labelUiName() const  ;
private:
	QString m_labelUrl ;
	QString m_labelName ;
};

class accounts
{
public:
	struct entry
	{
		QString accName ;
		QString accLabels ;
		QString accRefreshToken ;
	} ;

	accounts( const accounts::entry& ) ;
	accounts() ;

	const accounts::entry& data() const ;

	const QString& refreshToken() const ;
	const QString& accountName() const ;
	const QString& defaultLabelUrl() const ;
	const QString& labels() const ;
	const QString& labelUrlAt( int ) const ;
	const QString& nameUiAt( int ) const ;
	const QString& accessToken() const ;

	int numberOfLabels() const ;

	void updateAccountInfo( const QString& accName,const QString& labels ) ;
	void setAccessToken( const QString& ) const ;
private:
	void updateLabels() ;
	mutable QString m_accessToken ;
	accounts::entry m_entry ;
	QVector< accountLabel > m_labelUrls ;
};

#endif // ACCOUNTS_H
