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

#if !defined(_GMTL_MACHINE_H_) || defined(_GMTL_MACHINE_WINDOWS_INTEL64_H_)
#error Do not #include this internal file directly; use public TBB headers instead.
#endif

#ifndef _GMTL_MACHINE_WINDOWS_INTEL64_H_
#define _GMTL_MACHINE_WINDOWS_INTEL64_H_

#define __GMTL_WORDSIZE		8
#define __GMTL_BIG_ENDIAN	0

#include <intrin.h>

#if !__INTEL_COMPILER
    #pragma intrinsic(_InterlockedOr64)
    #pragma intrinsic(_InterlockedAnd64)
    #pragma intrinsic(_InterlockedCompareExchange)
    #pragma intrinsic(_InterlockedCompareExchange64)
    #pragma intrinsic(_InterlockedExchangeAdd)
    #pragma intrinsic(_InterlockedExchangeAdd64)
    #pragma intrinsic(_InterlockedExchange)
    #pragma intrinsic(_InterlockedExchange64)
#endif /* !defined(__INTEL_COMPILER) */

#if __INTEL_COMPILER
    #define __GMTL_compiler_fence()    __asm { __asm nop }
    #define __GMTL_full_memory_fence() __asm { __asm mfence }
#elif _MSC_VER >= 1300
    extern "C" void _ReadWriteBarrier();
    #pragma intrinsic(_ReadWriteBarrier)
    #pragma intrinsic(_mm_mfence)
    #define __GMTL_compiler_fence()    _ReadWriteBarrier()
    #define __GMTL_full_memory_fence() _mm_mfence()
#endif

#define __GMTL_control_consistency_helper() __GMTL_compiler_fence()
#define __GMTL_acquire_consistency_helper() __GMTL_compiler_fence()
#define __GMTL_release_consistency_helper() __GMTL_compiler_fence()

// ATTENTION: if you ever change argument types in machine-specific primitives,
// please take care of atomic_word<> specializations in tbb/atomic.h
extern "C" {
    __int8 __GMTL_EXPORTED_FUNC __GMTL_machine_cmpswp1 (volatile void *ptr, __int8 value, __int8 comparand );
    __int8 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchadd1 (volatile void *ptr, __int8 addend );
    __int8 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchstore1 (volatile void *ptr, __int8 value );
    __int16 __GMTL_EXPORTED_FUNC __GMTL_machine_cmpswp2 (volatile void *ptr, __int16 value, __int16 comparand );
    __int16 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchadd2 (volatile void *ptr, __int16 addend );
    __int16 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchstore2 (volatile void *ptr, __int16 value );
    void __GMTL_EXPORTED_FUNC __GMTL_machine_pause (__int32 delay );
}

inline long __GMTL_machine_cmpswp4 (volatile void *ptr, __int32 value, __int32 comparand ) {
    return _InterlockedCompareExchange( (long*)ptr, value, comparand );
}
inline long __GMTL_machine_fetchadd4 (volatile void *ptr, __int32 addend ) {
    return _InterlockedExchangeAdd( (long*)ptr, addend );
}
inline long __GMTL_machine_fetchstore4 (volatile void *ptr, __int32 value ) {
    return _InterlockedExchange( (long*)ptr, value );
}

inline __int64 __GMTL_machine_cmpswp8 (volatile void *ptr, __int64 value, __int64 comparand ) {
    return _InterlockedCompareExchange64( (__int64*)ptr, value, comparand );
}
inline __int64 __GMTL_machine_fetchadd8 (volatile void *ptr, __int64 addend ) {
    return _InterlockedExchangeAdd64( (__int64*)ptr, addend );
}
inline __int64 __GMTL_machine_fetchstore8 (volatile void *ptr, __int64 value ) {
    return _InterlockedExchange64( (__int64*)ptr, value );
}

#define __GMTL_USE_FETCHSTORE_AS_FULL_FENCED_STORE   1
#define __GMTL_USE_GENERIC_HALF_FENCED_LOAD_STORE    1
#define __GMTL_USE_GENERIC_RELAXED_LOAD_STORE        1

#if !__INTEL_COMPILER
extern "C" unsigned char _BitScanReverse64( unsigned long* i, unsigned __int64 w );
#pragma intrinsic(_BitScanReverse64)
#endif

inline __int64 __GMTL_machine_lg( unsigned __int64 i ) {
#if __INTEL_COMPILER
    unsigned __int64 j;
    __asm
    {
        bsr rax, i
        mov j, rax
    }
#else
    unsigned long j;
    _BitScanReverse64( &j, i );
#endif
    return j;
}

inline void __GMTL_machine_OR( volatile void *operand, intptr_t addend ) {
    _InterlockedOr64((__int64*)operand, addend); 
}

inline void __GMTL_machine_AND( volatile void *operand, intptr_t addend ) {
    _InterlockedAnd64((__int64*)operand, addend); 
}

#define __GMTL_AtomicOR(P,V) __GMTL_machine_OR(P,V)
#define __GMTL_AtomicAND(P,V) __GMTL_machine_AND(P,V)

extern "C" __declspec(dllimport) int __stdcall SwitchToThread( void );
#define __GMTL_Yield()  SwitchToThread()
#define __GMTL_Pause(V) __GMTL_machine_pause(V)
#define __GMTL_Log2(V)  __GMTL_machine_lg(V)

// API to retrieve/update FPU control setting
#define __GMTL_CPU_CTL_ENV_PRESENT 1

struct __GMTL_cpu_ctl_env_t {
    int     mxcsr;
    short   x87cw;
};

extern "C" {
    void __GMTL_EXPORTED_FUNC __GMTL_get_cpu_ctl_env ( __GMTL_cpu_ctl_env_t* );
    void __GMTL_EXPORTED_FUNC __GMTL_set_cpu_ctl_env ( const __GMTL_cpu_ctl_env_t* );
}

#endif  // _GMTL_MACHINE_WINDOWS_INTEL64_H_
