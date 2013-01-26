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
//#include <gmtl/scheduler.h>
//#include "itt_notify.h"

#include <gmtl/cache_aligned.h>
#include <gmtl/task_scheduler.h>

#include <string.h>

namespace gmtl {

using namespace std;

namespace internal {
    class no_name2 {
        int i;
    };
}

///////////////////////////////////////////////////////////////
// task_scheduler_t
///////////////////////////////////////////////////////////////

task_scheduler::task_scheduler( int number_of_threads /*= MTL_AUTOMATIC */,
                                   stack_size_type thread_stack_size /*= 0 */ )
    : my_scheduler(NULL)
{
    initialize( number_of_threads, thread_stack_size );
}

task_scheduler::~task_scheduler( void )
{
    if( my_scheduler )
        terminate();
}

void task_scheduler::initialize( int number_of_threads /*= MTL_AUTOMATIC */ )
{
    initialize( number_of_threads, 0 );
}

void task_scheduler::initialize( int number_of_threads, stack_size_type thread_stack_size )
{
    if (number_of_threads == MTL_AUTOMATIC) {
        m_num_threads = 1;  // get the default number of processor
    }
    else
        m_num_threads = number_of_threads;

    if (number_of_threads != MTL_DELAY_ACTIVATION) {
        //
    }
}

void task_scheduler::terminate( void )
{

}

int task_scheduler::default_num_threads( void )
{
    return 0;
}

GxString::GxString(void) : buffer(NULL), length(0)
{
}

GxString::~GxString(void)
{
    Free();
}

void GxString::Free( void )
{
    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }
    length = 0;
}

int GxString::Length(void)
{
    return (buffer != NULL) ? (int)strlen(buffer) : -1;
}

const char* GxString::_Char(void)
{
    return buffer;
}

GxString& GxString::operator =(GxString& str)
{
    if (this == &str)
        return *this;

    Free();

    size_t len = strlen(str._Char());
    buffer = new char[len + 1];
    if (buffer) {
        memset(buffer, 0, len + 1);
        memcpy(buffer, str._Char(), len);
        length = (unsigned int)len;
    }
    else
        length = 0;
    return *this;
}

char* GxString::SetLength( int len, char ch /* = 0 */ )
{
    Free();

    if (len >= 0) {
        buffer = new char[len + 1];
        if (buffer) {
            memset(buffer, ch, len + 1);
            buffer[len] = '\0';
            length = len;
        }
    }
    return buffer;
}

///////////////////////////////////////////////////////////////

}  // namespace gmtl
