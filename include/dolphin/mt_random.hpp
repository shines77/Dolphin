//************************************************************************
//  This is a slightly modified version of Equamen mersenne twister.
//
//  Copyright (C) 2009 Chipset
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//************************************************************************

// Original Coyright (c) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura
//
// Functions for MT19937, with initialization improved 2002/2/10.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// This is a faster version by taking Shawn Cokus's optimization,
// Matthe Bellew's simplification, Isaku Wada's real version.
// C++ version by Lyell Haynes (Equamen)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. The names of its contributors may not be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _MT_RANDOM_HPP_
#define _MT_RANDOM_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <stddef.h>
#include <stdlib.h>

//
// 引用自: 产生伪随机数常用的两种算法
// http://www.cppblog.com/Chipset/archive/2009/02/07/73177.html
//
// Mersenne Twister MT19937
//

#define MT_RANDOM_SEED_DEFAULT  19650218UL

class mt_random
{
private:
    typedef size_t value_type;
    static const int timer_null_seed = 0;
    static const int default_seed = MT_RANDOM_SEED_DEFAULT;

public:
    mt_random() : left(1), next(NULL) { init(); }

    explicit mt_random(value_type seed) : left(1), next(NULL) { init(seed); }

    mt_random(value_type *init_key, int key_length) : left(1), next(NULL)
    {
        int i = 1, j = 0;
        int k = N > key_length ? N : key_length;
        init();
        for (; k; --k) {
            state[i]  = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL)) + init_key[j] + j; // non linear
            state[i] &= 0xFFFFFFFFUL;   // for WORDSIZE > 32 machines
            ++i;
            ++j;
            if (i >= N) {
                state[0] = state[N - 1];
                i = 1;
            }
            if (j >= key_length)
                j = 0;
        }

        for (k = N - 1; k; --k) {
            state[i]  = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i; // non linear
            state[i] &= 0xFFFFFFFFUL;   // for WORDSIZE > 32 machines
            ++i;
            if (i >= N) {
                state[0] = state[N - 1];
                i = 1;
            }
        }

        state[0] = 0x80000000UL;        // MSB is 1; assuring non-zero initial array
    }

    inline void srand(value_type seed = timer_null_seed)
    {
        if (seed == timer_null_seed) {
            time_t timer;
            time(&timer);
            seed = (value_type)timer;
        }
        init(seed);
        next_state();
    }

    inline static int get_range_number(value_type x, int range_min, int range_max) {
        if (range_min == range_max)
            return range_min;
        else if (range_min > range_max) {
            int temp = range_max;
            range_min = range_max;
            range_max = temp;
        }
        int _value = (range_min + (x % (range_max - range_min + 1)));
        if (_value < range_min)
            _value += (range_max - range_min + 1);
        return _value;
    }

    inline value_type rand(void)
    {
        value_type y;
        if (0 == --left)
            next_state();
        if (next != NULL)
            y = *next++;
        else
            y = ::rand();
        // Tempering
        y ^= (y >> 11);
        y ^= (y <<  7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);
        return y;
    }

    inline int rand(int range_min, int range_max) {
        value_type x = rand();
        //printf("value_type x = mt_random::rand();\nx = %d.\n", x);
        return get_range_number(x, range_min, range_max);
    }

    inline double real() { return (double)((double)rand() / (double)0xFFFFFFFFUL); }

    // generates a random number on [0,1) with 53-bit resolution
    inline double res53()
    {
        value_type a = rand() >> 5, b = rand() >> 6;
        return (a * 67108864.0 + b) / 9007199254740992.0;
    }

private:
    void init(value_type seed = default_seed)
    {
        state[0] = seed & 0xFFFFFFFFUL;
        for (int j = 1; j < N; ++j) {
            state[j] = (1812433253UL * (state[j - 1] ^ (state[j - 1] >> 30)) + j);
            // See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
            // In the previous versions, MSBs of the seed affect
            // only MSBs of the array state[].
            // 2002/01/09 modified by Makoto Matsumoto
            state[j] &= 0xFFFFFFFFUL;  // for >32 bit machines
        }
    }

    inline void next_state()
    {
        value_type *p = state;
        int i;

        for (i = N - M + 1; --i; ++p)
            *p = (p[M] ^ twist(p[0], p[1]));

        for (i = M; --i; ++p)
            *p = (p[M - N] ^ twist(p[0], p[1]));

        *p = p[M - N] ^ twist(p[0], state[0]);
        left = N;
        next = state;
    }

    inline value_type mixbits(value_type u, value_type v) const
    {
        return (u & 0x80000000UL) | (v & 0x7FFFFFFFUL);
    }

    inline value_type twist(value_type u, value_type v) const
    {
        return ((mixbits(u, v)  >>  1) ^ (v & 1UL ? 2567483615UL : 0UL));
    }

    static const int N = 624, M = 397;
    value_type state[N];
    value_type left;
    value_type *next;
};

class mt_random_help
{
    static mt_random r;
public:
    mt_random_help() {}
    void operator()(size_t s) { r.srand(s); }
    size_t operator()() const { return r.rand(); }
    double operator()(double) { return r.real(); }
};

mt_random mt_random_help::r;

extern void mt_srand(unsigned int s) { mt_random_help()(s); }
extern size_t mt_irand()       { return mt_random_help()(); }
extern double mt_drand()    { return mt_random_help()(1.0); }

#endif  /* _MT_RANDOM_HPP_ */
