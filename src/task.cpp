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

Task::Task( LxQt::Wallet::Wallet * wallet,const QString& accName,
	    const QString& accPassWord,const QString& accLabels,const QString& accDisplayName ):
	m_action( Task::addAccount ),m_wallet( wallet ),m_accName( accName ),m_accPassWord( accPassWord ),
	m_accLabels( accLabels ),m_accDisplayName( accDisplayName )
{
}

Task::Task( LxQt::Wallet::Wallet * wallet,QVector<accounts> * acc ) : m_wallet( wallet ),m_acc( acc )
{
}

Task::Task( LxQt::Wallet::Wallet * wallet,const QString& accName ) : m_wallet( wallet ),m_accName( accName )
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

void Task::run()
{
	QString labels_id  = m_accName + QString( LABEL_IDENTIFIER ) ;
	QString display_id = m_accName + QString( DISPLAY_NAME_IDENTIFIER ) ;

	auto _addKey = [&](){
		m_wallet->addKey( m_accName,m_accPassWord.toLatin1() ) ;
		m_wallet->addKey( labels_id,m_accLabels.toLatin1() ) ;
		m_wallet->addKey( display_id,m_accDisplayName.toLatin1() ) ;
	} ;

	auto _deleteKey = [&](){
		m_wallet->deleteKey( m_accName ) ;
		m_wallet->deleteKey( labels_id ) ;
		m_wallet->deleteKey( display_id ) ;
	} ;

	auto _getAccInfo = [&](){
		QVector<LxQt::Wallet::walletKeyValues> entries = m_wallet->readAllKeyValues() ;

		m_acc->clear() ;

		auto _getAccEntry = [&]( const QString& acc ){
			for( const auto& it : entries ){
				if( it.getKey() == acc ){
					return it.getValue() ;
				}
			}
			static QByteArray shouldNotGetHere ;
			return shouldNotGetHere ;
		} ;

		labels_id  = QString( LABEL_IDENTIFIER ) ;
		display_id = QString( DISPLAY_NAME_IDENTIFIER ) ;

		for( const auto& it : entries ){

			const QString& accName = it.getKey() ;
			bool r = accName.endsWith( labels_id ) || accName.endsWith( display_id ) ;

			if( r == false ){

				const QByteArray& passWord    = _getAccEntry( accName ) ;
				const QByteArray& labels      = _getAccEntry( accName + labels_id ) ;
				const QByteArray& displayName = _getAccEntry( accName + display_id ) ;

				m_acc->append( accounts( accName,passWord,displayName,labels ) ) ;
			}
		}
	} ;

	switch( m_action ){
	case Task::editAccount :

		_deleteKey() ;
		_addKey() ;

		break ;
	case Task::addAccount :

		_addKey() ;

		break ;
	case Task::deleteAccount :

		_deleteKey() ;

		break ;
	case Task::showAccountInfo :
	case Task::getAccountInfo  :

		_getAccInfo() ;

		break ;
	}
}
