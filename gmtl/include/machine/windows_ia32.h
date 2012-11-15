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

#if !defined(_GMTL_MACHINE_H_) || defined(_GMTL_MACHINE_WINDOWS_IA32_H_)
#error Do not #include this internal file directly; use public IMTL headers instead.
#endif

#ifndef _GMTL_MACHINE_WINDOWS_IA32_H_
#define _GMTL_MACHINE_WINDOWS_IA32_H_

#define __GMTL_WORDSIZE      4
#define __GMTL_BIG_ENDIAN    0

#if __INTEL_COMPILER
    #define __GMTL_compiler_fence() __asm { __asm nop }
#elif _MSC_VER >= 1300
    extern "C" void _ReadWriteBarrier();
    #pragma intrinsic(_ReadWriteBarrier)
    #define __GMTL_compiler_fence() _ReadWriteBarrier()
#else
    #error Unsupported compiler - need to define __GMTL_{control,acquire,release}_consistency_helper to support it
#endif

#define __GMTL_control_consistency_helper() __GMTL_compiler_fence()
#define __GMTL_acquire_consistency_helper() __GMTL_compiler_fence()
#define __GMTL_release_consistency_helper() __GMTL_compiler_fence()
#define __GMTL_full_memory_fence()          __asm { __asm mfence }

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    // Workaround for overzealous compiler warnings in /Wp64 mode
    #pragma warning (push)
    #pragma warning (disable: 4244 4267)
#endif

extern "C" {
    __int64 __GMTL_EXPORTED_FUNC __GMTL_machine_cmpswp8 (volatile void *ptr, __int64 value, __int64 comparand );
    __int64 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchadd8 (volatile void *ptr, __int64 addend );
    __int64 __GMTL_EXPORTED_FUNC __GMTL_machine_fetchstore8 (volatile void *ptr, __int64 value );
    void __GMTL_EXPORTED_FUNC __GMTL_machine_store8 (volatile void *ptr, __int64 value );
    __int64 __GMTL_EXPORTED_FUNC __GMTL_machine_load8 (const volatile void *ptr);
}

#define __GMTL_MACHINE_DEFINE_ATOMICS(S,T,U,A,C) \
static inline T __GMTL_machine_cmpswp##S ( volatile void * ptr, U value, U comparand ) { \
    T result; \
    volatile T *p = (T *)ptr; \
    __asm \
    { \
       __asm mov edx, p \
       __asm mov C , value \
       __asm mov A , comparand \
       __asm lock cmpxchg [edx], C \
       __asm mov result, A \
    } \
    return result; \
} \
\
static inline T __GMTL_machine_fetchadd##S ( volatile void * ptr, U addend ) { \
    T result; \
    volatile T *p = (T *)ptr; \
    __asm \
    { \
        __asm mov edx, p \
        __asm mov A, addend \
        __asm lock xadd [edx], A \
        __asm mov result, A \
    } \
    return result; \
}\
\
static inline T __GMTL_machine_fetchstore##S ( volatile void * ptr, U value ) { \
    T result; \
    volatile T *p = (T *)ptr; \
    __asm \
    { \
        __asm mov edx, p \
        __asm mov A, value \
        __asm lock xchg [edx], A \
        __asm mov result, A \
    } \
    return result; \
}


__GMTL_MACHINE_DEFINE_ATOMICS(1, __int8, __int8, al, cl)
__GMTL_MACHINE_DEFINE_ATOMICS(2, __int16, __int16, ax, cx)
__GMTL_MACHINE_DEFINE_ATOMICS(4, ptrdiff_t, ptrdiff_t, eax, ecx)

#undef __GMTL_MACHINE_DEFINE_ATOMICS

static inline __int32 __GMTL_machine_lg( unsigned __int64 i ) {
    unsigned __int32 j;
    __asm
    {
        bsr eax, i
        mov j, eax
    }
    return j;
}

static inline void __GMTL_machine_OR( volatile void *operand, __int32 addend ) {
   __asm 
   {
       mov eax, addend
       mov edx, [operand]
       lock or [edx], eax
   }
}

static inline void __GMTL_machine_AND( volatile void *operand, __int32 addend ) {
   __asm 
   {
       mov eax, addend
       mov edx, [operand]
       lock and [edx], eax
   }
}

static inline void __GMTL_machine_pause (__int32 delay ) {
    _asm 
    {
        mov eax, delay
      L1: 
        pause
        add eax, -1
        jne L1  
    }
    return;
}

#define __GMTL_AtomicOR(P,V) __GMTL_machine_OR(P,V)
#define __GMTL_AtomicAND(P,V) __GMTL_machine_AND(P,V)

#define __GMTL_USE_FETCHSTORE_AS_FULL_FENCED_STORE   1
#define __GMTL_USE_GENERIC_HALF_FENCED_LOAD_STORE    1
#define __GMTL_USE_GENERIC_RELAXED_LOAD_STORE        1

// Definition of other functions
extern "C" __declspec(dllimport) int __stdcall SwitchToThread( void );
#define __GMTL_Yield()  SwitchToThread()
#define __GMTL_Pause(V) __GMTL_machine_pause(V)
#define __GMTL_Log2(V)  __GMTL_machine_lg(V)

#if defined(_MSC_VER)&&_MSC_VER<1400
    static inline void* __GMTL_machine_get_current_teb () {
        void* pteb;
        __asm mov eax, fs:[0x18]
        __asm mov pteb, eax
        return pteb;
    }
#endif

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #pragma warning (pop)
#endif // warnings 4244, 4267 are back

// API to retrieve/update FPU control setting
#define __GMTL_CPU_CTL_ENV_PRESENT 1

struct __GMTL_cpu_ctl_env_t {
    int     mxcsr;
    short   x87cw;
};
inline void __GMTL_get_cpu_ctl_env ( __GMTL_cpu_ctl_env_t* ctl ) {
    __asm {
        __asm mov     eax, ctl
        __asm stmxcsr [eax]
        __asm fstcw   [eax+4]
    }
}
inline void __GMTL_set_cpu_ctl_env ( const __GMTL_cpu_ctl_env_t* ctl ) {
    __asm {
        __asm mov     eax, ctl
        __asm ldmxcsr [eax]
        __asm fldcw   [eax+4]
    }
}

#endif  // _GMTL_MACHINE_WINDOWS_IA32_H_
