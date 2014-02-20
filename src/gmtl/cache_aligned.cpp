
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <crtdbg.h>
#include <gmtl/cache_aligned.h>

namespace gmtl {

static unsigned char _cAlignSignFill    = 0xE9;     /* fill no-man's sign for aligned routines */
static unsigned char _cClearSignFill    = 0x00;     /* fill no-man's sign for free routines */

//
// MS1B: BitScanForward
//
// ²Î¿¼:
// http://stackoverflow.com/questions/466204/rounding-off-to-nearest-power-of-2
// http://stackoverflow.com/questions/364985/algorithm-for-finding-the-smallest-power-of-two-thats-greater-or-equal-to-a-giv
//

unsigned cache_aligned::_next_power_of_2(unsigned x)
{
#if 0
    unsigned MS1B = 1;
	while (MS1B < x)
		MS1B <<= 1;

    return MS1B;
#else
    // MS1B
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
#endif
}

/*******************************************************************************
*static int _CheckBytes() - verify byte range set to proper value
*
*Purpose:
*       verify byte range set to proper value
*
*Entry:
*       unsigned char *pb       - pointer to start of byte range
*       unsigned char bCheck    - value byte range should be set to
*       size_t nSize            - size of byte range to be checked
*
*Return:
*       TRUE - if all bytes in range equal bcheck
*       FALSE otherwise
*
*******************************************************************************/
bool __cdecl cache_aligned::_check_bytes(
        unsigned char *pb,
        unsigned char bCheck,
        size_t nSize
       )
{
        bool bOkay = true;
        while (nSize--) {
            if (*pb++ != bCheck) {
/* Internal error report is just noise; calling functions all report results - JWM */
/*                _RPT3(_CRT_WARN, "memory check error at 0x%p = 0x%02X, should be 0x%02X.\n", */
/*                    (BYTE *)(pb-1),*(pb-1), bCheck); */
                bOkay = false;
            }
        }
        return bOkay;
}

cache_aligned::cache_aligned(void)
{
	init_cache(DEFAILT_CACHE_ALIGN_SIZE, true);
}

cache_aligned::cache_aligned(size_t size)
{
    init_cache(DEFAILT_CACHE_ALIGN_SIZE, true);
    mem_malloc(size);
}

cache_aligned::cache_aligned(size_t size,
                             int align_size, /*= DEFAILT_CACHE_ALIGN_SIZE */
                             bool auto_delete /*= false */)
{
    init_cache(align_size, auto_delete);
    mem_malloc(size);
}

cache_aligned::cache_aligned(const cache_aligned &src, bool copy_data /*= true */)
{
    if (copy_data)
        copy(src, true);
    else
        clone(src, true);
}

cache_aligned& cache_aligned::operator =(const cache_aligned &src)
{
    copy(src);
    return *this;
}

cache_aligned::~cache_aligned(void)
{
    if (m_auto_delete)
	    free_cache(true);
}

void cache_aligned::init_cache(int align_size /*= DEFAILT_CACHE_ALIGN_SIZE */,
                               bool auto_delete /*= true */)
{
    m_pvData  = NULL;
    m_pvAlloc = NULL;

    m_size        = 0;
    m_alloc_size  = 0;

    m_auto_delete = auto_delete;
    m_inited      = false;

    set_align_size(align_size);
}

void cache_aligned::free_cache(bool force_delete /*= false */)
{
    if (m_inited) {
        bool need_delete = (force_delete || m_auto_delete);
        if (need_delete) {
            _ASSERT(m_pvAlloc != NULL);
            if (m_pvAlloc != NULL) {
                ALIGN_BLOCK_HEADER *pBlockHdr;
                void *pvAlloc;
                _ASSERT(m_pvData != NULL);
                if (m_pvData != NULL) {
                    pBlockHdr = (ALIGN_BLOCK_HEADER *)((unsigned int)m_pvData & ~(sizeof(unsigned int) - 1)) - 1;
                    _ASSERT(pBlockHdr < m_pvData);

                    pvAlloc = _free_block_header(pBlockHdr);
                    _ASSERT(pvAlloc != NULL);
                }
                ::free(m_pvAlloc);
            }

            m_pvAlloc = NULL;
            m_pvData  = NULL;

            m_size       = 0;
            m_alloc_size = 0;

            m_inited = false;
        }
    }
}

// full copy, include struct and data
bool cache_aligned::copy(const cache_aligned &src, bool _is_inited /*= false */)
{
    if (!_is_inited)
        free_cache(true);
    init_cache(src.get_align_size(), src.get_auto_delete());
	mem_malloc(src.get_size());
    return (copy_data(src) != NULL);
}

// only copy struct, not copy data
void cache_aligned::clone(const cache_aligned &src, bool _is_inited /*= false */)
{
    if (!_is_inited)
        free_cache(true);
    init_cache(src.get_align_size(), src.get_auto_delete());
	mem_malloc(src.get_size());
}

// only copy data
void *cache_aligned::copy_data(const cache_aligned &src)
{
    return mem_copy_s(src.get_ptr(), src.get_size());
}

int cache_aligned::_std_align_size(int align_size)
{
    if (align_size < 0)
        align_size = -align_size;
    align_size = _next_power_of_2(align_size);
    _ASSERT(IS_POWER_OF_2(align_size));

    align_size = (align_size > sizeof(unsigned int)) ? align_size : sizeof(unsigned int);
    _ASSERT(align_size > 0);

    _ASSERT(align_size >= MIN_CACHE_ALIGN_SIZE);
    if (align_size < MIN_CACHE_ALIGN_SIZE)
        align_size = MIN_CACHE_ALIGN_SIZE;

    _ASSERT(align_size <= MAX_CACHE_ALIGN_SIZE);
    if (align_size > MAX_CACHE_ALIGN_SIZE)
        align_size = MAX_CACHE_ALIGN_SIZE;

    return align_size;
}

int cache_aligned::set_align_size(int align_size, bool force_realloc /*= false */)
{
    // whether need force realloc the memory use the new align size?
    if (force_realloc) {
        if (align_size != m_align_size) {
            if (mem_realloc(m_size, align_size) != NULL)
                align_size = m_align_size;
            else
                align_size = -1;
        }
    }
    else if (!is_inited()) {
        m_align_size = _std_align_size(align_size);
        align_size = m_align_size;
    }
    else
        align_size = -1;

    return align_size;
}

void *cache_aligned::mem_malloc(size_t size,
                                int align_size, /*= -1 */
                                bool force_realloc /*= false */)
{
    if (!is_inited() || force_realloc)
        return mem_realloc(size, align_size);
    else
        return NULL;
}

void *cache_aligned::mem_realloc(size_t size,
                                 int align_size /*= -1 */)
{
    // Release previous alloc memory data first
    free_cache(true);

    ALIGN_BLOCK_HEADER *pBlockHdr;

    if (align_size == USE_CURRENT_ALIGN_SIZE)
        align_size = _std_align_size(m_align_size);
    else if (align_size == USE_DEFAULT_ALIGN_SIZE)
        align_size = DEFAILT_CACHE_ALIGN_SIZE;
    else
        align_size = _std_align_size(align_size);

    if (align_size != m_align_size)
        m_align_size = align_size;

    size_t align_mask = align_size - 1;

    size_t alloc_size;
    // alloc size align to n_align_size bytes (isn't must need)
    alloc_size = size + align_mask + sizeof(ALIGN_BLOCK_HEADER);

    if (alloc_size > 0 && alloc_size >= (size + sizeof(ALIGN_BLOCK_HEADER))) {
        void *pvAlloc = (void *)::malloc(alloc_size);
        if (pvAlloc != NULL) {
            // Save pvAlloc's value first
            m_pvAlloc = pvAlloc;
            // Data pointer align to n_align_size bytes
            if (sizeof(uintptr_t *) <= sizeof(uintptr_t)) {
                m_pvData = (void *)(((size_t)(unsigned char *)pvAlloc + align_mask + sizeof(ALIGN_BLOCK_HEADER))
                    & (~align_mask));
            }
            else {
                m_pvData = (void *)(((unsigned __int64)pvAlloc + align_mask + sizeof(ALIGN_BLOCK_HEADER))
                    & (~((unsigned __int64)align_mask)));
            }

            pBlockHdr = (ALIGN_BLOCK_HEADER *)(m_pvData) - 1;
            _ASSERT(pBlockHdr >= pvAlloc);

            pBlockHdr->pvAlloc = pvAlloc;
            ::memset((void *)pBlockHdr->Sign, _cAlignSignFill, ALIGN_SIGN_SIZE);

            m_size       = size;
            m_alloc_size = alloc_size;

            // for debug
            int nFrontPaddedSize = get_front_padded_size();
            int nLastPaddedSize  = get_last_padded_size();
            //nFrontPaddedSize = (unsigned long)m_pvData - (unsigned long)m_pvAlloc;
            //nLastPaddedSize  = nAllocSize - nSize - nFrontPaddedSize;

            _ASSERT(nFrontPaddedSize >= sizeof(ALIGN_BLOCK_HEADER));
            _ASSERT(nLastPaddedSize >= 0);

            m_inited = true;

            return m_pvData;
        }
        return NULL;
    }
    else {
        // alloc size overflow or error
        return NULL;
    }
}

void *cache_aligned::_free_block_header(ALIGN_BLOCK_HEADER *pBlockHdr,
                                        bool bFreeMemBlock /* = false*/)
{
    void *pvAlloc = NULL;

    _ASSERT(pBlockHdr != NULL);
    if (pBlockHdr != NULL) {
        pvAlloc = pBlockHdr->pvAlloc;
        _ASSERT((void *)pBlockHdr >= pvAlloc);

        if ((void *)pBlockHdr < pvAlloc) {
            // We don't know where pvData was allocated
            _RPT1(_CRT_ERROR, "Damage before 0x%p which was allocated by aligned routine\n", pvAlloc);
            pvAlloc = (void *)-2;
        }
        else {
            if (_check_bytes(pBlockHdr->Sign, _cAlignSignFill, ALIGN_SIGN_SIZE)) {
                // Set and fill clear sign 
                ::memset(pBlockHdr->Sign, _cClearSignFill, ALIGN_SIGN_SIZE);

                // Set pvAlloc's value to NULL
                pBlockHdr->pvAlloc = NULL;

                // Free memory block if need
                if (pvAlloc != NULL) {
                    if (bFreeMemBlock)
                        ::free(pvAlloc);
                }
            }
            else {
                // We don't know where pvData was allocated
                _RPT1(_CRT_ERROR, "Damage before 0x%p which was allocated by aligned routine\n", pvAlloc);
                pvAlloc = (void *)-1;
            }
        }
    }
    return pvAlloc;
}

void *cache_aligned::free_block(const void *pvData)
{
    ALIGN_BLOCK_HEADER *pBlockHdr;
    void *pvAlloc = NULL;

    _ASSERT(pvData != NULL);
    if (pvData != NULL) {
        pBlockHdr = (ALIGN_BLOCK_HEADER *)((unsigned int)pvData & ~(sizeof(unsigned int) - 1)) - 1;
        _ASSERT(pBlockHdr < pvData);

        pvAlloc = _free_block_header(pBlockHdr, true);
        _ASSERT(pvAlloc != NULL);
    }
    return pvAlloc;
}

void *cache_aligned::mem_set(int value, size_t size /* =0 */)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            if (size <= 0) {
                return ::memset(m_pvData, value, m_size);
            }
            else {
                if (size < m_size)
                    return ::memset(m_pvData, value, size);
                else
                    return ::memset(m_pvData, value, m_size);
            }
        }
    }
    return NULL;
}

void *cache_aligned::mem_copy(const void *src, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            count = (count > m_size) ? m_size : count;
            return ::memcpy(m_pvData, src, count);
        }
    }
    return NULL;
}

void *cache_aligned::mem_copy_s(const void *src, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            errno_t err = ::memcpy_s(m_pvData, m_size, src, count);
            if (err == 0)
                return m_pvData;
        }
    }
    return NULL;
}

void *cache_aligned::mem_move(const void *src, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            count = (count > m_size) ? m_size : count;
            return ::memmove(m_pvData, src, count);
        }
    }
    return NULL;
}

void *cache_aligned::mem_move_s(const void *src, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            errno_t err = ::memmove_s(m_pvData, m_size, src, count);
            if (err == 0)
                return m_pvData;
        }
    }
    return NULL;
}

void *cache_aligned::mem_chr(int c)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            return ::memchr(m_pvData, c, m_size);
        }
    }
    return NULL;
}

void *cache_aligned::mem_chr(size_t offset, int c)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            if (offset < m_size)
                return ::memchr((unsigned char *)m_pvData + offset, c, (m_size - offset));
        }
    }
    return NULL;
}

int cache_aligned::mem_cmp(const void *buf, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            count = (count > m_size) ? m_size : count;
            return ::memcmp((unsigned char *)m_pvData, buf, count);
        }
    }
    return NULL;
}

int cache_aligned::mem_icmp(const void *buf, size_t count)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            count = (count > m_size) ? m_size : count;
            return ::_memicmp((unsigned char *)m_pvData, buf, count);
        }
    }
    return NULL;
}

int cache_aligned::mem_icmp_l(const void *buf, size_t count, _locale_t locale)
{
    if (is_inited()) {
        if (m_pvAlloc != NULL && m_pvData != NULL && m_size > 0) {
            count = (count > m_size) ? m_size : count;
            return ::_memicmp_l((unsigned char *)m_pvData, buf, count, locale);
        }
    }
    return NULL;
}

}  // namespace gmtl
