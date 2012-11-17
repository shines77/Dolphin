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

#ifndef _TASK_SCHEDULER_T_H_
#define _TASK_SCHEDULER_T_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "gmtl_stddef.h"
//#include "gmtl_machine.h"
#include "task.h"
#include "scheduler.h"

namespace gmtl {

class task_t;
class task_list_t;
class scheduler_t;

typedef std::size_t stack_size_type;

class task_scheduler_t : internal::no_copy
{
    scheduler_t *my_scheduler;

public:
    static const int MTL_AUTOMATIC = -1;
    static const int MTL_DELAY_ACTIVATION = -2;

    task_scheduler_t( int number_of_threads = MTL_AUTOMATIC, stack_size_type thread_stack_size = 0 );
    virtual ~task_scheduler_t( void );

    void initialize( int number_of_threads = MTL_AUTOMATIC );
    void initialize( int number_of_threads, stack_size_type thread_stack_size );
    void terminate( void );

    int get_num_threads( void ) const { return m_num_threads; }
    static int default_num_threads( void );

    bool is_active() const { return my_scheduler != NULL; }

private:
    int m_num_threads;
};

class GxString
{
private:
    char* buffer;
    unsigned int length;
public:
    GxString(void);
    virtual ~GxString(void);

    void Free(void);
    char* SetLength(int len, char ch = 0);

    GxString& operator =(GxString& str);
    int Length(void);
    const char* _Char(void);
};

}  // namespace gmtl

#endif /* _TASK_SCHEDULER_T_H_ */
