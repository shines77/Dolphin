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

#include <dolphin/bitboard.h>

namespace dolphin {

namespace internal {
    class no_name3 {
        int i;
    };
}

BitBoard bitboard::square_mask[64];

///////////////////////////////////////////////////////////////
// bitbaord
///////////////////////////////////////////////////////////////

bitboard::bitboard(void)
{
    init(0, 0);
}

bitboard::bitboard(uint32 _low, uint32 _high)
{
    init(_low, _high);
}

bitboard::bitboard(uint64 _bits)
{
    init(_bits);
}

bitboard::bitboard(const BitBoard &b)
{
    init(b.low, b.high);
}

bitboard::bitboard(int _low)
{
    init((unsigned int)_low, 0);
}

bitboard::bitboard(unsigned int _low)
{
    init(_low, 0);
}

bitboard &bitboard::operator =(const bitboard &src)
{
    low  = src.low;
    high = src.high;
    return *this;
}

bitboard::~bitboard(void)
{
}

///////////////////////////////////////////////////////////////

}  // namespace dolphin
