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

#if defined(WRAPPED_INTO_NAMESPACE) && (WRAPPED_INTO_NAMESPACE != 0)
namespace dolphin {
#endif

ALIGN_PREFIX(64) static unsigned int my_flip_mask[8][256] ALIGN_SUFFIX(64);
ALIGN_PREFIX(64) static unsigned int opp_flip_mask[8][64] ALIGN_SUFFIX(64);

void init_flip_mask(void)
{
    int i, j, k;
    unsigned int scan_bits, move_bit, left_my_bit, right_my_bit;
    unsigned int my_bits, opp_bits, my_mask, opp_mask;
    unsigned int left_my_contigs, right_my_contigs;
    unsigned int left_opp_bits, right_opp_bits, left_opp_contigs, right_opp_contigs;
    unsigned int left_contiguous_mask[8], right_contiguous_mask[8];
    int contig_count, left_count, right_count;

    // 左和右方向定义使用常规黑白棋棋盘的定义, 即左上角位置为A1, 右上角位置为A8, 左下角为H1, 右下角为H8
    // 为了讨论方便, 我们把任意一条直线(横, 竖, 斜线)的棋子都映射到A1-A8的8个棋子上, 下同

    // 先得到从move_bit(第0-7位)位置开始搜索的左方向和右方向上连续棋子的mask bits
    move_bit = 1;
    // 从A1位置向左搜索的连续旗子的mask bits为0
    left_contiguous_mask[0]  = 0;
    // 从A2位置向右搜索的连续旗子的mask bits为0xFE(111111110B, 二进制)
    right_contiguous_mask[0] = 0xFE;
    for (i = 1; i < 8; i++) {
        // 类推到A2以及A8位置的左方向
        left_contiguous_mask[i]  = left_contiguous_mask[i - 1] | move_bit;
        // 类推到A2以及A8位置的右方向
        right_contiguous_mask[i] = (right_contiguous_mask[i - 1] << 1) & 0xFF;
        move_bit <<= 1;
    }

    move_bit = 1;
    // 循环move_bit(0-7)
    for (i = 0; i < 8; i++) {
        // 遍历opp_bits时, 只需要用到中间的6个bit, 最边上的各1个bit无用
        for (opp_mask = 0; opp_mask < 64; opp_mask++) {
            // opp_bits范围为: (00000010B - 01111110B) (二进制), 即2~126
            opp_bits = opp_mask << 1;
            // 下棋位置出现对手的棋子时(一般情况下, 不应该出现这种情况), opp_flip_mask值为0
            if ((opp_bits & move_bit) == move_bit && (move_bit != 0)) {
                opp_flip_mask[i][opp_mask] = 0;
            }
            else {
                // 搜索当前位置的左方向
                left_opp_bits = opp_bits & left_contiguous_mask[i];
                scan_bits = move_bit >> 1;
                // 计算左方向上连续的棋子数
                contig_count = 0;
                for (j = i-1; j >= 0; j--) {
                    if ((scan_bits & left_opp_bits) == 0)
                        break;
                    scan_bits >>= 1;
                    contig_count++;
                }
                // 根据左方向上连续的棋子数得到左方向的连续棋子bits
                if (contig_count == 0)
                    left_opp_contigs = 0;
                else
                    left_opp_contigs = scan_bits & left_contiguous_mask[i];

                // 搜索当前位置的右方向
                right_opp_bits = opp_bits & right_contiguous_mask[i];
                scan_bits = move_bit << 1;
                // 计算右方向上连续的棋子数
                contig_count = 0;
                for (j = i+1; j < 8; j++) {
                    if ((scan_bits & right_opp_bits) == 0)
                        break;
                    scan_bits <<= 1;
                    contig_count++;
                }
                // 根据右方向上连续的棋子数得到右方向的连续棋子bits
                if (contig_count == 0)
                    right_opp_contigs = 0;
                else
                    right_opp_contigs = scan_bits & right_contiguous_mask[i];

                // 合并左, 右方向上的连续棋子bits
                opp_flip_mask[i][opp_mask] = left_opp_contigs | right_opp_contigs;
            }
        }
        move_bit <<= 1;
    }

    // my_flip_mask初始化
    for (i = 0; i < 8; i++) {
        for (my_bits = 0; my_bits < 256; my_bits++)
            my_flip_mask[i][my_bits] = 0;
    }

    move_bit = 1;
    // 循环move_bit(0-7)
    for (i = 0; i < 8; i++) {
        // left dir
        j = i - 1;
        left_count = 0;
        left_my_bit = move_bit;
        do {
            left_my_bit = (left_my_bit >> 1) & left_contiguous_mask[i];
            if (left_count == 0 || j < 0)
                left_my_contigs = 0;
            else
                left_my_contigs = (move_bit - 1) - ((left_my_bit << 1) - 1);

            // right dir
            k = i + 1;
            right_count = 0;
            right_my_bit = move_bit;
            do {
                right_my_bit = (right_my_bit << 1) & right_contiguous_mask[i];
                if (right_count == 0 || k >= 8)
                    right_my_contigs = 0;
                else
                    right_my_contigs = (right_my_bit - 1) - move_bit - (move_bit - 1);

                // my_mask = right_my_bit - now_bit - (left_my_bit << 1);
                my_mask = (left_my_contigs & left_contiguous_mask[i]) | (right_my_contigs & right_contiguous_mask[i]);

                my_bits = left_my_bit | right_my_bit;
                my_flip_mask[i][my_bits] = my_mask;
                if (left_count == 0)
                    my_flip_mask[i][right_my_bit] = my_mask;
                if (right_count == 0)
                    my_flip_mask[i][left_my_bit] = my_mask;

                right_count++;
                k++;
            } while (k < 8);

            left_count++;
            j--;
        } while (j >= 0);

        move_bit <<= 1;
    }
}

BB_FLIP_FUNC bitboard_getflips_a1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h1(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h2(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h3(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h4(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h5(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h6(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h7(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_a8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_b8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_c8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_d8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_e8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_f8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_g8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

BB_FLIP_FUNC bitboard_getflips_h8(const bitboard &my_bits, const bitboard &opp_bits, bitboard &flip_bits)
{
    return 0;
}

//int (__FASTCALL(2) * const bitboard_getflips[64])(const bitboard &, const bitboard &, bitboard &) = {
bitboard_getflips_func_t bitboard_getflips[64] = {

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

#if defined(WRAPPED_INTO_NAMESPACE) && (WRAPPED_INTO_NAMESPACE != 0)
}  // namespace dolphin
#endif
