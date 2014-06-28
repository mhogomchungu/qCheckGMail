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

#ifndef TASK_H
#define TASK_H

#include <functional>

typedef std::function< void( void ) > function_t ;

class continuation
{
public:
	explicit continuation( function_t ) ;
	continuation& then( function_t function = [](){} ) ;
	void start( void ) ;
	void run( void ) ;
private:
	function_t m_function = [](){} ;
	function_t m_start ;
};

namespace Task
{
	/*
	 * This API runs two tasks,the first one will be run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 *
	 * Sample use case below
	 */
	continuation& run( function_t ) ;
}

#if 0

auto _a = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
}

auto _b = [](){
	/*
	 * task _b does what task _b does here.
	 *
	 * This function body will run on the original thread
	 */
}

Task::run( _a ).then( _b ).start() ;

#endif

#endif // TASK_H
