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

#include "task.h"

#include "lxqt_wallet/frontend/lxqt_wallet.h"

#define TASK( x ) m_action == x

Task::Task( lxqt::Wallet::Wallet * wallet,QString accName,QString accPassWord,QString accLabels,QString accDisplayName ):
	m_action( Task::addAccount ),m_wallet( wallet ),m_accName( accName ),m_accPassWord( accPassWord ),
	m_accLabels( accLabels ),m_accDisplayName( accDisplayName )
{
}

Task::Task( lxqt::Wallet::Wallet * wallet,QVector<accounts> * acc ) : m_wallet( wallet ),m_acc( acc )
{
}

Task::Task( lxqt::Wallet::Wallet * wallet,QString accName ) : m_wallet( wallet ),m_accName( accName )
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

	if( TASK( Task::editAccount ) ){

		m_wallet->deleteKey( m_accName ) ;
		m_wallet->deleteKey( labels_id ) ;
		m_wallet->deleteKey( display_id ) ;

		m_wallet->addKey( m_accName,m_accPassWord.toAscii() ) ;
		m_wallet->addKey( labels_id,m_accLabels.toAscii() ) ;
		m_wallet->addKey( display_id,m_accDisplayName.toAscii() ) ;

	}else if( TASK( Task::addAccount ) ){

		m_wallet->addKey( m_accName,m_accPassWord.toAscii() ) ;
		m_wallet->addKey( labels_id,m_accLabels.toAscii() ) ;
		m_wallet->addKey( display_id,m_accDisplayName.toAscii() ) ;

	}else if( TASK( Task::deleteAccount ) ){

		m_wallet->deleteKey( m_accName ) ;
		m_wallet->deleteKey( labels_id ) ;
		m_wallet->deleteKey( display_id ) ;

	}else if( TASK( Task::readAccountInfo ) || TASK( Task::getAccountInfo ) ){

		QStringList accountNames = m_wallet->readAllKeys() ;

		int j = accountNames.size() ;

		QString passWord ;
		QString labels ;
		QString displayName ;

		m_acc->clear() ;

		for( int i = 0 ; i < j ; i++ ){
			const QString& accName = accountNames.at( i ) ;
			if( accName.endsWith( labels_id ) || accName.endsWith( display_id ) ){
				;
			}else{
				passWord    = m_wallet->readValue( accName ) ;
				labels      = m_wallet->readValue( accName + labels_id ) ;
				displayName = m_wallet->readValue( accName + display_id ) ;

				m_acc->append( accounts( accName,passWord,displayName,labels ) ) ;
			}
		}
	}
}
