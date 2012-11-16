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

#ifndef _MY_RANDOM_H_
#define _MY_RANDOM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "dol_stddef.h"
#include <stdlib.h>
#include <stdio.h>

#define MY_RANDOM_SEED_DEFAULT  0

namespace dolphin {

class my_random : internal::no_copy
{
private:
    typedef int value_type;
    static const int timer_null_seed = MY_RANDOM_SEED_DEFAULT;

public:
    my_random( void ) { _my_srandom(MY_RANDOM_SEED_DEFAULT); };
    my_random( uint32_t x ) { _my_srandom(x); };

    inline static void srandom( int seed = timer_null_seed ) {
        _my_srandom(seed);
    };
    inline static void srand( unsigned int seed = timer_null_seed ) {
        _my_srand(seed);
    };

    inline static value_type get_range_number(value_type x, int range_min, int range_max) {
        if (range_min == range_max)
            return range_min;
        else if (range_min > range_max) {
            int temp = range_max;
            range_min = range_max;
            range_max = temp;
        }
        value_type _value = (range_min + (x % (range_max - range_min + 1)));
        if (_value < range_min)
            _value += (range_max - range_min + 1);
        return _value;
    }

    inline static value_type random( void ) {
        return _my_random();
    }
    inline static value_type random( int range_min, int range_max ) {
        value_type x = _my_random();
        return get_range_number(x, range_min, range_max);
    }

    inline static value_type rand( void ) {
#if defined(RAND_MAX) && (RAND_MAX == 0x7FFF)
        return (value_type)(((unsigned int)::rand() << 30) | (::rand() << 15) | ::rand());
#elif defined(RAND_MAX)
        return (value_type)(((unsigned int)::rand() << 16) | ::rand());
#else
        return (value_type)::rand();
#endif
    }
    inline static value_type rand( int range_min, int range_max ) {
        value_type x = my_random::rand();
        //printf("value_type x = my_random::rand();\nx = %d.\n", x);
        return get_range_number(x, range_min, range_max);
    }

protected:

    static void _my_srandom( int seed = timer_null_seed );
    static value_type _my_random( void );

    static char *_my_initstate( unsigned seed, char *arg_state, int n );
    static char *_my_setstate( char *arg_state );

    static void _my_srand( unsigned int seed = timer_null_seed );
};

}  // namespace dolphin

#endif  /* _MY_RANDOM_H_ */
