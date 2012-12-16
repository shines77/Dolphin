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

#ifndef _ALIGNED_SPACE_T_H_
#define _ALIGNED_SPACE_T_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <gmtl/gmtl_stddef.h>
#include <gmtl/gmtl_machine.h>

//#define __GMTL_TypeWithAlignmentAtLeastAsStrict(T) T

namespace gmtl {

//! Block of space aligned sufficiently to construct an array T with N elements.
/** The elements are not constructed or destroyed by this class.
    @ingroup memory_allocation */
template<typename T, size_t N>
class aligned_space_t : internal::no_copy {
private:
    typedef __GMTL_TypeWithAlignmentAtLeastAsStrict(T) element_type;
    typedef std::size_t size_type;
    element_type array[(sizeof(T) * N + sizeof(element_type) - 1) / sizeof(element_type)];
    size_type m_size;

public:
    aligned_space_t( void ) : m_size(N) {};
    virtual ~aligned_space_t( void ) {};

public:
    // Get the array size
    size_type size( void ) { return m_size; }

    //! Pointer to beginning of array
    T* begin() { return internal::punned_cast<T*>(this); }

    //! Pointer to one past last element in array.
    T* end() { return begin() + N; }

    // Get the element by index
    T* elements( int _index ) {
        if (_index >= 0 && _index < (int)m_size) {
            T* temp = begin() + _index;
            return begin() + _index;
        }
        return NULL;
    }

    // Get the element by operator []
    T* operator[]( int _index ) {
        return elements(_index);
    }
};

}  // namespace gmtl

#endif  // _ALIGNED_SPACE_T_H_
