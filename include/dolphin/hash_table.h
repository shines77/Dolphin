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

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <gmtl/cache_aligned.h>
#include <dolphin/dol_stddef.h>
#include <dolphin/colour.h>
#include <dolphin/board.h>
#include <dolphin/bitboard.h>
#include <cstddef>
#include <stdlib.h>
#include <math.h>

using namespace gmtl;

/* Substitute an old position with draft n if the new position has
   draft >= n-4 */
#define REPLACEMENT_OFFSET      4

#define DEFAULT_HASH_BITS       20
#define DRAFT_MASK              0x000000FFUL
#define KEY1_MASK               0xFFFF0000UL

#define SECONDARY_HASH(a)       ((a) ^ 1)

namespace dolphin {

namespace internal {

#if defined(__GNUC__)
    struct hash_entry_t {
        unsigned int key2;
        unsigned int key1_flags_draft;
        unsigned int selectivity_eval;
        unsigned int moves;
    } __attribute((aligned(1)));

    struct extend_hash_entry_t {
        unsigned int key2;
        unsigned int key1;
        int          flags;
        int          draft;
        unsigned int moves[4];
        int          selectivity;
        int          eval;
        unsigned int reserved[6];
    } __attribute((aligned(1)));

    struct hash_mask_t {
        unsigned int low;
        unsigned int high;
    } __attribute((aligned(1)));
#else
#pragma pack(push, 1)
    struct hash_entry_t {
        unsigned int key2;
        unsigned int key1_flags_draft;
        unsigned int selectivity_eval;
        unsigned int moves;
    };

    struct extend_hash_entry_t {
        unsigned int key2;
        unsigned int key1;
        int          flags;
        int          draft;
        unsigned int moves[4];
        int          selectivity;
        int          eval;
        unsigned int reserved[6];
    };

    struct hash_mask_t {
        unsigned int low;
        unsigned int high;
    };
#pragma pack(pop)
#endif

};

using internal::hash_mask_t;

class hash_table_t : internal::no_copy
{
public:
    typedef long value_type;
    typedef std::size_t size_type;
    typedef unsigned int bits_type;
    typedef std::size_t entry_type;
    typedef unsigned int mask_type;
    typedef internal::hash_entry_t* pointer;
    typedef const internal::hash_entry_t* const_pointer;
    typedef internal::hash_entry_t& reference;
    typedef const internal::hash_entry_t& const_reference;

private:
    pointer     m_hash_table;
    cache_aligned_t m_hash_memory;
    bits_type   m_hash_bits;
    entry_type  m_hash_entries;
    size_type   m_hash_sizes;
    mask_type   m_hash_mask;
    bool        m_initialized;
    static bool g_mask_initialized;

public:
    /* The 64-bit hash masks for a piece of a certain color in a
        certain position. */
    static internal::hash_mask_t g_hash_disc_mask[CHESS_MAX_COLOR+1][BOARD_MAX_DISC];
    static internal::hash_mask_t g_hash_flip_value[BOARD_MAX_DISC];
    static internal::hash_mask_t g_hash_color_mask[CHESS_MAX_COLOR];
    static internal::hash_mask_t g_hash_switch_side;
    static internal::hash_mask_t g_hash_row_value[BOARD_ROW][BOARD_ROW_MASKS];
    static internal::hash_mask_t g_hash_put_value[CHESS_MAX_COLOR+1][BOARD_MAX_DISC];

    inline static unsigned int hash_table_t::popcount32( unsigned int bits );
    inline static unsigned int get_bits_closeness( unsigned int a0, unsigned int a1,
              unsigned int b0, unsigned int b1 );

public:
    hash_table_t( void );
    explicit hash_table_t( bits_type hash_bits );
    virtual ~hash_table_t( void );

    inline entry_type hash_entries( void ) const { return m_hash_entries; };
    inline entry_type hash_sizes( void ) const { return m_hash_sizes; };
    inline bits_type  hash_bits( void ) const { return m_hash_bits; };
    inline mask_type  hash_mask( void ) const { return m_hash_mask; };
    inline pointer hash_table( void ) const { return m_hash_table; };
    inline pointer data_ptr( void ) const { return (pointer)m_hash_memory.DataPtr(); };
    inline pointer alloc_ptr( void ) const { return (pointer)m_hash_memory.AllocPtr(); };

    inline bool is_valid( void ) const {
        return ((m_hash_table != NULL) && (m_hash_memory.DataPtr() != NULL));
    };
    inline bool is_data_valid( void ) const {
        return (m_hash_memory.DataPtr() != NULL);
    };
    inline bool is_alloc_valid( void ) const {
        return (m_hash_memory.AllocPtr() != NULL);
    };
    inline bool is_initialized( void ) const { return m_initialized; };
    inline bool is_mask_initialized( void ) const { return g_mask_initialized; };

    void setup_hash( bits_type hash_bits = DEFAULT_HASH_BITS,
        bool bClear = true, bool bSrand = true );

    pointer init_hash( bits_type hash_bits = DEFAULT_HASH_BITS );
    void free_hash( void );

    void init_hash_mask( bool bClear = true, bool bSrand = true );

    int resize_hash( bits_type new_hash_bits ); 
    void clear_drafts( void );

    void determine_board_hash( const BitBoard my_bits,
        const BitBoard opp_bits, int color,
        internal::hash_mask_t& board_hash );

protected:
    pointer init_hash_entries( entry_type hash_entries );
};

/*
  POPCOUNT
*/
inline
unsigned int
hash_table_t::popcount32( unsigned int bits ) {
	unsigned int n;
	for ( n = 0; bits != 0; n++, bits &= (bits - 1) )
        ;
	return n;
}

/*
  GET_CLOSENESS
  Returns the closeness between the 64-bit integers (a0,a1) and (b0,b1).
  A closeness of 0 means that 32 bits differ.
*/
inline
unsigned int
hash_table_t::get_bits_closeness( unsigned int a0, unsigned int a1,
              unsigned int b0, unsigned int b1 ) {
    return (unsigned int)::abs((int)(popcount32(a0 ^ b0) + popcount32(a1 ^ b1) - 32));
}

}  // namespace dolphin

#endif  /* _HASH_TABLE_H_ */
