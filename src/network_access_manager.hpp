/*
 * copyright: 2023
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef NetworkAccessManager_H
#define NetworkAccessManager_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QMutex>
#include <QTimer>

#include <memory>
#include <iostream>

class NetworkAccessManager
{
public:
	NetworkAccessManager( int timeOut ) : m_timeOut( timeOut )
	{
	}
	QNetworkAccessManager& QtNAM()
	{
		return m_manager ;
	}
	template< typename Reply >
	void get( const QNetworkRequest& r,Reply reply )
	{
		this->setupReply( m_manager.get( r ),std::move( reply ) ) ;
	}
	template< typename Reply,typename Data >
	void post( const QNetworkRequest& r,const Data& e,Reply reply )
	{
		this->setupReply( m_manager.post( r,e ),std::move( reply ) ) ;
	}
	template< typename Reply >
	void head( const QNetworkRequest& r,Reply reply )
	{
		this->setupReply( m_manager.head( r ),std::move( reply ) ) ;
	}
private:
	template< typename Reply >
	class handle
	{
	public:
		handle( Reply&& r ) : m_reply( std::move( r ) )
		{
		}
		void result( QNetworkReply * r,bool timeOut )
		{
			QObject::disconnect( m_networkConn ) ;
			QObject::disconnect( m_timerConn ) ;
			m_processed = true ;
			m_timer.stop() ;
			m_reply( *r,timeOut ) ;
		}
		QTimer * timer()
		{
			return &m_timer ;
		}
		QMutex * mutex()
		{
			return &m_mutex ;
		}
		bool notProcessed()
		{
			return !m_processed ;
		}
		void start( int timeOut,QMetaObject::Connection nc,QMetaObject::Connection tc )
		{
			m_networkConn = nc ;
			m_timerConn = tc ;
			m_timer.start( timeOut ) ;
		}
	private:
		bool m_processed = false ;

		QMutex m_mutex ;
		QTimer m_timer ;
		Reply m_reply ;
		QMetaObject::Connection m_networkConn ;
		QMetaObject::Connection m_timerConn ;
	} ;
	template< typename Reply >
	void setupReply( QNetworkReply * s,Reply&& reply )
	{
		m_replies.emplace_back( s ) ;

		auto hdl = std::make_shared< handle< Reply > >( std::move( reply ) ) ;

		auto nc = QObject::connect( s,&QNetworkReply::finished,[ s,hdl,this ](){

			if( hdl->notProcessed() ){

				hdl->result( s,false ) ;

				s->deleteLater() ;
			}
		} ) ;

		auto tc = QObject::connect( hdl->timer(),&QTimer::timeout,[ s,hdl,this ](){

			if( hdl->notProcessed() ){

				hdl->result( s,true ) ;

				s->abort() ;

				s->deleteLater() ;
			}
		} ) ;

		hdl->start( m_timeOut,nc,tc ) ;
	}
	QNetworkAccessManager m_manager ;
	int m_timeOut ;
	std::vector< QNetworkReply * > m_replies ;
} ;

#endif
