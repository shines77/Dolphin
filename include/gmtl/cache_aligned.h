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

#ifndef _GMTL_CACHE_ALIGNED_H_
#define _GMTL_CACHE_ALIGNED_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <gmtl/gmtl_stddef.h>

#define DEFAILT_CACHE_ALIGN_SIZE    128

// intel 32bit CPU cache line align size is 32 or 64 byte
#define MIN_CACHE_ALIGN_SIZE        16
// <= 8M Bytes
#define MAX_CACHE_ALIGN_SIZE        0x00800000UL

#define ALIGN_SIGN_SIZE             sizeof(void *)

/* unsigned int v; */
/* f = (v & (v - 1)) == 0; */
/* f = v && !(v & (v - 1)); */
#define GMTL_IS_POWER_2(V)          ((V) && (!((uint32_t)(V) & ((uint32_t)(V) - 1))))

#define IS_POWER_OF_2(x)            ((((x) != 0)) && (((x) & ((x) - 1)) == 0))

namespace gmtl {

typedef struct align_block_header_t
{
    void *          pvAlloc;
    unsigned char   Sign[ALIGN_SIGN_SIZE];
} ALIGN_BLOCK_HEADER, * PALIGN_BLOCK_HEADER;

class cache_aligned
{
private:
    void *  m_pvData;
    void *  m_pvAlloc;

    int     m_align_size;
    size_t  m_size;
    size_t  m_alloc_size;

    bool    m_auto_delete;
    bool    m_inited;

public:
    cache_aligned(void);
    explicit cache_aligned(size_t size);
    cache_aligned(size_t size, int align_size, bool auto_delete = true);
    explicit cache_aligned(const cache_aligned &src, bool copy_data = true);
    virtual ~cache_aligned(void);

    cache_aligned & operator =(const cache_aligned &src);

public:
    static const int USE_CURRENT_ALIGN_SIZE = -1;
    static const int USE_DEFAULT_ALIGN_SIZE = -2;

    static void *       free_block(const void *pvData);
    static unsigned     _next_power_of_2(unsigned x);
    static bool __cdecl _check_bytes(unsigned char *, unsigned char, size_t);

    void            free_cache(bool force_delete = false);

    inline int      get_align_size(void) const   { return m_align_size; };
    inline size_t   get_size(void) const         { return m_size;      };
    inline size_t   get_alloc_size(void) const   { return m_alloc_size; };

    int             set_align_size(int align_size, bool force_realloc = false);

    bool get_auto_delete(void) const { return m_auto_delete; };
    bool set_auto_delete(bool auto_delete) {
        bool old_value = m_auto_delete;
        m_auto_delete = auto_delete;
        return old_value;
    };

    int get_front_padded_size(void) const {
        return (int)((unsigned char *)m_pvData - (unsigned char *)m_pvAlloc);
    };
    int get_last_padded_size (void) const {
        return (int)(m_alloc_size - m_size - get_front_padded_size());
    };

    inline bool     is_inited(void)           { return m_inited;  };
    inline void *   get_ptr(void) const       { return m_pvData;  };
    inline void *   get_data_ptr(void) const  { return m_pvData;  };
    inline void *   get_alloc_ptr(void) const { return m_pvAlloc; };

    void *          mem_alloc  (size_t size, int align_size = USE_CURRENT_ALIGN_SIZE, bool force_realloc = false);
    void *          mem_realloc(size_t size, int align_size = USE_CURRENT_ALIGN_SIZE);
    bool            copy       (const cache_aligned &src, bool _is_inited = false);     // full copy, include struct and data
    void            clone      (const cache_aligned &src, bool _is_inited = false);     // only copy struct, not copy data
    void *          copy_data  (const cache_aligned &src);                               // only copy data

    void *          mem_set    (int value, size_t size = 0);
    void *          mem_copy   (const void *src, size_t count);
    void *          mem_move   (const void *src, size_t count);
    void *          mem_copy_s (const void *src, size_t count);
    void *          mem_move_s (const void *src, size_t count);

    void *          mem_chr    (int c);
    void *          mem_chr    (size_t offset, int c);

    int             mem_cmp    (const void *buf, size_t count);
    int             mem_icmp   (const void *buf, size_t count);
    int             mem_icmp_l (const void *buf, size_t count, _locale_t locale);

protected:
    void            init_cache(int align_size = DEFAILT_CACHE_ALIGN_SIZE, bool auto_delete = true);

private:
    int             _std_align_size(int align_size);

    static void *   _free_block_header(ALIGN_BLOCK_HEADER * pBlockHdr, bool b_free_memblock = false);
};

}  // namespace gmtl

#endif  /* _GMTL_CACHE_ALIGNED_H_ */
