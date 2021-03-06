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
    bitboards(uint32_t my_low, uint32_t my_high, uint32_t opp_low, uint32_t opp_high);
    bitboards(uint64_t _my_bits, uint64_t _opp_bits);
    bitboards(const BitBoard &_my_bits, const BitBoard &_opp_bits);
    bitboards(const bitboard &_my_bits, const bitboard &_opp_bits);
    ~bitboards(void);

    bitboards &operator =(const bitboards &src);

    inline void init(uint32_t my_low, uint32_t my_high, uint32_t opp_low, uint32_t opp_high);
    inline void init(uint64_t _my_bits, uint64_t _opp_bits);
    inline void init(const BitBoard &_my_bits, const BitBoard &_opp_bits);
    inline void init(const bitboard &_my_bits, const bitboard &_opp_bits);

    inline void default     (int type = OPENNING_DISCS_EMPTY);

    inline void empty       (void);
    inline void clear       (void);

    inline void set         (const BitBoard &src_my_bits, const BitBoard &src_opp_bits);
    inline void not         (const BitBoard &src_bits);
    inline void and         (const BitBoard &src_bits);
    inline void or          (const BitBoard &src_bits);
    inline void xor         (const BitBoard &src_bits);
    inline void andnot      (const BitBoard &src_bits);

    inline void set         (const bitboard &src_my_bits, const bitboard &src_opp_bits);
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

inline void bitboards::init(uint32_t my_low, uint32_t my_high, uint32_t opp_low, uint32_t opp_high)
{
    my_bits.low     = my_low;
    my_bits.high    = my_high;
    opp_bits.low    = opp_low;
    opp_bits.high   = opp_high;
}

inline void bitboards::init(uint64_t _my_bits, uint64_t _opp_bits)
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

///////////////////////////////////////////////////////////////////////////

inline void bitboards::empty(void)
{
    my_bits.low     = 0;
    my_bits.high    = 0;
    opp_bits.low    = 0;
    opp_bits.high   = 0;
}

inline void bitboards::clear(void)
{
    my_bits.low     = 0;
    my_bits.high    = 0;
    opp_bits.low    = 0;
    opp_bits.high   = 0;
}

///////////////////////////////////////////////////////////////////////////

inline void bitboards::set(const BitBoard &src_my_bits, const BitBoard &src_opp_bits)
{
    my_bits.low   = src_my_bits.low;
    my_bits.high  = src_my_bits.high;

    opp_bits.low  = src_opp_bits.low;
    opp_bits.high = src_opp_bits.high;
}

inline void bitboards::set(const bitboard &src_my_bits, const bitboard &src_opp_bits)
{
    my_bits.low   = src_my_bits.low;
    my_bits.high  = src_my_bits.high;

    opp_bits.low  = src_opp_bits.low;
    opp_bits.high = src_opp_bits.high;
}

inline void bitboards::not(const bitboard &src_bits)
{
    my_bits.low  = ~(src_bits.low);
    my_bits.high = ~(src_bits.high);
}

inline void bitboards::and(const bitboard &src_bits)
{
    my_bits.low  &= src_bits.low;
    my_bits.high &= src_bits.high;
}

inline void bitboards::or(const bitboard &src_bits)
{
    my_bits.low  |= src_bits.low;
    my_bits.high |= src_bits.high;
}

inline void bitboards::xor(const bitboard &src_bits)
{
    my_bits.low  ^= src_bits.low;
    my_bits.high ^= src_bits.high;
}

inline void bitboards::andnot(const bitboard &src_bits)
{
    my_bits.low  &= ~(src_bits.low);
    my_bits.high &= ~(src_bits.high);
}

inline void bitboards::reverse(void)
{
    uint32_t my_low, my_high;
    uint32_t opp_low, opp_high;

#if 1
    // swap odd and even bits
    my_low   = ((my_bits.low   >>  1) & 0x55555555UL) | ((my_bits.low   <<  1) & 0xAAAAAAAAUL);
    opp_low  = ((opp_bits.low  >>  1) & 0x55555555UL) | ((opp_bits.low  <<  1) & 0xAAAAAAAAUL);

    my_high  = ((my_bits.high  >>  1) & 0x55555555UL) | ((my_bits.high  <<  1) & 0xAAAAAAAAUL);
    opp_high = ((opp_bits.high >>  1) & 0x55555555UL) | ((opp_bits.high <<  1) & 0xAAAAAAAAUL);

    // swap consecutive pairs
    my_low   = ((my_low   >>  2) & 0x33333333UL) | ((my_low   <<  2) & 0xCCCCCCCCUL);
    opp_low  = ((opp_low  >>  2) & 0x33333333UL) | ((opp_low  <<  2) & 0xCCCCCCCCUL);

    my_high  = ((my_high  >>  2) & 0x33333333UL) | ((my_high  <<  2) & 0xCCCCCCCCUL);
    opp_high = ((opp_high >>  2) & 0x33333333UL) | ((opp_high <<  2) & 0xCCCCCCCCUL);

    // swap nibbles ...
    my_low   = ((my_low   >>  4) & 0x0F0F0F0FUL) | ((my_low   <<  4) & 0xF0F0F0F0UL);
    opp_low  = ((opp_low  >>  4) & 0x0F0F0F0FUL) | ((opp_low  <<  4) & 0xF0F0F0F0UL);

    my_high  = ((my_high  >>  4) & 0x0F0F0F0FUL) | ((my_high  <<  4) & 0xF0F0F0F0UL);
    opp_high = ((opp_high >>  4) & 0x0F0F0F0FUL) | ((opp_high <<  4) & 0xF0F0F0F0UL);

    // swap bytes
    my_low   = ((my_low   >>  8) & 0x00FF00FFUL) | ((my_low   <<  8) & 0xFF00FF00UL);
    opp_low  = ((opp_low  >>  8) & 0x00FF00FFUL) | ((opp_low  <<  8) & 0xFF00FF00UL);

    my_high  = ((my_high  >>  8) & 0x00FF00FFUL) | ((my_high  <<  8) & 0xFF00FF00UL);
    opp_high = ((opp_high >>  8) & 0x00FF00FFUL) | ((opp_high <<  8) & 0xFF00FF00UL);

    // swap 2-byte long pairs and swap low and high
    my_bits.high  = ((my_low   >> 16) & 0x0000FFFFUL) | ((my_low   << 16) & 0xFFFF0000UL);
    opp_bits.high = ((opp_low  >> 16) & 0x0000FFFFUL) | ((opp_low  << 16) & 0xFFFF0000UL);

    my_bits.low   = ((my_high  >> 16) & 0x0000FFFFUL) | ((my_high  << 16) & 0xFFFF0000UL);
    opp_bits.low  = ((opp_high >> 16) & 0x0000FFFFUL) | ((opp_high << 16) & 0xFFFF0000UL);

#else
    // swap odd and even bits
    my_low  = ((my_bits.low   >>  1) & 0x55555555UL) | ((my_bits.low   <<  1) & 0xAAAAAAAAUL);
    my_high = ((my_bits.high  >>  1) & 0x55555555UL) | ((my_bits.high  <<  1) & 0xAAAAAAAAUL);

    // swap consecutive pairs
    my_low  = ((my_low  >>  2) & 0x33333333UL) | ((my_low  <<  2) & 0xCCCCCCCCUL);
    my_high = ((my_high >>  2) & 0x33333333UL) | ((my_high <<  2) & 0xCCCCCCCCUL);

    // swap nibbles ...
    my_low  = ((my_low  >>  4) & 0x0F0F0F0FUL) | ((my_low  <<  4) & 0xF0F0F0F0UL);
    my_high = ((my_high >>  4) & 0x0F0F0F0FUL) | ((my_high <<  4) & 0xF0F0F0F0UL);

    // swap bytes
    my_low  = ((my_low  >>  8) & 0x00FF00FFUL) | ((my_low  <<  8) & 0xFF00FF00UL);
    my_high = ((my_high >>  8) & 0x00FF00FFUL) | ((my_high <<  8) & 0xFF00FF00UL);

    // swap 2-byte long pairs and swap low and high
    my_bits.high = ((my_low  >> 16) & 0x0000FFFFUL) | ((my_low  << 16) & 0xFFFF0000UL);
    my_bits.low  = ((my_high >> 16) & 0x0000FFFFUL) | ((my_high << 16) & 0xFFFF0000UL);

    ///////////////////////////////////////////////////////////////////////////

    // swap odd and even bits
    opp_low  = ((opp_bits.low   >>  1) & 0x55555555UL) | ((opp_bits.low   <<  1) & 0xAAAAAAAAUL);
    opp_high = ((opp_bits.high  >>  1) & 0x55555555UL) | ((opp_bits.high  <<  1) & 0xAAAAAAAAUL);

    // swap consecutive pairs
    opp_low  = ((opp_low  >>  2) & 0x33333333UL) | ((opp_low  <<  2) & 0xCCCCCCCCUL);
    opp_high = ((opp_high >>  2) & 0x33333333UL) | ((opp_high <<  2) & 0xCCCCCCCCUL);

    // swap nibbles ...
    opp_low  = ((opp_low  >>  4) & 0x0F0F0F0FUL) | ((opp_low  <<  4) & 0xF0F0F0F0UL);
    opp_high = ((opp_high >>  4) & 0x0F0F0F0FUL) | ((opp_high <<  4) & 0xF0F0F0F0UL);

    // swap bytes
    opp_low  = ((opp_low  >>  8) & 0x00FF00FFUL) | ((opp_low  <<  8) & 0xFF00FF00UL);
    opp_high = ((opp_high >>  8) & 0x00FF00FFUL) | ((opp_high <<  8) & 0xFF00FF00UL);

    // swap 2-byte long pairs and swap low and high
    opp_bits.high = ((opp_low  >> 16) & 0x0000FFFFUL) | ((opp_low  << 16) & 0xFFFF0000UL);
    opp_bits.low  = ((opp_high >> 16) & 0x0000FFFFUL) | ((opp_high << 16) & 0xFFFF0000UL);

#endif
}

}  // namespace dolphin

#endif  /* _DOL_BITBOARDS_H_ */
