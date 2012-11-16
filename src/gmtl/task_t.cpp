/*
    Copyright 2005-2011 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#include <new>

// Do not include task.h directly. Use scheduler_common.h instead
//#include "scheduler_common.h"
//#include "governor.h"
//#include "scheduler.h"
//#include "itt_notify.h"

#include "../../include/gmtl/cache_aligned.h"
#include "../../include/gmtl/task.h"

namespace gmtl {

using namespace std;

namespace internal {
    class no_name {
        int i;
    };
}

///////////////////////////////////////////////////////////////
// task_base_t
///////////////////////////////////////////////////////////////

void task_base_t::spawn( task_t& t )
{

}

void task_base_t::spawn( task_list_t& list )
{

}

void task_base_t::spawn_root_and_wait( task_t& t )
{

}

void task_base_t::spawn_root_and_wait( task_list_t& t )
{

}

void task_base_t::spawn_and_wait_for( task_t& t )
{

}

void task_base_t::spawn_and_wait_for( task_list_t& list )
{

}

void task_base_t::wait_for_all( task_t& t )
{

}

///////////////////////////////////////////////////////////////
// task_t
///////////////////////////////////////////////////////////////

void task_t::spawn( void )
{
    int i = 0;
}

void task_t::spawn_root_and_wait( void )
{
    //
}

void task_t::spawn_and_wait_for( void )
{
    //
}

void task_t::wait_for_all( void )
{
    //
}

task_t* task_t::execute( void )
{
    return NULL;
}

///////////////////////////////////////////////////////////////

}  // namespace imtl
