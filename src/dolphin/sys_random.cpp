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

#include <dolphin/sys_random.h>

#include <stdlib.h>
#ifndef _WIN32_WCE
#include <time.h>
#endif

namespace dolphin {

/*
  Init c runtime lib's random number seed
*/

void sys_random::_sys_srand( unsigned int seed /* =timer_null_seed(0) */ )
{
    if (seed == timer_null_seed)
        ::srand((unsigned)time(NULL));
    else
        ::srand(seed);
}

/*
  Generates a random number use c runtime lib'
*/

sys_random::value_type sys_random::_sys_rand( void )
{
#if defined(RAND_MAX) && (RAND_MAX == 0x7FFF)
    return (value_type)(
          (((unsigned int)::rand() & RAND_MAX) << 30)
        | (((unsigned int)::rand() & RAND_MAX) << 15)
        |  ((unsigned int)::rand() & RAND_MAX)
        );
#elif defined(RAND_MAX) && (RAND_MAX >= 0xFFFF)
    return (value_type)(
          (((unsigned int)::rand() & RAND_MAX) << 16)
        |  ((unsigned int)::rand() & RAND_MAX)
        );
#else
    return (value_type)(
          (((unsigned int)::rand() & 0x00FF) << 24)
        | (((unsigned int)::rand() & 0x00FF) << 16)
        | (((unsigned int)::rand() & 0x00FF) << 8)
        |  ((unsigned int)::rand() & 0x00FF)
        );
#endif
}

}  // namespace dolphin
