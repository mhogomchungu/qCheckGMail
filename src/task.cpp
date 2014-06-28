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

#include <QThread>

#include "task.h"

continuation::continuation( function_t function ) : m_start( function )
{
}

void continuation::then( function_t function )
{
	m_function = function ;
	m_start() ;
}

void continuation::start()
{
	m_start() ;
}

void continuation::run()
{
	m_function() ;
}

class thread : public QThread
{
public:
	thread( function_t function ) :	m_function( function ),m_continuation( [&](){ this->start() ; } )
	{
		connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
	}
	continuation& taskContinuation( void )
	{
		return m_continuation ;
	}
	~thread()
	{
		m_continuation.run() ;
	}
private:
	void run( void )
	{
		m_function() ;
	}
	function_t m_function ;
	continuation m_continuation ;
};

namespace Task
{
	continuation& run( function_t function )
	{
		auto t = new thread( function ) ;
		return t->taskContinuation() ;
	}

	void exec( function_t function )
	{
		Task::run( function ).start() ;
	}
}
