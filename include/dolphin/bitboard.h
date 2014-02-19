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

#ifndef _DOL_BITBOARD_H_
#define _DOL_BITBOARD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <dolphin/colour.h>
#include <dolphin/board.h>

#define BITBOARD_CLEAR(a) { \
    a.low  = 0; \
    a.high = 0; \
}

#define BITBOARD_NOT(a) { \
    a.low  = ~(a.low); \
    a.high = ~(a.high); \
}

#define BITBOARD_XOR(a, b) { \
    a.low  ^= b.low; \
    a.high ^= b.high; \
}

#define BITBOARD_OR(a, b) { \
    a.low  |= b.low; \
    a.high |= b.high; \
}

#define BITBOARD_AND(a, b) { \
    a.low  &= b.low; \
    a.high &= b.high; \
}

#define BITBOARD_ANDNOT(a, b) { \
    a.low  &= ~(b.low); \
    a.high &= ~(b.high); \
}

#define BITBOARD_FULL_XOR(a, b, c) { \
    a.low  = b.low  ^ c.low; \
    a.high = b.high ^ c.high; \
}

#define BITBOARD_FULL_OR(a, b, c) { \
    a.low  = b.low  | c.low; \
    a.high = b.high | c.high; \
}

#define BITBOARD_FULL_AND(a, b, c) { \
    a.low  = b.low  & c.low; \
    a.high = b.high & c.high; \
}

#define BITBOARD_FULL_ANDNOT(a, b, c) { \
    a.low  = b.low  & ~(c.low); \
    a.high = b.high & ~(c.high); \
}

#define BITBOARD_FULL_NOTOR(a, b, c) { \
    a.low  = ~(b.low  | c.low); \
    a.high = ~(b.high | c.high); \
}

namespace dolphin {

enum openning_pos {
    OPENNING_EMPTY = 0,
    OPENNING_POS_1_BLACK,
    OPENNING_POS_1_WHITE,
    OPENNING_POS_2_BLACK,
    OPENNING_POS_2_WHITE,
    OPENNING_POS_ALL,
    OPENNING_POS_TEST
};

enum rotate_dir {
    RT_SELF             = 0,
    RT_MIRROR_V         = 1,
    RT_ROTATE_RT        = 2,
    RT_REVERSE          = 3,
    RT_MIRROR_DIAG_1    = 4,
    RT_MIRROR_H         = 5,
    RT_ROTATE_LT        = 6,
    RT_MIRROR_DIAG_2    = 7,
    RT_MAX_DIR
};

typedef struct BitBoard {
	//uint32 low;
	//uint32 high;
    union {
        uint64 bits;
        struct {
	        uint32 low;
	        uint32 high;
        };
    };
} BitBoard_t, *PBitBoard;

typedef union ubitboard
{
    uint64 bits;
    struct {
	    uint32 low;
	    uint32 high;
    };
} ubitboard_t;

/////////////////////////////////////////////////////////
// bitboard_t
/////////////////////////////////////////////////////////

class bitboard : public BitBoard
{
public:
    bitboard(void);
    bitboard(uint32 _low, uint32 _high);
    bitboard(int32 _low);
    bitboard(uint32 _low);
    bitboard(uint64 _bits);
    bitboard(const BitBoard &b);
    ~bitboard(void);

    bitboard &operator =(const bitboard &src);

    inline void init(uint32 _low, uint32 _high);
    inline void init(uint64 _bits);
    inline void init(const BitBoard &b);
    inline void init(const bitboard &b);

    inline void default     (int type = OPENNING_EMPTY);

    inline void empty       (void);
    inline void clear       (void);

    inline void set         (unsigned int pos);
    inline void not         (unsigned int pos);
    inline void and         (unsigned int pos);
    inline void or          (unsigned int pos);
    inline void xor         (unsigned int pos);
    inline void andnot      (unsigned int pos);

    inline void set         (const BitBoard *src_bits);
    inline void not         (const BitBoard *src_bits);
    inline void and         (const BitBoard *src_bits);
    inline void or          (const BitBoard *src_bits);
    inline void xor         (const BitBoard *src_bits);
    inline void andnot      (const BitBoard *src_bits);

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
    static BitBoard square_mask[64];

    static inline unsigned int reverse32(unsigned int val);

    static inline unsigned int popcount(const BitBoard &b);
    static inline unsigned int REGPARM(2) non_iterative_popcount(unsigned int n1, unsigned int n2);
    static inline unsigned int iterative_popcount(BitBoard b);

    static inline void init_bitboard(void);
    static inline void init_square_mask(void);

    static inline unsigned int make_pos(unsigned int x, unsigned int y);

    static inline void set_bitboard(int *board, int color,
        BitBoard *my_out, BitBoard *opp_out);

    static inline void set_bitboard_10x10(int *board, int color,
        BitBoard *my_out, BitBoard *opp_out);

    static inline void set_board(int *board, int color,
        BitBoard my_bits, BitBoard opp_bits);

    static inline void set_board_10x10(int *board, int color,
        BitBoard my_bits, BitBoard opp_bits);

protected:
    //
};

/////////////////////////////////////////////////////////
// inline routines
/////////////////////////////////////////////////////////

inline void bitboard::init(uint32 _low, uint32 _high)
{
    low  = _low;
    high = _high;
}

inline void bitboard::init(uint64 _bits)
{
    init((uint32)(_bits & 0xFFFFFFFFULL), (uint32)(_bits >> 32));
}

inline void bitboard::init(const BitBoard &b)
{
    init(b.low, b.high);
}

inline void bitboard::init(const bitboard &b)
{
    init(b.low, b.high);
}

///////////////////////////////////////////////////////////////////////////

inline void bitboard::default(int type /* =OPENNING_EMPTY */)
{
    if (type == OPENNING_POS_1_BLACK || type == OPENNING_POS_2_WHITE) {
        // (3, 3), (4, 4)
        low  = 1UL << 27;
        high = 1UL << 4;
    }
    else if (type == OPENNING_POS_1_WHITE || type == OPENNING_POS_2_BLACK) {
        // (4, 3), (3, 4)
        low  = 1UL << 28;
        high = 1UL << 3;
    }
    else if (type == OPENNING_POS_ALL) {
        // (3, 3), (4, 4), (4, 3), (3, 4)
        low  = 3UL << 27;
        high = 3UL << 3;
    }
    else if (type == OPENNING_POS_TEST) {
        low  = 0x55555555UL;
        high = 0x33333333UL;
    }
    /* type == OPENNING_EMPTY */
    else {
        low  = 0;
        high = 0;
    }
}

inline void bitboard::empty(void)
{
    low  = 0;
    high = 0;
}

inline void bitboard::clear(void)
{
    low  = 0;
    high = 0;
}

inline void bitboard::set(unsigned int pos)
{
    if (pos < 32) {
        low  = (1 << pos);
        high = 0;
    }
    else {
        low  = 0;
        high = (1 << (pos - 32));
    }
}

inline void bitboard::not(unsigned int pos)
{
    if (pos < 32) {
        low  = ~(unsigned int)(1UL << pos);
        high = ~(0UL);
    }
    else {
        low  = ~(0UL);
        high = ~(unsigned int)(1UL << (pos - 32));
    }
}

inline void bitboard::and(unsigned int pos)
{
    if (pos < 32) {
        low  &= (1UL << pos);
        high  = 0UL;
    }
    else {
        low   = 0UL;
        high &= (1UL << (pos - 32));
    }
}

inline void bitboard::or(unsigned int pos)
{
    if (pos < 32)
        low  |= (1UL << pos);
    else
        high |= (1UL << (pos - 32));
}

inline void bitboard::xor(unsigned int pos)
{
    if (pos < 32) {
        low  ^= (1UL << pos);
        high ^= 0UL;
    }
    else {
        low  ^= 0UL;
        high ^= (1UL << (pos - 32));
    }
}

inline void bitboard::andnot(unsigned int pos)
{
    if (pos < 32) {
        low  &= ~(unsigned int)(1UL << pos);
        high &= ~(0UL);
    }
    else {
        low  &= ~(0UL);
        high &= ~(unsigned int)(1UL << (pos - 32));
    }
}

///////////////////////////////////////////////////////////////////////////

inline void bitboard::set(const BitBoard *src_bits)
{
    low  = src_bits->low;
    high = src_bits->high;
}

inline void bitboard::not(const BitBoard *src_bits)
{
    low  = ~(src_bits->low);
    high = ~(src_bits->high);
}

inline void bitboard::and(const BitBoard *src_bits)
{
    low  &= src_bits->low;
    high &= src_bits->high;
}

inline void bitboard::or(const BitBoard *src_bits)
{
    low  |= src_bits->low;
    high |= src_bits->high;
}

inline void bitboard::xor(const BitBoard *src_bits)
{
    low  ^= src_bits->low;
    high ^= src_bits->high;
}

inline void bitboard::andnot(const BitBoard *src_bits)
{
    low  &= ~(src_bits->low);
    high &= ~(src_bits->high);
}

///////////////////////////////////////////////////////////////////////////

inline void bitboard::set(const bitboard &src_bits)
{
    low  = src_bits.low;
    high = src_bits.high;
}

inline void bitboard::not(const bitboard &src_bits)
{
    low  = ~(src_bits.low);
    high = ~(src_bits.high);
}

inline void bitboard::and(const bitboard &src_bits)
{
    low  &= src_bits.low;
    high &= src_bits.high;
}

inline void bitboard::or(const bitboard &src_bits)
{
    low  |= src_bits.low;
    high |= src_bits.high;
}

inline void bitboard::xor(const bitboard &src_bits)
{
    low  ^= src_bits.low;
    high ^= src_bits.high;
}

inline void bitboard::andnot(const bitboard &src_bits)
{
    low  &= ~(src_bits.low);
    high &= ~(src_bits.high);
}

/**************************************************************************
    Bit Twiddling Hacks

    From: http://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    Reverse an N-bit quantity in parallel in 5 * lg(N) operations:

    unsigned int v; // 32-bit word to reverse bit order

    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ... 
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    // swap 2-byte long pairs
    v = ( v >> 16             ) | ( v               << 16);

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    // The original bits

    01 02 03 04 05 06 07 08
    09 10 11 12 13 14 15 16
    17 18 19 20 21 22 23 24
    25 26 27 28 29 30 31 32

    // swap odd and even bits

    02 01 04 03 06 05 08 07
    10 09 12 11 14 13 16 15
    18 17 20 19 22 21 24 23
    26 25 28 27 30 29 32 31

    // swap consecutive pairs

    04 03 02 01 08 07 06 05
    12 11 10 09 16 15 14 13
    20 19 18 17 24 23 22 21
    28 27 26 25 32 31 30 29

    // swap nibbles ...

    08 07 06 05 04 03 02 01
    16 15 14 13 12 11 10 09
    24 23 22 21 20 19 18 17
    32 31 30 29 28 27 26 25

    // swap bytes

    16 15 14 13 12 11 10 09
    08 07 06 05 04 03 02 01
    32 31 30 29 28 27 26 25
    24 23 22 21 20 19 18 17

    // swap 2-byte long pairs

    32 31 30 29 28 27 26 25
    24 23 22 21 20 19 18 17
    16 15 14 13 12 11 10 09
    08 07 06 05 04 03 02 01

**************************************************************************/

inline void bitboard::reverse(void)
{
    uint32 _low, _high;
    // swap odd and even bits
    _low  = ((low   >>  1) & 0x55555555UL) | ((low   <<  1) & 0xAAAAAAAAUL);
    _high = ((high  >>  1) & 0x55555555UL) | ((high  <<  1) & 0xAAAAAAAAUL);

    // swap consecutive pairs
    _low  = ((_low  >>  2) & 0x33333333UL) | ((_low  <<  2) & 0xCCCCCCCCUL);
    _high = ((_high >>  2) & 0x33333333UL) | ((_high <<  2) & 0xCCCCCCCCUL);

    // swap nibbles ...
    _low  = ((_low  >>  4) & 0x0F0F0F0FUL) | ((_low  <<  4) & 0xF0F0F0F0UL);
    _high = ((_high >>  4) & 0x0F0F0F0FUL) | ((_high <<  4) & 0xF0F0F0F0UL);

    // swap bytes
    _low  = ((_low  >>  8) & 0x00FF00FFUL) | ((_low  <<  8) & 0xFF00FF00UL);
    _high = ((_high >>  8) & 0x00FF00FFUL) | ((_high <<  8) & 0xFF00FF00UL);

    // swap 2-byte long pairs and swap low and high
    high  = ((_low  >> 16) & 0x0000FFFFUL) | ((_low  << 16) & 0xFFFF0000UL);
    low   = ((_high >> 16) & 0x0000FFFFUL) | ((_high << 16) & 0xFFFF0000UL);
}

/**************************************************************************

    Horizontal mirror an N-bit quantity

**************************************************************************/

inline void bitboard::mirror_h(void)
{
    uint32 _low, _high;
    _low  = low;
    _high = high;
#if 1
    low   = ((_high & 0xFF000000UL) >> 24);
    high  = ((_low  & 0xFF000000UL) >> 24);

    low  |= ((_high & 0x00FF0000UL) >>  8);
    high |= ((_low  & 0x00FF0000UL) >>  8);

    low  |= ((_high & 0x0000FF00UL) <<  8);
    high |= ((_low  & 0x0000FF00UL) <<  8);

    low  |= ((_high & 0x000000FFUL) << 24);
    high |= ((_low  & 0x000000FFUL) << 24);
#else
    low  = ((_high & 0xFF000000UL) >> 24) | ((_high & 0x00FF0000UL) >>  8)
         | ((_high & 0x0000FF00UL) <<  8) | ((_high & 0x000000FFUL) << 24);
    high = ((_low  & 0xFF000000UL) >> 24) | ((_low  & 0x00FF0000UL) >>  8)
         | ((_low  & 0x0000FF00UL) <<  8) | ((_low  & 0x000000FFUL) << 24);
#endif
}

/**************************************************************************

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    Vertical mirror an N-bit quantity in parallel in 3 * lg(N) operations:

    unsigned int v; // 32-bit word to vertical mirror bit order

    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ... 
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    // The original bits

    01 02 03 04 05 06 07 08
    09 10 11 12 13 14 15 16
    17 18 19 20 21 22 23 24
    25 26 27 28 29 30 31 32

    // swap odd and even bits

    02 01 04 03 06 05 08 07
    10 09 12 11 14 13 16 15
    18 17 20 19 22 21 24 23
    26 25 28 27 30 29 32 31

    // swap consecutive pairs

    04 03 02 01 08 07 06 05
    12 11 10 09 16 15 14 13
    20 19 18 17 24 23 22 21
    28 27 26 25 32 31 30 29

    // swap nibbles ...

    08 07 06 05 04 03 02 01
    16 15 14 13 12 11 10 09
    24 23 22 21 20 19 18 17
    32 31 30 29 28 27 26 25

**************************************************************************/

inline void bitboard::mirror_v(void)
{
    uint32 _low, _high;
    // swap odd and even bits
    _low  = ((low   >>  1) & 0x55555555UL) | ((low   <<  1) & 0xAAAAAAAAUL);
    _high = ((high  >>  1) & 0x55555555UL) | ((high  <<  1) & 0xAAAAAAAAUL);

    // swap consecutive pairs
    _low  = ((_low  >>  2) & 0x33333333UL) | ((_low  <<  2) & 0xCCCCCCCCUL);
    _high = ((_high >>  2) & 0x33333333UL) | ((_high <<  2) & 0xCCCCCCCCUL);

    // swap nibbles ...
    low   = ((_low  >>  4) & 0x0F0F0F0FUL) | ((_low  <<  4) & 0xF0F0F0F0UL);
    high  = ((_high >>  4) & 0x0F0F0F0FUL) | ((_high <<  4) & 0xF0F0F0F0UL);
}

/**************************************************************************

    rotate_rt() equivalent to mirror_diag_2() and mirror_v()

**************************************************************************/

inline void bitboard::rotate_rt(void)
{
    mirror_diag_2();
    mirror_v();
}

inline void bitboard::rotate_lt(void)
{
    uint32 _low, _high;

    // A1-A8 rotate to A8-H8
    _high  = ((((low & 0x01010101UL) | ((high & 0x01010101UL) << 4)) * 0x00204081UL) & 0x1FE00000UL) << 3;
    // B1-B8 rotate to A7-H7
    _high |= ((((low & 0x02020202UL) | ((high & 0x02020202UL) << 4)) * 0x00204081UL) & 0x3FC00000UL) >> 6;
    // C1-C8 rotate to A6-H6
    _high |= ((((low & 0x04040404UL) | ((high & 0x04040404UL) << 4)) * 0x00204081UL) & 0x7F800000UL) >> 15;
    // D1-D8 rotate to A5-H5
    _high |= ((((low & 0x08080808UL) | ((high & 0x08080808UL) << 4)) * 0x00204081UL) & 0xFF000000UL) >> 24;

    // E1-E8 rotate to A4-H4
    _low   = (((((low & 0x10101010UL) >> 4) | (high & 0x10101010UL)) * 0x00204081UL) & 0x1FE00000UL) << 3;
    // F1-F8 rotate to A3-H3
    _low  |= (((((low & 0x20202020UL) >> 4) | (high & 0x20202020UL)) * 0x00204081UL) & 0x3FC00000UL) >> 6;
    // G1-G8 rotate to A2-H2
    _low  |= (((((low & 0x40404040UL) >> 4) | (high & 0x40404040UL)) * 0x00204081UL) & 0x7F800000UL) >> 15;
    // H1-H8 rotate to A1-H1
    _low  |= (((((low & 0x80808080UL) >> 4) | (high & 0x80808080UL)) * 0x00204081UL) & 0xFF000000UL) >> 24;

    low  = _low;
    high = _high;
}

/**************************************************************************

    mirror_diag_1() equivalent to rotate_lt() and mirror_v()

**************************************************************************/

inline void bitboard::mirror_diag_1(void)
{
    rotate_lt();
    mirror_v();
}

inline void bitboard::mirror_diag_2(void)
{
    uint32 _low, _high;

    // A1-A8 transform to A1-H1
    _low   = ((((low & 0x01010101UL) | ((high & 0x01010101UL) << 4)) * 0x00204081UL) & 0x1FE00000UL) >> 21;
    // B1-B8 transform to A2-H2
    _low  |= ((((low & 0x02020202UL) | ((high & 0x02020202UL) << 4)) * 0x00204081UL) & 0x3FC00000UL) >> 14;
    // C1-C8 transform to A3-H3
    _low  |= ((((low & 0x04040404UL) | ((high & 0x04040404UL) << 4)) * 0x00204081UL) & 0x7F800000UL) >> 7;
    // D1-D8 transform to A4-H4
    _low  |= ((((low & 0x08080808UL) | ((high & 0x08080808UL) << 4)) * 0x00204081UL) & 0xFF000000UL);

    // E1-E8 transform to A5-H5
    _high  = (((((low & 0x10101010UL) >> 4) | (high & 0x10101010UL)) * 0x00204081UL) & 0x1FE00000UL) >> 21;
    // F1-F8 transform to A6-H6
    _high |= (((((low & 0x20202020UL) >> 4) | (high & 0x20202020UL)) * 0x00204081UL) & 0x3FC00000UL) >> 14;
    // G1-G8 transform to A7-H7
    _high |= (((((low & 0x40404040UL) >> 4) | (high & 0x40404040UL)) * 0x00204081UL) & 0x7F800000UL) >> 7;
    // H1-H8 transform to A8-H8
    _high |= (((((low & 0x80808080UL) >> 4) | (high & 0x80808080UL)) * 0x00204081UL) & 0xFF000000UL);

    low  = _low;
    high = _high;
}

inline void bitboard::rotate(unsigned int dir)
{
    switch (dir) {
        case RT_SELF:
            // don't need rotate
            break;
        case RT_MIRROR_V:
            mirror_v();
            break;
        case RT_ROTATE_RT:
            rotate_rt();
            break;
        case RT_REVERSE:
            reverse();
            break;
        case RT_MIRROR_DIAG_1:
            mirror_diag_1();
            break;
        case RT_MIRROR_H:
            mirror_h();
            break;
        case RT_ROTATE_LT:
            rotate_lt();
            break;
        case RT_MIRROR_DIAG_2:
            mirror_diag_2();
            break;
        default:
            break;
    }
}

inline unsigned int bitboard::popcount(void)
{
    const unsigned long m1 = 0x55555555UL;
    const unsigned long m2 = 0x33333333UL;
    unsigned int a, b, n1, n2;

    a  = high - ((high >> 1) & m1);
    n1 = (a & m2) + ((a >> 2) & m2);
    n1 = (n1 & 0x0F0F0F0FUL) + ((n1 >>  4) & 0x0F0F0F0FUL);
    n1 = (n1 & 0x0000FFFFUL) +  (n1 >> 16);
    n1 = (n1 & 0x000000FFUL) +  (n1 >>  8);

    b  = low - ((low >> 1) & m1);
    n2 = (b & m2) + ((b >> 2) & m2);
    n2 = (n2 & 0x0F0F0F0FUL) + ((n2 >>  4) & 0x0F0F0F0FUL);
    n2 = (n2 & 0x0000FFFFUL) +  (n2 >> 16);
    n2 = (n2 & 0x000000FFUL) +  (n2 >>  8);

    return n1 + n2;
}

/////////////////////////////////////////////////////////
// static inline routines
/////////////////////////////////////////////////////////

/*
  BIT_REVERSE_32
  Returns the bit-reverse of a 32-bit integer.
*/

inline unsigned int bitboard::reverse32(unsigned int bits)
{
    bits = ((bits >>  1) & 0x55555555UL) | ((bits <<  1) & 0xAAAAAAAAUL);
    bits = ((bits >>  2) & 0x33333333UL) | ((bits <<  2) & 0xCCCCCCCCUL);
    bits = ((bits >>  4) & 0x0F0F0F0FUL) | ((bits <<  4) & 0xF0F0F0F0UL);
    bits = ((bits >>  8) & 0x00FF00FFUL) | ((bits <<  8) & 0xFF00FF00UL);
    bits = ((bits >> 16) & 0x0000FFFFUL) | ((bits << 16) & 0xFFFF0000UL);

    return bits;
}

/*
  NON_ITERATIVE_POPCOUNT
  Counts the number of bits set in a 64-bit integer.
  This is done using some bitfiddling tricks.
*/

inline unsigned int bitboard::popcount(const BitBoard &b)
{
    const unsigned long m1 = 0x55555555UL;
    const unsigned long m2 = 0x33333333UL;
    unsigned int a, n1, n2;

    a  = b.high - ((b.high >> 1) & m1);
    n1 = (a & m2) + ((a >> 2) & m2);
    n1 = (n1 & 0x0F0F0F0FUL) + ((n1 >>  4) & 0x0F0F0F0FUL);
    n1 = (n1 & 0x0000FFFFUL) +  (n1 >> 16);
    n1 = (n1 & 0x000000FFUL) +  (n1 >>  8);

    a  = b.low - ((b.low >> 1) & m1);
    n2 = (a & m2) + ((a >> 2) & m2);
    n2 = (n2 & 0x0F0F0F0FUL) + ((n2 >>  4) & 0x0F0F0F0FUL);
    n2 = (n2 & 0x0000FFFFUL) +  (n2 >> 16);
    n2 = (n2 & 0x000000FFUL) +  (n2 >>  8);

    return n1 + n2;
}

/*
  NON_ITERATIVE_POPCOUNT2
  Counts the number of bits set in a 64-bit integer.
  This is done using some bitfiddling tricks.
*/

inline unsigned int REGPARM(2) bitboard::non_iterative_popcount(unsigned int n1, unsigned int n2)
{
    n1 = n1 - ((n1 >> 1) & 0x55555555UL);
    n2 = n2 - ((n2 >> 1) & 0x55555555UL);
    n1 = (n1 & 0x33333333UL) + ((n1 >> 2) & 0x33333333UL);
    n2 = (n2 & 0x33333333UL) + ((n2 >> 2) & 0x33333333UL);
    n1 = (n1 + (n1 >> 4)) & 0x0F0F0F0FUL;
    n2 = (n2 + (n2 >> 4)) & 0x0F0F0F0FUL;
    return ((n1 + n2) * 0x01010101UL) >> 24;
}

/*
  ITERATIVE_POPCOUNT
  Counts the number of bits set in a 64-bit integer.
  This is done using an iterative procedure which loops
  a number of times equal to the number of bits set,
  hence this function is fast when the number of bits
  set is low.
*/

inline unsigned int bitboard::iterative_popcount(BitBoard b)
{
    unsigned int n;
    n = 0;
    for (; b.high != 0; n++, b.high &= (b.high - 1))
        ;
    for (; b.low != 0; n++, b.low &= (b.low - 1))
        ;
    return n;
}

inline unsigned int bitboard::make_pos(unsigned int x, unsigned int y)
{
    /* pos = y * 8 + x */
    return (y << 3) + x;
}

inline void bitboard::init_bitboard(void)
{
    bitboard::init_square_mask();
}

inline void bitboard::init_square_mask(void)
{
#if 1
    int pos;
    for (pos = 0; pos < 64; pos++) {
        if (pos < 32) {
            square_mask[pos].low  = 1UL << pos;
            square_mask[pos].high = 0;
        }
        else {
            square_mask[pos].low  = 0;
            square_mask[pos].high = 1UL << (pos - 32);
        }
    }
#else
    int pos;
    unsigned long mask;

    mask = 1;
    for (pos = 0; pos < 32; pos++, mask <<= 1) {
        square_mask[pos].low  = mask;
        square_mask[pos].high = 0;
    }

    mask = 1;
    for (pos = 32; pos < 64; pos++, mask <<= 1) {
        square_mask[pos].low  = 0;
        square_mask[pos].high = mask;
    }
#endif
}

/*
  SET_BITBOARD
  Converts the vector board representation to the bitboard representation.
*/

inline void bitboard::set_bitboard(int *board, int color, BitBoard *my_out, BitBoard *opp_out)
{
    int i, j;
    int pos;
    unsigned long mask;
    int opp_color = OPP_COLOR(color);
    BitBoard my_bits, opp_bits;

    my_bits.high  = 0;
    my_bits.low   = 0;
    opp_bits.high = 0;
    opp_bits.low  = 0;

    mask = 1;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++, mask <<= 1) {
            pos = 8 * i + j;
            if (board[pos] == color) {
                my_bits.low |= mask;
            }
            else if (board[pos] == opp_color) {
                opp_bits.low |= mask;
            }
        }
    }

    mask = 1;
    for (i = 4; i < 8; i++) {
        for (j = 0; j < 8; j++, mask <<= 1) {
            pos = 8 * i + j;
            if (board[pos] == color) {
                my_bits.high |= mask;
            }
            else if (board[pos] == opp_color) {
                opp_bits.high |= mask;
            }
        }
    }

    *my_out  = my_bits;
    *opp_out = opp_bits;
}

/*
  SET_BITBOARD_10x10
  Converts the vector board representation to the bitboard representation.
*/

inline void bitboard::set_bitboard_10x10(int *board, int color, BitBoard *my_out, BitBoard *opp_out)
{
    int i, j;
    int pos;
    unsigned long mask;
    int opp_color = OPP_COLOR(color);
    BitBoard my_bits, opp_bits;

    my_bits.high  = 0;
    my_bits.low   = 0;
    opp_bits.high = 0;
    opp_bits.low  = 0;

    mask = 1;
    for (i = 1; i <= 4; i++) {
        for (j = 1; j <= 8; j++, mask <<= 1) {
            pos = 10 * i + j;
            if (board[pos] == color) {
                my_bits.low |= mask;
            }
            else if (board[pos] == opp_color) {
                opp_bits.low |= mask;
            }
        }
    }

    mask = 1;
    for (i = 5; i <= 8; i++) {
        for (j = 1; j <= 8; j++, mask <<= 1) {
            pos = 10 * i + j;
            if (board[pos] == color) {
                my_bits.high |= mask;
            }
            else if (board[pos] == opp_color) {
                opp_bits.high |= mask;
            }
        }
    }

    *my_out  = my_bits;
    *opp_out = opp_bits;
}

/*
  SET_BOARD_8x8
  Converts the bitboard representation to the board representation.
*/

inline void bitboard::set_board(int *board, int color, BitBoard my_bits, BitBoard opp_bits)
{
    int i, j;
    int pos;
    int opp_color = OPP_COLOR(color);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            pos = 8 * i + j;
            if ((square_mask[pos].low & my_bits.low)
                | (square_mask[pos].high & my_bits.high)) {
                    board[pos] = color;
            }
            else if ((square_mask[pos].low & opp_bits.low)
                | (square_mask[pos].high & opp_bits.high)) {
                    board[pos] = opp_color;
            }
            else {
                board[pos] = CHESS_EMPTY;
            }
        }
    }
}

/*
  SET_BOARD_10x10
  Converts the bitboard representation to the board representation.
*/

inline void bitboard::set_board_10x10(int *board, int color, BitBoard my_bits, BitBoard opp_bits)
{
    int i, j;
    int pos;
    int opp_color = OPP_COLOR(color);

    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
            pos = 10 * i + j;
            if ((square_mask[pos].low & my_bits.low)
                | (square_mask[pos].high & my_bits.high)) {
                    board[pos] = color;
            }
            else if ((square_mask[pos].low & opp_bits.low)
                | (square_mask[pos].high & opp_bits.high)) {
                    board[pos] = opp_color;
            }
            else {
                board[pos] = CHESS_EMPTY;
            }
        }
    }
}

}  // namespace dolphin

#endif  /* _DOL_BITBOARD_H_ */
