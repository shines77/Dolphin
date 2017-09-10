
#ifndef _DOL_STDDEF_H_
#define _DOL_STDDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

// Platform Initialization ==================================================

#if defined( _MSC_VER ) || defined( __MINGW__ ) || defined( _WINDOWS ) || defined( _WIN32 )
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  ifndef _CRT_SECURE_NO_WARNINGS
#     define _CRT_SECURE_NO_WARNINGS
#  endif
#  define DIRECTORY_DELIMITER   "\\"
#  define SIGACTION 0
#else
#  define DIRECTORY_DELIMITER   "/"
#  define SIGACTION 1
#endif

#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef NULL
    #define NULL  0
#endif

// Switching of using 'const'-flag on methods possible ======================
// Results of the simulator should always be the same, no matter if this is set to:
// #define CONST_T const    -OR-
// #define CONST_T
#ifndef CONST_T
#define CONST_T const
#endif

#if defined( _MSC_VER )
#  include <dolphin/vs/stdint.h>
//#  define snprintf _snprintf
#else
#  include <stdint.h>
#endif

//#include <dolphin/colour.h>
//#include <dolphin/board.h>

#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#endif

#if defined(__GNUC__)
#  define likely(x)     __builtin_expect((x),1)
#  define unlikely(x)   __builtin_expect((x),0)
#else
#  define likely(x)     (x)
#  define unlikely(x)   (x)
#endif

////////////////////////////////////////////////////////
// Defined whether use memory attribute aligned.
////////////////////////////////////////////////////////

#define USE_ATTR_ALIGNED        1

#if defined(USE_ATTR_ALIGNED) && (USE_ATTR_ALIGNED != 0)
#   if defined(__GNUC__)
#       define DECLARE_ALIGNED(x)      __attribute__((aligned(x)))
#   elif defined(_WIN32) || defined(_MSC_VER)
#       define DECLARE_ALIGNED(x)      __declspec(align(x))
#   else
#       define DECLARE_ALIGNED(x)
#   endif
#else
#   define DECLARE_ALIGNED(x)
#endif

// Statement: int __declspec(align(N)) X; support from MS VC++ 7.1 (_MSC_VER >= 1310)
// or Intel C++ Compile (ICC) Version > 8.1

#ifdef ALIGN_PREFIX
#undef ALIGN_PREFIX
#endif

#if defined(USE_ATTR_ALIGNED) && (USE_ATTR_ALIGNED != 0)
#   if defined(__ICL) && (defined(__VERSION__) || defined(__INTEL_COMPILER_BUILD_DATE))
#       define ALIGN_PREFIX(N)      __declspec(align(N))
#   elif defined(_MSC_VER) && (_MSC_VER >= 1310)
#       define ALIGN_PREFIX(N)      __declspec(align(N))
#   else
#       define ALIGN_PREFIX(N)
#   endif
#else
#   define ALIGN_PREFIX(N)
#endif

// Statement: int X __attribute__((aligned(N))); support by gcc (GNU C)

#ifdef ALIGN_SUFFIX
#undef ALIGN_SUFFIX
#endif

#if defined(USE_ATTR_ALIGNED) && (USE_ATTR_ALIGNED != 0)
#   if defined(__GNUC__)
#       define ALIGN_SUFFIX(N)      __attribute__((aligned(N)))
#   else
#       define ALIGN_SUFFIX(N)
#   endif
#else
#   define ALIGN_SUFFIX(N)
#endif

#define ALIGN_PREFIX_8          ALIGN_PREFIX(8)
#define ALIGN_PREFIX_16         ALIGN_PREFIX(16)
#define ALIGN_PREFIX_32         ALIGN_PREFIX(32)
#define ALIGN_PREFIX_64         ALIGN_PREFIX(64)

#define ALIGN_SUFFIX_8          ALIGN_SUFFIX(8)
#define ALIGN_SUFFIX_16         ALIGN_SUFFIX(16)
#define ALIGN_SUFFIX_32         ALIGN_SUFFIX(32)
#define ALIGN_SUFFIX_64         ALIGN_SUFFIX(64)

// address align
#define ADDR_ALGIN_8BYTES(p)    (((unsigned)(p) +  7) & 0XFFFFFFF8UL)
#define ADDR_ALGIN_16BYTES(p)   (((unsigned)(p) + 15) & 0XFFFFFFF0UL)
#define ADDR_ALGIN_32BYTES(p)   (((unsigned)(p) + 31) & 0XFFFFFFE0UL)
#define ADDR_ALGIN_64BYTES(p)   (((unsigned)(p) + 63) & 0XFFFFFFC0UL)

/* Define function attributes directive when available */
#if __GNUC__ >= 3
#define __FASTCALL(num)         __attribute__((regparm(num)))
#else
#if defined (_MSC_VER) || defined(__BORLANDC__)
#define __FASTCALL(num)         __fastcall
#else
#define __FASTCALL(num)
#endif
#endif

////////////////////////////////////////////////////////

// We do not need defines below for resource processing on windows
#ifndef RC_INVOKED

#define DOL_ASSERT_FALSE( x, ... )
#define DOL_ASSERT_TRUE ( x, ... )

#define DOL_ASSERT      DOL_ASSERT_FALSE

//! The namespace dolphin contains all components of the library.

namespace dolphin {

namespace internal {

#if DOL_USE_ASSERT
static void* const poisoned_ptr = reinterpret_cast<void*>(-1);

//! Set p to invalid pointer value.
template<typename T>
inline void poison_pointer( T*& p ) { p = reinterpret_cast<T*>(poisoned_ptr); }

/** Expected to be used in assertions only, thus no empty form is defined. **/
template<typename T>
inline bool is_poisoned( T* p ) { return p == reinterpret_cast<T*>(poisoned_ptr); }
#else
template<typename T>
inline void poison_pointer( T* ) {/*do nothing*/}
#endif /* !TBB_USE_ASSERT */

//! Cast pointer from U* to T.
/** This method should be used sparingly as a last resort for dealing with
    situations that inherently break strict ISO C++ aliasing rules. */
template<typename T, typename U>
inline T punned_cast( U* ptr ) {
    uintptr_t x = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<T>(x);
}

//! Base class for types that should not be assigned.
class no_assign {
    // Deny assignment
    void operator=( const no_assign& );
#if __GNUC__
public:
    //! Explicitly define default construction, because otherwise gcc issues gratuitous warning.
    no_assign() {}
#endif /* __GNUC__ */
};

//! Base class for types that should not be copied or assigned.
class no_copy: no_assign {
    //! Deny copy construction
    no_copy( const no_copy& );
public:
    //! Allow default construction
    no_copy() {}
};

}  // namespace ::internal

}  // namespace dolphin

#endif  // RC_INVOKED

#endif  /* _DOL_STDDEF_H_ */
