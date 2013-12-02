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

#ifndef _GMTL_TASK_H_
#define _GMTL_TASK_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <gmtl/gmtl_stddef.h>
//#include <gmtl/gmtl_machine.h>
#include <gmtl/task_scheduler.h>
#include <gmtl/scheduler.h>
#include <climits>

namespace gmtl {

class task;
class task_list;
class scheduler;

class task_scheduler;

class task_base : internal::no_copy
{
public:
    friend class gmtl::task;
    friend class gmtl::task_list;

    static void spawn( task& t );
    static void spawn( task_list& list );

    static void spawn_root_and_wait( task& t );
    static void spawn_root_and_wait( task_list& t );

    static void spawn_and_wait_for( task& t );
    static void spawn_and_wait_for( task_list& list );

    static void wait_for_all( task& t );
    static void wait_for_all( task_list& list );

    virtual void spawn( void ) = 0;
    virtual void spawn_root_and_wait( void ) = 0;
    virtual void spawn_and_wait_for( void ) = 0;

    virtual void wait_for_all( void ) = 0;

    static void destroy( task& victim );

    //! Should be overridden by derived classes.
    virtual task* execute() = 0;

private:
    task_base* parent_base;
};

class task : public task_base
{
public:
    scheduler* m_origin;
    scheduler* m_owner;

    task* m_parent;
    task* m_next;

    int32_t m_depth;
    int32_t m_ref_count;

private:
    unsigned char m_state;
    unsigned char m_extra_state;

public:
    //! "next" field for list of task
    task* next;

public:
    //! Enumeration of task states that the scheduler considers.
    enum state_type {
        //! task is running, and will be destroyed after method execute() completes.
        executing,
        //! task to be rescheduled.
        reexecute,
        //! task is in ready pool, or is going to be put there, or was just taken off.
        ready,
        //! task object is freshly allocated or recycled.
        allocated,
        //! task object is on free list, or is going to be put there, or was just taken off.
        freed,
        //! task to be recycled as continuation
        recycle
    };

    //! Define recommended static forms via import from base class.
    using task_base::spawn;
    using task_base::spawn_root_and_wait;
    using task_base::spawn_and_wait_for;
    using task_base::wait_for_all;

    virtual void spawn( void );

    virtual void spawn_root_and_wait( void );
    virtual void spawn_and_wait_for( void );
    virtual void wait_for_all( void );

    virtual task* execute( void );

    void recycle_as_continuation( void );
    void recycle_as_safe_continuation( void );

    void recycle_as_child_of( task& new_parent );
    void recycle_to_reexecute( void );

    int32_t depth() const { return m_depth; }
    void set_depth( int32_t depth ) { m_depth = depth; }
    void add_to_depth( int32_t delta ) { m_depth += delta; }

    int32_t ref_count( void ) {
        return m_ref_count;
    }

    void set_ref_count( int count ) {
        m_ref_count = count;
    }

    int increment_ref_count() {
        m_ref_count++;
        return m_ref_count;
    }

    int decrement_ref_count() {
        m_ref_count--;
        return m_ref_count;
    }

    void set_parent(task* p) {
        m_parent = p;
    }

    bool is_stolen_task() const {
        return ((m_extra_state & 0x80) != 0);
    }

    state_type state() const { return state_type(m_state); }
};

class task_list : internal::no_copy
{
private:
    friend class task;
    task*     first;
    task**    nextptr;

public:
    task_list() : first(NULL), nextptr(&first) {}

    //! Destroys the list, but does not destroy the task objects.
    virtual ~task_list() {}

    //! True if list if empty; false otherwise.
    bool empty() const { return (first != NULL); }

    //! Push task onto back of list.
    void push_back( task& task ) {
        task.next = NULL;
        *nextptr  = &task;
        nextptr   = &task.next;
    }

    //! Pop the front task from the list.
    task& pop_front() {
        _DOL_ASSERT( !empty(), "attempt to pop item from empty task_list" );
        task* result = first;
        first = result->next;
        if ( first != NULL ) nextptr = &first;
        return *result;
    }

    //! Clear the list
    void clear() {
        first = NULL;
        nextptr = &first;
    }
};

}  // namespace gmtl

#endif /* _GMTL_TASK_H_ */
