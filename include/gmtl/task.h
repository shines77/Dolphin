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

#ifndef _TASK_T_H_
#define _TASK_T_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <gmtl/gmtl_stddef.h>
//#include <gmtl/gmtl_machine.h>
#include <gmtl/task_scheduler.h>
#include <gmtl/scheduler.h>
#include <climits>

namespace gmtl {

class task_t;
class task_list_t;
class scheduler_t;

class task_scheduler_t;

class task_base_t : internal::no_copy
{
public:
    friend class gmtl::task_t;
    friend class gmtl::task_list_t;

    static void spawn( task_t& t );
    static void spawn( task_list_t& list );

    static void spawn_root_and_wait( task_t& t );
    static void spawn_root_and_wait( task_list_t& t );

    static void spawn_and_wait_for( task_t& t );
    static void spawn_and_wait_for( task_list_t& list );

    static void wait_for_all( task_t& t );
    static void wait_for_all( task_list_t& list );

    virtual void spawn( void ) = 0;
    virtual void spawn_root_and_wait( void ) = 0;
    virtual void spawn_and_wait_for( void ) = 0;

    virtual void wait_for_all( void ) = 0;

    static void destroy( task_t& victim );

    //! Should be overridden by derived classes.
    virtual task_t* execute() = 0;

private:
    task_base_t* parent_base;
};

class task_t : public task_base_t
{
public:
    scheduler_t* m_origin;
    scheduler_t* m_owner;

    task_t* m_parent;
    task_t* m_next;

    int32_t m_depth;
    int32_t m_ref_count;

private:
    unsigned char m_state;
    unsigned char m_extra_state;

public:
    //! "next" field for list of task
    task_t* next;

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
    using task_base_t::spawn;
    using task_base_t::spawn_root_and_wait;
    using task_base_t::spawn_and_wait_for;
    using task_base_t::wait_for_all;

    virtual void spawn( void );

    virtual void spawn_root_and_wait( void );
    virtual void spawn_and_wait_for( void );
    virtual void wait_for_all( void );

    virtual task_t* execute( void );

    void recycle_as_continuation( void );
    void recycle_as_safe_continuation( void );

    void recycle_as_child_of( task_t& new_parent );
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

    void set_parent(task_t* p) {
        m_parent = p;
    }

    bool is_stolen_task() const {
        return ((m_extra_state & 0x80) != 0);
    }

    state_type state() const { return state_type(m_state); }
};

class task_list_t : internal::no_copy
{
private:
    task_t*     first;
    task_t**    nextptr;
    friend class task_t;

public:
    task_list_t() : first(NULL), nextptr(&first) {}

    //! Destroys the list, but does not destroy the task objects.
    virtual ~task_list_t() {}

    //! True if list if empty; false otherwise.
    bool empty() const { return (first != NULL); }

    //! Push task onto back of list.
    void push_back( task_t& task ) {
        task.next = NULL;
        *nextptr  = &task;
        nextptr   = &task.next;
    }

    //! Pop the front task from the list.
    task_t& pop_front() {
        _DOL_ASSERT( !empty(), "attempt to pop item from empty task_list" );
        task_t* result = first;
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

#endif /* _TASK_T_H_ */
