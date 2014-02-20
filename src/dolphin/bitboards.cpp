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

#include <dolphin/bitboards.h>

namespace dolphin {

namespace internal {
    class no_name4 {
        int i;
    };
}

///////////////////////////////////////////////////////////////
// bitbaords
///////////////////////////////////////////////////////////////

bitboards::bitboards(void)
{
    /* do nothing! */
}

bitboards::bitboards(uint32 my_low, uint32 my_high, uint32 opp_low, uint32 opp_high)
{
    init(my_low, my_high, opp_low, opp_high);
}

bitboards::bitboards(uint64 _my_bits, uint64 _opp_bits)
{
    init(_my_bits, _opp_bits);
}

bitboards::bitboards(const BitBoard &_my_bits, const BitBoard &_opp_bits)
{
    init(_my_bits, _opp_bits);
}

bitboards::bitboards(const bitboard &_my_bits, const bitboard &_opp_bits)
{
    init(_my_bits, _opp_bits);
}

bitboards &bitboards::operator =(const bitboards &src)
{
    my_bits.low     = src.my_bits.low;
    my_bits.high    = src.my_bits.high;
    opp_bits.low    = src.opp_bits.low;
    opp_bits.high   = src.opp_bits.high;
    return *this;
}

bitboards::~bitboards(void)
{
    /* do nothing! */
}

///////////////////////////////////////////////////////////////

}  // namespace dolphin
