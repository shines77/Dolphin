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

#define USE_NEW_SYS_SRAND       1

namespace dolphin {

void sys_random::_new_sys_srand( unsigned int seed /* =timer_null_seed(0) */ )
{
    // notice: prime[] can't empty!!
    const double prime[] = { 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.3, 1.5, 1.7, 2.1, 2.3 };
    const int prime_cnt = _countof(prime);
    int range_min = 2, range_max = 10;
    if (range_min < 0)
        range_min = 0;
    if (range_max <= range_min)
        range_max = range_min + 1;

    // 种随机种子(当前时间)
    if (seed == timer_null_seed)
        ::srand( (unsigned)time(NULL) );
    else
        ::srand(seed);

    int first = ::rand();
    printf("first rand() = %d\n", first);

    int n = (::rand() % (range_max - range_min)) + range_min;
    int m = 0;
    for (int i = 0; i < n; ++i)
        m = ::rand() % prime_cnt;

    // check prime[] is not empty?
    if (prime_cnt > 0) {
        double f = prime[m];
        printf("m = %d, f = %0.3g\n", m, f);

        // 再根据得到的随机系数种一次种子, 这样就比较随机了
        unsigned int new_seed = (unsigned int)((double)((unsigned)time(NULL)) * f);
        ::srand(new_seed);
    }

    first = ::rand();
    printf("new first rand() = %d\n\n", first);
}

/*
  Init c runtime lib's random number seed
*/

void sys_random::_sys_srand( unsigned int seed /* =timer_null_seed(0) */ )
{
#if defined(USE_NEW_SYS_SRAND) && (USE_NEW_SYS_SRAND != 0)
    return _new_sys_srand(seed);
#else
    if (seed == timer_null_seed)
        ::srand((unsigned)time(NULL));
    else
        ::srand(seed);
#endif
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
