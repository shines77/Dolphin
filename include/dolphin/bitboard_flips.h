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

#ifndef _DOL_BITBOARD_FLIPS_H_
#define _DOL_BITBOARD_FLIPS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <dolphin/colour.h>
#include <dolphin/board.h>
#include <dolphin/bitboard.h>

#define WRAPPED_INTO_NAMESPACE      1

#define BB_FLIP_FUNC                static int __FASTCALL(2)

/* int bitboard_getflips_xx(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits); */

#if defined(WRAPPED_INTO_NAMESPACE) && (WRAPPED_INTO_NAMESPACE != 0)

namespace dolphin {

typedef int (__FASTCALL(2) * const bitboard_getflips_func_t)(const bitboard &, const bitboard &, bitboard &);

extern bitboard_getflips_func_t bitboard_getflips[64];

extern void init_flip_mask(void);

}  // namespace dolphin

#else

using namespace dolphin;

#ifdef __cplusplus
extern "C" {
#endif

typedef int (__FASTCALL(2) * const bitboard_getflips_func_t)(const bitboard &, const bitboard &, bitboard &)

extern bitboard_getflips_func_t bitboard_getflips[64];

extern void init_flip_mask(void);

#ifdef __cplusplus
}
#endif

#endif  /* !WRAPPED_INTO_NAMESPACE */

#endif  /* _DOL_BITBOARD_FLIPS_H_ */
