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

typedef struct BitBoard {
	unsigned int low;
	unsigned int high;
} bitboard_t, *PBitBoard;

/////////////////////////////////////////////////////////
// bitboard_t
/////////////////////////////////////////////////////////

class bitboard : public BitBoard
{
public:
    bitboard(void);
    bitboard(unsigned int _low, unsigned int _high);
    bitboard(uint64 _bits);
    bitboard(BitBoard &b);
    ~bitboard(void);

    bitboard &operator =(const bitboard &src);

    inline void init(uint32 _low, uint32 _high);
    inline void init(uint64 _bits);
    inline void init(BitBoard &b);

    inline void clear(void);

    inline void set      (unsigned int pos);
    inline void not      (unsigned int pos);
    inline void and      (unsigned int pos);
    inline void or       (unsigned int pos);
    inline void xor      (unsigned int pos);
    inline void andnot   (unsigned int pos);

    inline void set      (BitBoard *src);
    inline void not      (BitBoard *src);
    inline void and      (BitBoard *src);
    inline void or       (BitBoard *src);
    inline void xor      (BitBoard *src);
    inline void andnot   (BitBoard *src);

public:
    static BitBoard square_mask[64];

    static inline void clear(BitBoard *b);

    static inline void set      (BitBoard *b, unsigned int pos);
    static inline void not      (BitBoard *b, unsigned int pos);
    static inline void and      (BitBoard *b, unsigned int pos);
    static inline void or       (BitBoard *b, unsigned int pos);
    static inline void xor      (BitBoard *b, unsigned int pos);
    static inline void andnot   (BitBoard *b, unsigned int pos);

    static inline void set      (BitBoard *b, BitBoard *src);
    static inline void not      (BitBoard *b, BitBoard *src);
    static inline void and      (BitBoard *b, BitBoard *src);
    static inline void or       (BitBoard *b, BitBoard *src);
    static inline void xor      (BitBoard *b, BitBoard *src);
    static inline void andnot   (BitBoard *b, BitBoard *src);

    static inline unsigned int reverse32(unsigned int val);

    static inline unsigned int popcount(BitBoard b);
    static inline unsigned int REGPARM(2) non_iterative_popcount(unsigned int n1, unsigned int n2);
    static inline unsigned int iterative_popcount(BitBoard b);

    static inline void init_square_mask(void);

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

inline void bitboard::init(BitBoard &b)
{
    init(b.low, b.high);
}

///////////////////////////////////////////////////////////////////////////

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

inline void bitboard::set(BitBoard *src)
{
    low  = src->low;
    high = src->high;
}

inline void bitboard::not(BitBoard *src)
{
    low  = ~(src->low);
    high = ~(src->high);
}

inline void bitboard::and(BitBoard *src)
{
    low  &= src->low;
    high &= src->high;
}

inline void bitboard::or(BitBoard *src)
{
    low  |= src->low;
    high |= src->high;
}

inline void bitboard::xor(BitBoard *src)
{
    low  ^= src->low;
    high ^= src->high;
}

inline void bitboard::andnot(BitBoard *src)
{
    low  &= ~(src->low);
    high &= ~(src->high);
}

/////////////////////////////////////////////////////////
// static inline routines
/////////////////////////////////////////////////////////

inline void bitboard::clear(BitBoard *b)
{
    b->low  = 0;
    b->high = 0;
}

///////////////////////////////////////////////////////////////////////////

inline void bitboard::set(BitBoard *b, unsigned int pos)
{
    if (pos < 32) {
        b->low  = (1 << pos);
        b->high = 0;
    }
    else {
        b->low  = 0;
        b->high = (1 << (pos - 32));
    }
}

inline void bitboard::not(BitBoard *b, unsigned int pos)
{
    if (pos < 32) {
        b->low  = ~(unsigned int)(1UL << pos);
        b->high = ~(0UL);
    }
    else {
        b->low  = ~(0UL);
        b->high = ~(unsigned int)(1UL << (pos - 32));
    }
}

inline void bitboard::and(BitBoard *b, unsigned int pos)
{
    if (pos < 32) {
        b->low  &= (1UL << pos);
        b->high  = 0UL;
    }
    else {
        b->low   = 0UL;
        b->high &= (1UL << (pos - 32));
    }
}

inline void bitboard::or(BitBoard *b, unsigned int pos)
{
    if (pos < 32)
        b->low  |= (1UL << pos);
    else
        b->high |= (1UL << (pos - 32));
}

inline void bitboard::xor(BitBoard *b, unsigned int pos)
{
    if (pos < 32) {
        b->low  ^= (1UL << pos);
        b->high ^= 0UL;
    }
    else {
        b->low  ^= 0UL;
        b->high ^= (1UL << (pos - 32));
    }
}

inline void bitboard::andnot(BitBoard *b, unsigned int pos)
{
    if (pos < 32) {
        b->low  &= ~(unsigned int)(1UL << pos);
        b->high &= ~(0UL);
    }
    else {
        b->low  &= ~(0UL);
        b->high &= ~(unsigned int)(1UL << (pos - 32));
    }
}

///////////////////////////////////////////////////////////////////////////

inline void bitboard::set(BitBoard *b, BitBoard *src)
{
    b->low  = src->low;
    b->high = src->high;
}

inline void bitboard::not(BitBoard *b, BitBoard *src)
{
    b->low  = ~(src->low);
    b->high = ~(src->high);
}

inline void bitboard::and(BitBoard *b, BitBoard *src)
{
    b->low  &= src->low;
    b->high &= src->high;
}

inline void bitboard::or(BitBoard *b, BitBoard *src)
{
    b->low  |= src->low;
    b->high |= src->high;
}

inline void bitboard::xor(BitBoard *b, BitBoard *src)
{
    b->low  ^= src->low;
    b->high ^= src->high;
}

inline void bitboard::andnot(BitBoard *b, BitBoard *src)
{
    b->low  &= ~(src->low);
    b->high &= ~(src->high);
}

///////////////////////////////////////////////////////////////////////////

/*
  BIT_REVERSE_32
  Returns the bit-reverse of a 32-bit integer.
*/

inline unsigned int bitboard::reverse32(unsigned int val)
{
    val = ((val >>  1) & 0x55555555UL) | ((val <<  1) & 0xAAAAAAAAUL);
    val = ((val >>  2) & 0x33333333UL) | ((val <<  2) & 0xCCCCCCCCUL);
    val = ((val >>  4) & 0x0F0F0F0FUL) | ((val <<  4) & 0xF0F0F0F0UL);
    val = ((val >>  8) & 0x00FF00FFUL) | ((val <<  8) & 0xFF00FF00UL);
    val = ((val >> 16) & 0x0000FFFFUL) | ((val << 16) & 0xFFFF0000UL);

    return val;
}

/*
  NON_ITERATIVE_POPCOUNT
  Counts the number of bits set in a 64-bit integer.
  This is done using some bitfiddling tricks.
*/

inline unsigned int bitboard::popcount(BitBoard b)
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
