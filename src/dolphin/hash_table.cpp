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

bool hash_table_t::g_mask_initialized = false;
internal::hash_mask_t hash_table_t::g_hash_disc_mask[CHESS_MAX_COLOR+1][BOARD_MAX_DISC];
internal::hash_mask_t hash_table_t::g_hash_flip_value[BOARD_MAX_DISC];
internal::hash_mask_t hash_table_t::g_hash_color_mask[CHESS_MAX_COLOR];
internal::hash_mask_t hash_table_t::g_hash_switch_side;
internal::hash_mask_t hash_table_t::g_hash_row_value[BOARD_ROW][BOARD_ROW_MASKS];
internal::hash_mask_t hash_table_t::g_hash_put_value[CHESS_MAX_COLOR+1][BOARD_MAX_DISC];

hash_table_t::hash_table_t( void ) :
    m_hash_table(NULL),
    m_hash_bits(0),
    m_hash_entries(0),
    m_hash_sizes(0),
    m_hash_mask(0),
    m_initialized(false)
{
    //init_hash(DEFAULT_HASH_BITS);
}

 hash_table_t::hash_table_t( bits_type hash_bits ) :
    m_hash_table(NULL),
    m_hash_bits(hash_bits),
    m_hash_entries(1 << hash_bits),
    m_hash_sizes(0),
    m_hash_mask((1 << hash_bits) - 1),
    m_initialized(false)
{
    init_hash(hash_bits);
    init_hash_mask();
}

hash_table_t::~hash_table_t( void )
{
    free_hash();
}
   
void hash_table_t::free_hash( void )
{
    if (is_initialized()) {
        __MY_ASSERT((m_hash_table == NULL),
            "@ chess_hash_table::free_hash(): m_hash_table == NULL.");
        if (is_valid())
            m_hash_memory.Free(true);

        m_hash_table = NULL;

        //m_hash_bits = 0;
        m_hash_entries = 0;
        m_hash_sizes = 0;
        m_hash_mask = 0;

        m_initialized = false;
    }
}

void hash_table_t::setup_hash( bits_type hash_bits /*= DEFAULT_HASH_BITS*/,
                                  bool bClear /*= true*/,
                                  bool bSrand /*= true */ )
{
    free_hash();

    init_hash(hash_bits);
    init_hash_mask(bClear, bSrand);
}

int hash_table_t::resize_hash( bits_type new_hash_bits )
{
    int old_hash_bits = m_hash_bits;
    free_hash();

    init_hash(new_hash_bits);
    init_hash_mask(true);

    return old_hash_bits;
}

void hash_table_t::clear_drafts( void )
{
    for (int i=0; i<(int)m_hash_entries; ++i)
        m_hash_table[i].key1_flags_draft &= ~DRAFT_MASK;
}

hash_table_t::pointer hash_table_t::init_hash_entries( entry_type hash_entries )
{
    // free hash first
    free_hash();

    if ( hash_entries <= 0 ) {
        m_hash_bits = DEFAULT_HASH_BITS;
        hash_entries = 1 << m_hash_bits;
    }
    m_hash_entries = hash_entries;
    m_hash_sizes = m_hash_entries * sizeof(internal::hash_entry_t);
    m_hash_mask = (mask_type)m_hash_entries - 1;

    size_type nAllocSize = m_hash_entries * sizeof(internal::hash_entry_t);
    m_hash_table = (hash_table_t::pointer)
        m_hash_memory.Malloc(nAllocSize, cache_aligned_t::USE_CURRENT_ALIGN_SIZE);
    __MY_ASSERT((m_hash_table != NULL), "m_hash_table is NULL, hash table malloc failure.");
    if (m_hash_table == NULL) {
        m_hash_bits = 0;
        m_hash_entries = 0;
        m_hash_sizes = 0;
        m_hash_mask = 0;
    }
    m_initialized = (m_hash_table != NULL);

    return m_hash_table;
}

hash_table_t::pointer hash_table_t::init_hash( bits_type hash_bits /* =DEFAULT_HASH_BITS */)
{
    entry_type hash_entries;
    if (hash_bits <= 0)
        hash_bits = DEFAULT_HASH_BITS;
    m_hash_bits = hash_bits;
    hash_entries = 1 << hash_bits;
    return init_hash_entries(hash_entries);
}

void hash_table_t::init_hash_mask( bool bClear, /* =true */
                                      bool bSrand /* =true */ )
{
    int i;
    int index;
    int j, pos, mask;
    bool is_first_bit;
    unsigned int scan_bit;
    unsigned int hash_tmp_low, hash_tmp_high;
    const unsigned int max_pair_closeness = 10;		// 10
    const unsigned int max_zero_closeness = 9;		// 9
    const int max_index = 130;
    unsigned int closeness;
    unsigned int rand_pair[max_index][2];
    int count = 0;

    if (bSrand) {
#if defined(_USE_C_SRAND) && _USE_C_SRAND
        sys_random::srand();
#else
        my_random::srand();
#endif
    }

    if (!is_initialized() || !is_valid()) {
        init_hash(m_hash_bits);
    }

    if (bClear) {
        for (i=0; i<(int)m_hash_entries; ++i) {
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
            get_bits_closeness( rand_pair[index][0], rand_pair[index][1], 0, 0 );
        if (closeness > max_zero_closeness)
            goto TRY_AGAIN2;
        for (i=0; i<index; i++) {
            closeness =
                get_bits_closeness( rand_pair[index][0], rand_pair[index][1],
                    rand_pair[i][0], rand_pair[i][1] );
            if (closeness > max_pair_closeness)
                goto TRY_AGAIN2;
            closeness =
                get_bits_closeness( rand_pair[index][0], rand_pair[index][1],
                    rand_pair[i][1], rand_pair[i][0] );
            if (closeness > max_pair_closeness)
                goto TRY_AGAIN2;
        }
        index++;
    }

	index = 0;
	for (i=0; i<BOARD_MAX_DISC; ++i) {
		g_hash_disc_mask[CHESS_BLACK][i].low  = 0;
		g_hash_disc_mask[CHESS_BLACK][i].high = 0;
		g_hash_disc_mask[CHESS_WHITE][i].low  = 0;
		g_hash_disc_mask[CHESS_WHITE][i].high = 0;
	}
	for (i=0; i<BOARD_MAX_DISC; ++i) {
		g_hash_disc_mask[CHESS_BLACK][i].low  = rand_pair[index][0];
		g_hash_disc_mask[CHESS_BLACK][i].high = rand_pair[index][1];
		index++;
		g_hash_disc_mask[CHESS_WHITE][i].low  = rand_pair[index][0];
		g_hash_disc_mask[CHESS_WHITE][i].high = rand_pair[index][1];
		index++;
	}

    for (i=0; i<BOARD_MAX_DISC; ++i) {
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
	for (i=0; i<BOARD_MAX_DISC; ++i) {
		g_hash_put_value[CHESS_BLACK][i].low  = g_hash_disc_mask[CHESS_BLACK][i].low  ^ g_hash_switch_side.low;
		g_hash_put_value[CHESS_BLACK][i].high = g_hash_disc_mask[CHESS_BLACK][i].high ^ g_hash_switch_side.high;
		g_hash_put_value[CHESS_WHITE][i].low  = g_hash_disc_mask[CHESS_WHITE][i].low  ^ g_hash_switch_side.low;
		g_hash_put_value[CHESS_WHITE][i].high = g_hash_disc_mask[CHESS_WHITE][i].high ^ g_hash_switch_side.high;
	}

    // disc row hash values
    for (i=0; i<BOARD_ROW; ++i) {
        for (mask=0; mask<BOARD_ROW_MASKS; ++mask) {
            hash_tmp_low  = 0;
            hash_tmp_high = 0;
            pos = i * BOARD_ROW;
            is_first_bit = true;
            scan_bit = 1;
            for (j=0; j<BOARD_COL; ++j) {
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
    g_mask_initialized = true;
}

void hash_table_t::determine_board_hash( const BitBoard my_bits,
                                            const BitBoard opp_bits,
                                            int color,
                                            internal::hash_mask_t& board_hash )
{
    int opp_color = OPP_COLOR( color );

    board_hash.low  = 0;
    board_hash.high = 0;
    for (int pos=0; pos<BOARD_MAX_DISC; ++pos) {
        if ( ((my_bits.low & bitboard::square_mask[pos].low) != 0)
            || ((my_bits.high & bitboard::square_mask[pos].high) != 0) ) {
                board_hash.low  ^= g_hash_disc_mask[color][pos].low;
                board_hash.high ^= g_hash_disc_mask[color][pos].high;
        }
        else {
            if ( ((opp_bits.low & bitboard::square_mask[pos].low) != 0)
                || ((opp_bits.high & bitboard::square_mask[pos].high) != 0) ) {
                    board_hash.low  ^= g_hash_disc_mask[opp_color][pos].low;
                    board_hash.high ^= g_hash_disc_mask[opp_color][pos].high;
            }
        }
    }
    board_hash.low  ^= g_hash_color_mask[color].low;
    board_hash.high ^= g_hash_color_mask[color].high;
}

}  // namespace dolphin
