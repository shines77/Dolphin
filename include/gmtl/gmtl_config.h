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

#ifndef _GMTL_CONFIG_H_
#define _GMTL_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

/** This header is supposed to contain macro definitions and C style comments only.
    The macros defined here are intended to control such aspects of TBB build as
    - presence of compiler features
    - compilation modes
    - feature sets
    - known compiler/platform issues
**/

#define __GMTL_BUILD     1

#define __GMTL_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

/** Presence of compiler features **/

#if (__GMTL_GCC_VERSION >= 40400) && !defined(__INTEL_COMPILER)
    /** warning suppression pragmas available in GCC since 4.4 **/
    #define __GMTL_GCC_WARNING_SUPPRESSION_PRESENT 1
#endif

/* TODO: The following condition should be extended when new compilers/runtimes
         with std::exception_ptr support appear. */
#define __GMTL_EXCEPTION_PTR_PRESENT  ((_MSC_VER >= 1600 || (__GXX_EXPERIMENTAL_CXX0X__ && __GNUC__==4 && __GNUC_MINOR__>=4)) && !__INTEL_COMPILER)

#if __GNUC__ || __SUNPRO_CC || __IBMCPP__
    /* ICC defines __GNUC__ and so is covered */
    #define __GMTL_ATTRIBUTE_ALIGNED_PRESENT 1
#elif _MSC_VER && (_MSC_VER >= 1300 || __INTEL_COMPILER)
    #define __GMTL_DECLSPEC_ALIGN_PRESENT 1
#endif

#if (__GMTL_GCC_VERSION >= 40102) && !defined(__INTEL_COMPILER)
    /** built-in atomics available in GCC since 4.1.2 **/
    #define __GMTL_GCC_BUILTIN_ATOMICS_PRESENT 1
#endif

/** User controlled TBB features & modes **/

#ifndef MTL_USE_DEBUG
#ifdef MTL_DO_ASSERT
#define MTL_USE_DEBUG MTL_DO_ASSERT
#else
#ifdef _DEBUG
#define MTL_USE_DEBUG _DEBUG
#else
#define MTL_USE_DEBUG 0
#endif
#endif /* MTL_DO_ASSERT */
#endif /* MTL_USE_DEBUG */

#ifndef IMTL_USE_ASSERT
#ifdef MTL_DO_ASSERT
#define IMTL_USE_ASSERT MTL_DO_ASSERT
#else
#define IMTL_USE_ASSERT MTL_USE_DEBUG
#endif /* MTL_DO_ASSERT */
#endif /* MTL_USE_ASSERT */

#ifndef MTL_USE_THREADING_TOOLS
#ifdef MTL_DO_THREADING_TOOLS
#define MTL_USE_THREADING_TOOLS MTL_DO_THREADING_TOOLS
#else
#define MTL_USE_THREADING_TOOLS MTL_USE_DEBUG
#endif /* MTL_DO_THREADING_TOOLS */
#endif /* MTL_USE_THREADING_TOOLS */

#ifndef MTL_USE_PERFORMANCE_WARNINGS
#ifdef MTL_PERFORMANCE_WARNINGS
#define MTL_USE_PERFORMANCE_WARNINGS MTL_PERFORMANCE_WARNINGS
#else
#define MTL_USE_PERFORMANCE_WARNINGS MTL_USE_DEBUG
#endif /* MTL_PEFORMANCE_WARNINGS */
#endif /* MTL_USE_PERFORMANCE_WARNINGS */

#if !defined(__EXCEPTIONS) && !defined(_CPPUNWIND) && !defined(__SUNPRO_CC) || defined(_XBOX)
    #if MTL_USE_EXCEPTIONS
        #error Compilation settings do not support exception handling. Please do not set MTL_USE_EXCEPTIONS macro or set it to 0.
    #elif !defined(MTL_USE_EXCEPTIONS)
        #define MTL_USE_EXCEPTIONS 0
    #endif
#elif !defined(MTL_USE_EXCEPTIONS)
    #define MTL_USE_EXCEPTIONS 1
#endif

#ifndef MTL_IMPLEMENT_CPP0X
    /** By default, use C++0x classes if available **/
    #if __GNUC__==4 && __GNUC_MINOR__>=4 && __GXX_EXPERIMENTAL_CXX0X__
        #define MTL_IMPLEMENT_CPP0X 0
    #else
        #define MTL_IMPLEMENT_CPP0X 1
    #endif
#endif /* MTL_IMPLEMENT_CPP0X */

#ifndef MTL_USE_CAPTURED_EXCEPTION
    #if __GMTL_EXCEPTION_PTR_PRESENT
        #define MTL_USE_CAPTURED_EXCEPTION 0
    #else
        #define MTL_USE_CAPTURED_EXCEPTION 1
    #endif
#else /* defined MTL_USE_CAPTURED_EXCEPTION */
    #if !MTL_USE_CAPTURED_EXCEPTION && !__GMTL_EXCEPTION_PTR_PRESENT
        #error Current runtime does not support std::exception_ptr. Set MTL_USE_CAPTURED_EXCEPTION and make sure that your code is ready to catch mtl::captured_exception.
    #endif
#endif /* defined MTL_USE_CAPTURED_EXCEPTION */

/** Check whether the request to use GCC atomics can be satisfied **/
#if (MTL_USE_GCC_BUILTINS && !__GMTL_GCC_BUILTIN_ATOMICS_PRESENT)
    #error "GCC atomic built-ins are not supported."
#endif

/** Internal TBB features & modes **/

#ifndef __GMTL_DYNAMIC_LOAD_ENABLED
    #define __GMTL_DYNAMIC_LOAD_ENABLED !__GMTL_TASK_CPP_DIRECTLY_INCLUDED
#elif !__GMTL_DYNAMIC_LOAD_ENABLED
    #if _WIN32||_WIN64
        #define __GMTL_NO_IMPLICIT_LINKAGE 1
        #define __TBBMALLOC_NO_IMPLICIT_LINKAGE 1
    #else
        #define __GMTL_WEAK_SYMBOLS 1
    #endif
#endif

#ifndef __GMTL_COUNT_TASK_NODES
    #define __GMTL_COUNT_TASK_NODES IMTL_USE_ASSERT
#endif

#ifndef __GMTL_TASK_GROUP_CONTEXT
    #define __GMTL_TASK_GROUP_CONTEXT 1
#endif /* __GMTL_TASK_GROUP_CONTEXT */

#if MTL_USE_EXCEPTIONS && !__GMTL_TASK_GROUP_CONTEXT
    #error MTL_USE_EXCEPTIONS requires __GMTL_TASK_GROUP_CONTEXT to be enabled
#endif

#ifndef __GMTL_SCHEDULER_OBSERVER
    #define __GMTL_SCHEDULER_OBSERVER 1
#endif /* __GMTL_SCHEDULER_OBSERVER */

#ifndef __GMTL_TASK_PRIORITY
    #define __GMTL_TASK_PRIORITY __GMTL_TASK_GROUP_CONTEXT
#endif /* __GMTL_TASK_PRIORITY */

#if __GMTL_TASK_PRIORITY && !__GMTL_TASK_GROUP_CONTEXT
    #error __GMTL_TASK_PRIORITY requires __GMTL_TASK_GROUP_CONTEXT to be enabled
#endif

#if !defined(__GMTL_SURVIVE_THREAD_SWITCH) && (_WIN32 || _WIN64 || __linux__)
    #define __GMTL_SURVIVE_THREAD_SWITCH 1
#endif /* __GMTL_SURVIVE_THREAD_SWITCH */

#ifndef __GMTL_DEFAULT_PARTITIONER
#if MTL_DEPRECATED
/** Default partitioner for parallel loop templates in TBB 1.0-2.1 */
#define __GMTL_DEFAULT_PARTITIONER mtl::simple_partitioner
#else
/** Default partitioner for parallel loop templates since TBB 2.2 */
#define __GMTL_DEFAULT_PARTITIONER mtl::auto_partitioner
#endif /* MTL_DEPRECATED */
#endif /* !defined(__GMTL_DEFAULT_PARTITIONER */

/** Macros of the form __GMTL_XXX_BROKEN denote known issues that are caused by
    the bugs in compilers, standard or OS specific libraries. They should be
    removed as soon as the corresponding bugs are fixed or the buggy OS/compiler
    versions go out of the support list.
**/

#if __GNUC__ && __GMTL_x86_64 && __INTEL_COMPILER == 1200
    #define __GMTL_ICC_12_0_INL_ASM_FSTCW_BROKEN 1
#endif

#if _MSC_VER && __INTEL_COMPILER && (__INTEL_COMPILER<1110 || __INTEL_COMPILER==1110 && __INTEL_COMPILER_BUILD_DATE < 20091012)
    /** Necessary to avoid ICL error (or warning in non-strict mode):
        "exception specification for implicitly declared virtual destructor is
        incompatible with that of overridden one". **/
    #define __GMTL_DEFAULT_DTOR_THROW_SPEC_BROKEN 1
#endif

#if defined(_MSC_VER) && _MSC_VER < 1500 && !defined(__INTEL_COMPILER)
    /** VS2005 and earlier do not allow declaring template class as a friend
        of classes defined in other namespaces. **/
    #define __GMTL_TEMPLATE_FRIENDS_BROKEN 1
#endif

#if __GLIBC__==2 && __GLIBC_MINOR__==3 || __MINGW32__ || (__APPLE__ && __INTEL_COMPILER==1200 && !MTL_USE_DEBUG)
    //! Macro controlling EH usages in TBB tests
    /** Some older versions of glibc crash when exception handling happens concurrently. **/
    #define __GMTL_THROW_ACROSS_MODULE_BOUNDARY_BROKEN 1
#endif

#if (_WIN32||_WIN64) && __INTEL_COMPILER == 1110
    /** That's a bug in Intel compiler 11.1.044/IA-32/Windows, that leads to a worker thread crash on the thread's startup. **/
    #define __GMTL_ICL_11_1_CODE_GEN_BROKEN 1
#endif

#if __GNUC__==3 && __GNUC_MINOR__==3 && !defined(__INTEL_COMPILER)
    /** A bug in GCC 3.3 with access to nested classes declared in protected area */
    #define __GMTL_GCC_3_3_PROTECTED_BROKEN 1
#endif

#if __MINGW32__ && (__GNUC__<4 || __GNUC__==4 && __GNUC_MINOR__<2)
    /** MinGW has a bug with stack alignment for routines invoked from MS RTLs.
        Since GCC 4.2, the bug can be worked around via a special attribute. **/
    #define __GMTL_SSE_STACK_ALIGNMENT_BROKEN 1
#endif

#if __GNUC__==4 && __GNUC_MINOR__==3 && __GNUC_PATCHLEVEL__==0
    // GCC of this version may rashly ignore control dependencies
    #define __GMTL_GCC_OPTIMIZER_ORDERING_BROKEN 1
#endif

#if __FreeBSD__
    /** A bug in FreeBSD 8.0 results in kernel panic when there is contention
        on a mutex created with this attribute. **/
    #define __GMTL_PRIO_INHERIT_BROKEN 1

    /** A bug in FreeBSD 8.0 results in test hanging when an exception occurs
        during (concurrent?) object construction by means of placement new operator. **/
    #define __GMTL_PLACEMENT_NEW_EXCEPTION_SAFETY_BROKEN 1
#endif /* __FreeBSD__ */

#if (__linux__ || __APPLE__) && __i386__ && defined(__INTEL_COMPILER)
    /** The Intel compiler for IA-32 (Linux|Mac OS X) crashes or generates
        incorrect code when __asm__ arguments have a cast to volatile. **/
    #define __GMTL_ICC_ASM_VOLATILE_BROKEN 1
#endif

#if !__INTEL_COMPILER && (_MSC_VER || __GNUC__==3 && __GNUC_MINOR__<=2)
    /** Bug in GCC 3.2 and MSVC compilers that sometimes return 0 for __alignof(T)
        when T has not yet been instantiated. **/
    #define __GMTL_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN 1
#endif

#endif  /* _GMTL_CONFIG_H_ */
