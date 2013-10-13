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

#include <QThreadPool>
#include <QStringList>
#include <QDebug>

#include "task.h"

Task::Task( lxqt::Wallet::Wallet * wallet,const QString& accName,
	    const QString& accPassWord,const QString& accLabels,const QString& accDisplayName ):
	m_action( Task::addAccount ),m_wallet( wallet ),m_accName( accName ),m_accPassWord( accPassWord ),
	m_accLabels( accLabels ),m_accDisplayName( accDisplayName )
{
}

Task::Task( lxqt::Wallet::Wallet * wallet,QVector<accounts> * acc ) : m_wallet( wallet ),m_acc( acc )
{
}

Task::Task( lxqt::Wallet::Wallet * wallet,const QString& accName ) : m_wallet( wallet ),m_accName( accName )
{
}

Task::~Task()
{
	emit taskFinished( int( m_action ) ) ;
}

void Task::start( Task::action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

void Task::addKey( const QString& accName,const QString& accDisplayName,const QString& accLabels )
{
	m_wallet->addKey( accName,m_accPassWord.toAscii() ) ;
	m_wallet->addKey( accLabels,m_accLabels.toAscii() ) ;
	m_wallet->addKey( accDisplayName,m_accDisplayName.toAscii() ) ;
}

void Task::deleteKey( const QString& accName,const QString& accDisplayName,const QString& accLabels )
{
	m_wallet->deleteKey( accName ) ;
	m_wallet->deleteKey( accLabels ) ;
	m_wallet->deleteKey( accDisplayName ) ;
}

const QByteArray& Task::getAccInfo( const QVector<lxqt::Wallet::walletKeyValues>& entries,const QString& acc )
{
	int j = entries.size() ;

	for( int i = 0 ; i < j ; i++ ){
		if( entries.at( i ).getKey() == acc ){
			return entries.at( i ).getValue() ;
		}
	}

	return m_emptyEntry ;
}

void Task::run()
{
	QString labels_id  = m_accName + QString( LABEL_IDENTIFIER ) ;
	QString display_id = m_accName + QString( DISPLAY_NAME_IDENTIFIER ) ;

	switch( m_action ){
	case Task::editAccount :

		this->deleteKey( m_accName,display_id,labels_id ) ;
		this->addKey( m_accName,display_id,labels_id ) ;

		break ;
	case Task::addAccount :

		this->addKey( m_accName,display_id,labels_id ) ;

		break ;
	case Task::deleteAccount :

		this->deleteKey( m_accName,display_id,labels_id ) ;

		break ;
	case Task::showAccountInfo :
	case Task::getAccountInfo  :

		QVector<lxqt::Wallet::walletKeyValues> entries = m_wallet->readAllKeyValues() ;

		m_acc->clear() ;

		labels_id  = QString( LABEL_IDENTIFIER ) ;
		display_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

		int j = entries.size() ;

		for( int i = 0 ; i < j ; i++ ){
			const QString& accName = entries.at( i ).getKey() ;
			if( accName.endsWith( labels_id ) || accName.endsWith( display_id ) ){
				;
			}else{
				const QByteArray& passWord    = this->getAccInfo( entries,accName ) ;
				const QByteArray& labels      = this->getAccInfo( entries,accName + labels_id ) ;
				const QByteArray& displayName = this->getAccInfo( entries,accName + display_id ) ;

				m_acc->append( accounts( accName,passWord,displayName,labels ) ) ;
			}
		}
	}
}
