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

#ifndef _CACHE_ALIGNED_T_H_
#define _CACHE_ALIGNED_T_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "gmtl_stddef.h"

#define DEFAILT_CACHE_ALIGN_SIZE    128

// intel 32bit CPU cache line align size is 32 or 64 byte
#define MIN_CACHE_ALIGN_SIZE        16
// <= 8M Bytes
#define MAX_CACHE_ALIGN_SIZE        0x00800000UL

#define ALIGN_SIGN_SIZE             sizeof(void *)

#define IS_POWER_OF_2(x)            (((x)&(x-1)) == 0)

namespace gmtl {

typedef struct _align_block_header
{
    void            *pvAlloc;
    unsigned char   Sign[ALIGN_SIGN_SIZE];
} ALIGN_BLOCK_HEADER, *PALIGN_BLOCK_HEADER;

class cache_aligned_t
{
private:
	void   *m_pvData;
    void   *m_pvAlloc;

    int     m_nAlignSize;
    size_t  m_nSize;
    size_t  m_nAllocSize;

    bool    m_bAutoDelete;
    bool    m_bInited;

public:
	cache_aligned_t(void);
    explicit cache_aligned_t(size_t nSize);
	cache_aligned_t(size_t nSize, int nAlignSize,
        bool bAutoDelete = true);
    explicit cache_aligned_t(const cache_aligned_t& src, bool bCopyData = true);
    virtual ~cache_aligned_t(void);

    cache_aligned_t& operator =(const cache_aligned_t& src);

public:
    static const int USE_CURRENT_ALIGN_SIZE = -1;
    static const int USE_DEFAULT_ALIGN_SIZE = -2;

    static void *FreeBlock(const void *pvData);
    static unsigned _NearestPowerOf2(unsigned x);
    static bool __cdecl _CheckBytes(unsigned char *, unsigned char, size_t);

    inline int     AlignSize(void) const { return m_nAlignSize; };
    inline size_t  Size(void) const { return m_nSize; };
    inline size_t  AllocSize(void) const { return m_nAllocSize; };

    int     SetAlignSize(int nAlignSize, bool bForceReset = false);

    bool    GetAutoDelete(void) const { return m_bAutoDelete; };
    bool    SetAutoDelete(bool bAutoDelete) {
        bool bOldValue = m_bAutoDelete;
        m_bAutoDelete = bAutoDelete;
        return bOldValue;
    };

    int     FrontPaddedSize(void) const {
        return (int)((unsigned char *)m_pvData - (unsigned char *)m_pvAlloc);
    };
    int     LastPaddedSize (void) const {
        return (int)(m_nAllocSize - m_nSize - FrontPaddedSize());
    };

    inline bool    IsInited(void)       { return m_bInited; };
    inline void   *GetPtr(void) const   { return m_pvData;  };
    inline void   *DataPtr(void) const  { return m_pvData;  };
    inline void   *AllocPtr(void) const { return m_pvAlloc; };

    void   *Malloc   (size_t nSize, int nAlignSize = USE_CURRENT_ALIGN_SIZE, bool bForceRealloc = false);
    void   *Realloc  (size_t nSize, int nAlignSize = USE_CURRENT_ALIGN_SIZE);
    bool    Copy     (const cache_aligned_t& src, bool bIsInit = false); // full copy, include struct and data
    void    Clone    (const cache_aligned_t& src, bool bIsInit = false); // only copy struct, not copy data
    void   *CopyData (const cache_aligned_t& src);                       // only copy data
    void    Free     (bool bForceDelete = false);

    void   *MemSet   (int nValue);
    void   *MemCopy  (const void *src, size_t count);
    void   *MemMove  (const void *src, size_t count);
    void   *MemCopy_s(const void *src, size_t count);
    void   *MemMove_s(const void *src, size_t count);

    void   *MemChr   (int c);
    void   *MemChr   (size_t offset, int c);

    int     MemCmp   (const void *buf, size_t count);
    int     MemICmp  (const void *buf, size_t count);
    int     MemICmp_l(const void *buf, size_t count, _locale_t locale);

protected:
    //

private:
    void    Init(int nAlignSize = DEFAILT_CACHE_ALIGN_SIZE, bool bAutoDelete = true);
    int     _StdAlignSize(int nAlignSize);

    static void *_FreeBlockHeader(ALIGN_BLOCK_HEADER *pBlockHdr, bool bFreeMemBlock = false);	
};

}  // namespace imtl

#endif  // _CACHE_ALIGNED_T_H_
