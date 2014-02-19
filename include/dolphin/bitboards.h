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

#ifndef _DOL_BITBOARDS_H_
#define _DOL_BITBOARDS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <dolphin/bitboard.h>

namespace dolphin {

enum openning_discs {
    OPENNING_DISCS_EMPTY = 0,
    OPENNING_DISCS_1,
    OPENNING_DISCS_2,
    OPENNING_DISCS_TEST
};

/////////////////////////////////////////////////////////
// bitboard_t
/////////////////////////////////////////////////////////

class bitboards
{
public:
    bitboards(void);
    bitboards(uint32 my_low, uint32 my_high, uint32 opp_low, uint32 opp_high);
    bitboards(uint64 _my_bits, uint64 _opp_bits);
    bitboards(const BitBoard &_my_bits, const BitBoard &_opp_bits);
    bitboards(const bitboard &_my_bits, const bitboard &_opp_bits);
    ~bitboards(void);

    bitboards &operator =(const bitboards &src);

    inline void init(uint32 my_low, uint32 my_high, uint32 opp_low, uint32 opp_high);
    inline void init(uint64 _my_bits, uint64 _opp_bits);
    inline void init(const BitBoard &_my_bits, const BitBoard &_opp_bits);
    inline void init(const bitboard &_my_bits, const bitboard &_opp_bits);

    inline void default     (int type = OPENNING_DISCS_EMPTY);

    inline void empty       (void);
    inline void clear       (void);

    inline void set         (const BitBoard &src_bits);
    inline void not         (const BitBoard &src_bits);
    inline void and         (const BitBoard &src_bits);
    inline void or          (const BitBoard &src_bits);
    inline void xor         (const BitBoard &src_bits);
    inline void andnot      (const BitBoard &src_bits);

    inline void set         (const bitboard &src_bits);
    inline void not         (const bitboard &src_bits);
    inline void and         (const bitboard &src_bits);
    inline void or          (const bitboard &src_bits);
    inline void xor         (const bitboard &src_bits);
    inline void andnot      (const bitboard &src_bits);

    inline void reverse     (void);
    inline void mirror_h    (void);
    inline void mirror_v    (void);
    inline void rotate_rt   (void);
    inline void rotate_lt   (void);

    inline void mirror_diag_1(void);
    inline void mirror_diag_2(void);

    inline void rotate      (unsigned int dir);

    inline unsigned int popcount (void);

public:
    bitboard my_bits;
    bitboard opp_bits;
};

/////////////////////////////////////////////////////////
// inline routines
/////////////////////////////////////////////////////////

inline void bitboards::init(uint32 my_low, uint32 my_high, uint32 opp_low, uint32 opp_high)
{
    my_bits.low     = my_low;
    my_bits.high    = my_high;
    opp_bits.low    = opp_low;
    opp_bits.high   = opp_high;
}

inline void bitboards::init(uint64 _my_bits, uint64 _opp_bits)
{
    my_bits.bits    = _my_bits;
    opp_bits.bits   = _opp_bits;
}

inline void bitboards::init(const BitBoard &_my_bits, const BitBoard &_opp_bits)
{
    init(_my_bits.low, _my_bits.high, _opp_bits.low, _opp_bits.high);
}

inline void bitboards::init(const bitboard &_my_bits, const bitboard &_opp_bits)
{
    init(_my_bits.low, _my_bits.high, _opp_bits.low, _opp_bits.high);
}

}  // namespace dolphin

#endif  /* _DOL_BITBOARDS_H_ */
