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

bitboard::bitboard( void )
{
    initialize(0, 0);
}

bitboard::bitboard( uint32 _low, uint32 _high )
{
    initialize(_low, _high);
}

bitboard::bitboard( BitBoard & b )
{
    initialize(b.low, b.high);
}

bitboard::bitboard( uint64 _bits )
{
    initialize(_bits);
}

bitboard& bitboard::operator =( const bitboard & src )
{
    initialize(src.low, src.high);
    return *this;
}

bitboard::~bitboard( void )
{
}

inline void bitboard::initialize( uint32 _low, uint32 _high )
{
    low = _low; high = _high;
}

inline void bitboard::initialize( uint64 _bits )
{
    initialize((uint32)(_bits & 0xFFFFFFFFULL), (uint32)(_bits >> 32));
}

void bitboard::init( uint32 _low, uint32 _high )
{
    initialize(_low, _high);
}

void bitboard::init( uint64 _bits )
{
    initialize(_bits);
}

void bitboard::init( BitBoard & b )
{
    initialize(b.low, b.high);
}

///////////////////////////////////////////////////////////////

}  // namespace dolphin
