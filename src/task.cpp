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

class thread : public QThread
{
public:
	thread( function_t function_0,function_t function_1 ) :
		m_function_0( function_0 ),m_function_1( function_1 )
	{
		connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
		this->start() ;
	}
	~thread()
	{
		m_function_1() ;
	}
private:
	void run( void )
	{
		m_function_0() ;
	}
	function_t m_function_0 ;
	function_t m_function_1 ;
};

namespace Task
{
	void exec( function_t function_0,function_t function_1 )
	{
		new thread( function_0,function_1 ) ;
	}
}
