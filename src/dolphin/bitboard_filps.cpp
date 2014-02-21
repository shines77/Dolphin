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
#include <dolphin/bitboard_flips.h>

#if defined(WRAPPER_INTO_NAMESPACE_DOLPHIN) && (WRAPPER_INTO_NAMESPACE_DOLPHIN != 0)
namespace dolphin {
#endif

ALIGN_PREFIX(64) static unsigned int my_flip_mask[8][256] ALIGN_SUFFIX(64);
ALIGN_PREFIX(64) static unsigned int opp_flip_mask[8][64] ALIGN_SUFFIX(64);

void init_flip_mask(void)
{
    int i, j, k;
    unsigned int mask_bit, scan_bit, l_bit, r_bit;
    unsigned int opp_mask, mask, mask_l, mask_r;
    unsigned int mask_left[8], mask_right[8];
    int d_count, l_count, r_count;
    scan_bit = 1;
    mask_left[0]  = 0;
    mask_right[0] = 0xFE;
    for (i = 1; i < 8; i++) {
        mask_left[i]  = mask_left[i - 1] | scan_bit;
        mask_right[i] = (mask_right[i - 1] << 1) & 0xFF;
        scan_bit <<= 1;
    }

    scan_bit = 1;
    for (i = 0; i < 8; i++) {
        // 遍历opp_bits时, 只需要用到中间的6个bit, 最边上的各1个bit无用
        for (mask = 0; mask < 64; mask++) {
            // opp_mask范围为: (00000010B - 01111110B) (二进制), 即2~126
            opp_mask = mask << 1;
            if ((opp_mask & scan_bit) == (scan_bit + 0x80000000UL)) {
                opp_flip_mask[i][mask] = 0;
            }
            else {
                // left direction
                mask_l = opp_mask & mask_left[i];
                mask_bit = scan_bit >> 1;
                d_count = 0;
                for (j = i-1; j >= 0; j--) {
                    if ((mask_bit & mask_l) == 0)
                        break;
                    mask_bit >>= 1;
                    d_count++;
                }
                if (d_count == 0)
                    mask_l = 0;
                else
                    mask_l = mask_bit & mask_left[i];

                // right direction
                mask_r = opp_mask & mask_right[i];
                mask_bit = scan_bit << 1;
                d_count = 0;
                for (j = i+1; j < 8; j++) {
                    if ((mask_bit & mask_r) == 0)
                        break;
                    mask_bit <<= 1;
                    d_count++;
                }
                if (d_count == 0)
                    mask_r = 0;
                else
                    mask_r = mask_bit & mask_right[i];

                opp_flip_mask[i][mask] = mask_l | mask_r;
            }
        }
        scan_bit <<= 1;
    }

    for (i = 0; i < 8; i++)
        for (opp_mask = 0; opp_mask < 256; opp_mask++)
            my_flip_mask[i][opp_mask] = 0;

    scan_bit = 1;
    for (i = 0; i < 8; i++) {
        // left dir
        j = i - 1;
        l_count = 0;
        l_bit = scan_bit;
        do {
            l_bit = (l_bit >> 1) & mask_left[i];
            if (l_count == 0 || j < 0)
                mask_l = 0;
            else
                mask_l = (scan_bit - 1) - ((l_bit << 1) - 1);

            // right dir
            k = i + 1;
            r_count = 0;
            r_bit = scan_bit;
            do {
                r_bit = (r_bit << 1) & mask_right[i];
                if (r_count == 0 || k >= 8)
                    mask_r = 0;
                else
                    mask_r = (r_bit - 1) - scan_bit - (scan_bit - 1);

                mask = (mask_l & mask_left[i]) | (mask_r & mask_right[i]);

                // mask = r_bit - now_bit - (l_bit << 1);
                opp_mask = l_bit | r_bit;
                my_flip_mask[i][opp_mask] = mask;
                if (l_count == 0)
                    my_flip_mask[i][r_bit] = mask;
                if (r_count == 0)
                    my_flip_mask[i][l_bit] = mask;

                r_count++;
                k++;
            } while (k < 8);

            l_count++;
            j--;
        } while (j >= 0);

        scan_bit <<= 1;
    }
}

int bitboard_getflips_a1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_a8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_b8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_c8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_d8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_e8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_f8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_g8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int bitboard_getflips_h8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

int (*bitboard_getflips[64])(const bitboard &, const bitboard &, bitboard &) = {

    bitboard_getflips_a1,
    bitboard_getflips_b1,
    bitboard_getflips_c1,
    bitboard_getflips_d1,
    bitboard_getflips_e1,
    bitboard_getflips_f1,
    bitboard_getflips_g1,
    bitboard_getflips_h1,

    bitboard_getflips_a2,
    bitboard_getflips_b2,
    bitboard_getflips_c2,
    bitboard_getflips_d2,
    bitboard_getflips_e2,
    bitboard_getflips_f2,
    bitboard_getflips_g2,
    bitboard_getflips_h2,

    bitboard_getflips_a3,
    bitboard_getflips_b3,
    bitboard_getflips_c3,
    bitboard_getflips_d3,
    bitboard_getflips_e3,
    bitboard_getflips_f3,
    bitboard_getflips_g3,
    bitboard_getflips_h3,

    bitboard_getflips_a4,
    bitboard_getflips_b4,
    bitboard_getflips_c4,
    bitboard_getflips_d4,
    bitboard_getflips_e4,
    bitboard_getflips_f4,
    bitboard_getflips_g4,
    bitboard_getflips_h4,

    bitboard_getflips_a5,
    bitboard_getflips_b5,
    bitboard_getflips_c5,
    bitboard_getflips_d5,
    bitboard_getflips_e5,
    bitboard_getflips_f5,
    bitboard_getflips_g5,
    bitboard_getflips_h5,

    bitboard_getflips_a6,
    bitboard_getflips_b6,
    bitboard_getflips_c6,
    bitboard_getflips_d6,
    bitboard_getflips_e6,
    bitboard_getflips_f6,
    bitboard_getflips_g6,
    bitboard_getflips_h6,

    bitboard_getflips_a7,
    bitboard_getflips_b7,
    bitboard_getflips_c7,
    bitboard_getflips_d7,
    bitboard_getflips_e7,
    bitboard_getflips_f7,
    bitboard_getflips_g7,
    bitboard_getflips_h7,

    bitboard_getflips_a8,
    bitboard_getflips_b8,
    bitboard_getflips_c8,
    bitboard_getflips_d8,
    bitboard_getflips_e8,
    bitboard_getflips_f8,
    bitboard_getflips_g8,
    bitboard_getflips_h8

};

#if defined(WRAPPER_INTO_NAMESPACE_DOLPHIN) && (WRAPPER_INTO_NAMESPACE_DOLPHIN != 0)
}  // namespace dolphin
#endif
