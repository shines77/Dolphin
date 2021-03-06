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

#include <dolphin/hash_table.h>
#include <dolphin/my_random.h>

// use srand() or my_srandom()
#define _USE_C_SRAND     0

namespace dolphin {

bool hash_table::s_mask_inited = false;

internal::hash_mask_t hash_table::g_hash_disc_mask[CHESS_MAX_COLOR + 1][BOARD_MAX_DISCS];
internal::hash_mask_t hash_table::g_hash_flip_value[BOARD_MAX_DISCS];
internal::hash_mask_t hash_table::g_hash_color_mask[CHESS_MAX_COLOR];
internal::hash_mask_t hash_table::g_hash_switch_side;
internal::hash_mask_t hash_table::g_hash_row_value[BOARD_ROWS][BOARD_ROW_MASKS];
internal::hash_mask_t hash_table::g_hash_put_value[CHESS_MAX_COLOR + 1][BOARD_MAX_DISCS];

//
// MS1B: BitScanForward
//
// �ο�:
// http://stackoverflow.com/questions/466204/rounding-off-to-nearest-power-of-2
// http://stackoverflow.com/questions/364985/algorithm-for-finding-the-smallest-power-of-two-thats-greater-or-equal-to-a-giv
//

hash_table::bits_type hash_table::_next_power_of_2(hash_table::bits_type v)
{
    if (!DOL_IS_POWER_2(v)) {
#if 0
        bits_type MS1B = 1;
        while (MS1B < v)
            MS1B <<= 1;

        return MS1B;
#else
        // MS1B
        --v;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return ++v;
#endif
    }
    return v;
}

hash_table::bits_type hash_table::_get_log_2(hash_table::bits_type v)
{
    entry_type entries = 0;
    if (v == 0)
        return entries;
    // v round to power of 2
    if (!DOL_IS_POWER_2(v))
        v = hash_table::_next_power_of_2(v);
    while ((v & 1) == 0) {
        entries++;
        v >>= 1;
    }
    return entries;
}

hash_table::hash_table(void) :
    m_hash_table(NULL),
    m_hash_bits(0),
    m_hash_entries(0),
    m_hash_sizes(0),
    m_hash_mask(0),
    m_inited(false)
{
    init_hash_bits(0);
    init_hash_mask();
}

 hash_table::hash_table(bits_type hash_bits) :
    m_hash_table(NULL),
    m_hash_bits(hash_bits),
    m_hash_entries(1 << hash_bits),
    m_hash_sizes(0),
    m_hash_mask((1 << hash_bits) - 1),
    m_inited(false)
{
    init_hash_bits(hash_bits);
    init_hash_mask();
}

hash_table::~hash_table(void)
{
    free_hash();
}

void hash_table::free_hash(void)
{
    if (is_inited()) {
        DOL_ASSERT((m_hash_table == NULL),
            "@ chess_hash_table::free_hash(): m_hash_table == NULL.");
        if (is_valid())
            m_hash_memory.free_cache(true);

        m_hash_table = NULL;

        m_hash_bits = 0;
        m_hash_entries = 0;
        m_hash_sizes = 0;
        m_hash_mask = 0;

        m_inited = false;
    }
}

hash_table::pointer hash_table::init_hash_bits(bits_type hash_bits /* =DEFAULT_HASH_BITS */)
{
    entry_type hash_entries;
    if (hash_bits <= 0) {
        hash_bits = 0;
        hash_entries = 0;
    }
    else {
        m_hash_bits = hash_bits;
        hash_entries = 1 << hash_bits;
    }
    return reinit_hash_entries(hash_entries);
}

hash_table::pointer hash_table::init_hash_entries(entry_type hash_entries /* =DEFAULT_HASH_ENTRIES */)
{
    bits_type hash_bits;
    if (hash_entries <= 0)
        hash_entries = 0;
    hash_entries = hash_table::_next_power_of_2(hash_entries);
    hash_bits = hash_table::_get_log_2(hash_entries);
    m_hash_bits = hash_bits;
    hash_entries = hash_entries;
    return reinit_hash_entries(hash_entries);
}

hash_table::pointer hash_table::reinit_hash_entries(entry_type hash_entries)
{
    // free hash first
    free_hash();

    if (hash_entries > 0) {
        hash_entries = hash_table::_next_power_of_2(hash_entries);
        m_hash_entries = hash_entries;
        m_hash_sizes = m_hash_entries * sizeof(internal::hash_entry_t);
        m_hash_mask = (mask_type)m_hash_entries - 1;

        size_type nAllocSize = m_hash_entries * sizeof(internal::hash_entry_t);
        m_hash_table = (hash_table::pointer)
            m_hash_memory.mem_alloc(nAllocSize, cache_aligned::USE_CURRENT_ALIGN_SIZE);
        DOL_ASSERT((m_hash_table != NULL), "m_hash_table is NULL, hash table malloc failure.");
        if (m_hash_table == NULL) {
            m_hash_bits = 0;
            m_hash_entries = 0;
            m_hash_sizes = 0;
            m_hash_mask = 0;
        }
    }
    m_inited = (m_hash_table != NULL);

    return m_hash_table;
}

int hash_table::resize_hash(bits_type new_hash_bits)
{
    int old_hash_bits = m_hash_bits;
    free_hash();

    init_hash_bits(new_hash_bits);
    init_hash_mask(true);

    return old_hash_bits;
}

void hash_table::clear_drafts(void)
{
    for (int i = 0; i < (int)m_hash_entries; ++i)
        m_hash_table[i].key1_flags_draft &= ~DRAFT_MASK;
}

void hash_table::setup_hash_bits(bits_type hash_bits    /* = DEFAULT_HASH_BITS */,
                                 bool b_clear           /* = true */,
                                 bool b_srand           /* = true */)
{
    free_hash();

    init_hash_bits(hash_bits);
    init_hash_mask(b_clear, b_srand);
}

void hash_table::setup_hash_entries(entry_type hash_entries /* = DEFAULT_HASH_ENTRIES */,
                                    bool b_clear            /* = true */,
                                    bool b_srand            /* = true */)
{
    free_hash();

    init_hash_entries(hash_entries);
    init_hash_mask(b_clear, b_srand);
}

void hash_table::init_hash_mask(bool b_clear, /* = true */
                                bool b_srand  /* = true */)
{
    int i, j;
    int index;
    int pos, mask;
    bool is_first_bit;
    unsigned int scan_bit;
    unsigned int hash_tmp_low, hash_tmp_high;
    const unsigned int max_pair_closeness = 10;     // 10
    const unsigned int max_zero_closeness = 9;      // 9
    const int max_index = 130;
    unsigned int closeness;
    unsigned int rand_pair[max_index][2];
    int count = 0;

    if (b_srand) {
#if defined(_USE_C_SRAND) && _USE_C_SRAND
        sys_random::srand();
#else
        my_random::srand();
#endif
    }

    if (!is_inited() || !is_valid()) {
        init_hash_bits(m_hash_bits);
    }

    if (b_clear) {
        for (i = 0; i < (int)m_hash_entries; ++i) {
            m_hash_table[i].key2 = 0;
            m_hash_table[i].key1_flags_draft &= (~KEY1_MASK) & (~DRAFT_MASK);
        }
    }

    index = 0;
    while (index < max_index) {     // max_index = 130
TRY_AGAIN2:
#if defined(_USE_C_SRAND) && _USE_C_SRAND
        rand_pair[index][0] = ((unsigned int)sys_random::rand() << 3) + ((unsigned int)sys_random::rand() >> 2);
        rand_pair[index][1] = ((unsigned int)sys_random::rand() << 3) + ((unsigned int)sys_random::rand() >> 2);
#else
        rand_pair[index][0] = ((unsigned int)my_random::rand() << 3) + ((unsigned int)my_random::rand() >> 2);
        rand_pair[index][1] = ((unsigned int)my_random::rand() << 3) + ((unsigned int)my_random::rand() >> 2);
#endif

#if 0
        count++;
        printf("\x08\x08\x08\x08\x08\x08\x08\x08\x08%d", count);
        fflush(stdout);
        Sleep(200);
#endif

        closeness =
            get_bits_closeness(rand_pair[index][0], rand_pair[index][1], 0, 0);
        if (closeness > max_zero_closeness)
            goto TRY_AGAIN2;
        for (i = 0; i < index; i++) {
            closeness =
                get_bits_closeness(rand_pair[index][0], rand_pair[index][1],
                    rand_pair[i][0], rand_pair[i][1]);
            if (closeness > max_pair_closeness)
                goto TRY_AGAIN2;
            closeness =
                get_bits_closeness(rand_pair[index][0], rand_pair[index][1],
                    rand_pair[i][1], rand_pair[i][0]);
            if (closeness > max_pair_closeness)
                goto TRY_AGAIN2;
        }
        index++;
    }

    index = 0;
    for (i = 0; i < BOARD_MAX_DISCS; ++i) {
        g_hash_disc_mask[CHESS_BLACK][i].low  = 0;
        g_hash_disc_mask[CHESS_BLACK][i].high = 0;
        g_hash_disc_mask[CHESS_WHITE][i].low  = 0;
        g_hash_disc_mask[CHESS_WHITE][i].high = 0;
    }
    for (i = 0; i < BOARD_MAX_DISCS; ++i) {
        g_hash_disc_mask[CHESS_BLACK][i].low  = rand_pair[index][0];
        g_hash_disc_mask[CHESS_BLACK][i].high = rand_pair[index][1];
        index++;
        g_hash_disc_mask[CHESS_WHITE][i].low  = rand_pair[index][0];
        g_hash_disc_mask[CHESS_WHITE][i].high = rand_pair[index][1];
        index++;
    }

    for (i = 0; i < BOARD_MAX_DISCS; ++i) {
        g_hash_flip_value[i].low  = g_hash_disc_mask[CHESS_BLACK][i].low  ^ g_hash_disc_mask[CHESS_WHITE][i].low;
        g_hash_flip_value[i].high = g_hash_disc_mask[CHESS_BLACK][i].high ^ g_hash_disc_mask[CHESS_WHITE][i].high;
    }

    g_hash_color_mask[CHESS_BLACK].low  = rand_pair[index][0];
    g_hash_color_mask[CHESS_BLACK].high = rand_pair[index][1];
    index++;
    g_hash_color_mask[CHESS_WHITE].low  = rand_pair[index][0];
    g_hash_color_mask[CHESS_WHITE].high = rand_pair[index][1];
    index++;

    g_hash_switch_side.low  = g_hash_color_mask[CHESS_BLACK].low  ^ g_hash_color_mask[CHESS_WHITE].low;
    g_hash_switch_side.high = g_hash_color_mask[CHESS_BLACK].high ^ g_hash_color_mask[CHESS_WHITE].high;

    // put the disc hash values
    for (i = 0; i < BOARD_MAX_DISCS; ++i) {
        g_hash_put_value[CHESS_BLACK][i].low  = g_hash_disc_mask[CHESS_BLACK][i].low  ^ g_hash_switch_side.low;
        g_hash_put_value[CHESS_BLACK][i].high = g_hash_disc_mask[CHESS_BLACK][i].high ^ g_hash_switch_side.high;
        g_hash_put_value[CHESS_WHITE][i].low  = g_hash_disc_mask[CHESS_WHITE][i].low  ^ g_hash_switch_side.low;
        g_hash_put_value[CHESS_WHITE][i].high = g_hash_disc_mask[CHESS_WHITE][i].high ^ g_hash_switch_side.high;
    }

    // disc row hash values
    for (i = 0; i < BOARD_ROWS; ++i) {
        for (mask = 0; mask < BOARD_ROW_MASKS; ++mask) {
            hash_tmp_low  = 0;
            hash_tmp_high = 0;
            pos = i * BOARD_ROWS;
            is_first_bit = true;
            scan_bit = 1;
            for (j = 0; j < BOARD_COLS; ++j) {
                if ((mask & scan_bit) == scan_bit) {
                    if (is_first_bit) {
                        hash_tmp_low  = g_hash_flip_value[pos].low;
                        hash_tmp_high = g_hash_flip_value[pos].high;
                        is_first_bit = false;
                    }
                    else {
                        hash_tmp_low  ^= g_hash_flip_value[pos].low;
                        hash_tmp_high ^= g_hash_flip_value[pos].high;
                    }
                }
                pos++;
                scan_bit <<= 1;
            }
            g_hash_row_value[i][mask].low  = hash_tmp_low;
            g_hash_row_value[i][mask].high = hash_tmp_high;
        }
    }

    // global mask initialized status
    s_mask_inited = true;
}

void hash_table::determine_board_hash(const BitBoard my_bits,
                                      const BitBoard opp_bits,
                                      int color,
                                      internal::hash_mask_t& board_hash)
{
    int opp_color = OPP_COLOR(color);

    board_hash.low  = 0;
    board_hash.high = 0;
    for (int pos = 0; pos < BOARD_MAX_DISCS; ++pos) {
        if (((my_bits.low & bitboard::square_mask[pos].low) != 0)
            || ((my_bits.high & bitboard::square_mask[pos].high) != 0)) {
                board_hash.low  ^= g_hash_disc_mask[color][pos].low;
                board_hash.high ^= g_hash_disc_mask[color][pos].high;
        }
        else {
            if (((opp_bits.low & bitboard::square_mask[pos].low) != 0)
                || ((opp_bits.high & bitboard::square_mask[pos].high) != 0)) {
                    board_hash.low  ^= g_hash_disc_mask[opp_color][pos].low;
                    board_hash.high ^= g_hash_disc_mask[opp_color][pos].high;
            }
        }
    }
    board_hash.low  ^= g_hash_color_mask[color].low;
    board_hash.high ^= g_hash_color_mask[color].high;
}

}  // namespace dolphin
