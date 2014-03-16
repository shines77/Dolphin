
#ifndef _GMTL_STDDEF_H_
#define _GMTL_STDDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

// Platform Initialization ==================================================

#if defined( _MSC_VER ) || defined( __MINGW__ ) || defined( _WINDOWS ) || defined( WIN32 )
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  ifndef _CRT_SECURE_NO_WARNINGS
#     define _CRT_SECURE_NO_WARNINGS
#  endif
#  define DIRECTORY_DELIMITER "\\"
#  define SIGACTION 0
#else
#  define DIRECTORY_DELIMITER "/"
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

// Define preprocessor symbols used to determine architecture
#if _WIN32||_WIN64
#   if defined(_M_X64)||defined(__x86_64__)  // the latter for MinGW support
#       define __GMTL_x86_64 1
#   elif defined(_M_IA64)
#       define __GMTL_ipf 1
#   elif defined(_M_IX86)||defined(__i386__) // the latter for MinGW support
#       define __GMTL_x86_32 1
#   endif
#else /* Assume generic Unix */
#   if !__linux__ && !__APPLE__
#       define __GMTL_generic_os 1
#   endif
#   if __x86_64__
#       define __GMTL_x86_64 1
#   elif __ia64__
#       define __GMTL_ipf 1
#   elif __i386__||__i386  // __i386 is for Sun OS
#       define __GMTL_x86_32 1
#   else
#       define __GMTL_generic_arch 1
#   endif
#endif

// mtl_config.h should be included the first since it contains macro definitions used in other headers
#include <gmtl/gmtl_config.h>

#if _MSC_VER >=1400
    #define __GMTL_EXPORTED_FUNC   __cdecl
    #define __GMTL_EXPORTED_METHOD __thiscall
#else
    #define __GMTL_EXPORTED_FUNC
    #define __GMTL_EXPORTED_METHOD
#endif

#if defined( _MSC_VER )
#   include <gmtl/vs/stdint.h>
#   define snprintf _snprintf
#else
#   include <stdint.h>
#endif

#if _MSC_VER
    #define _GMTL_imtl_windef_H_
    #include <gmtl/internal/_gmtl_windef.h>
    #undef  _GMTL_imtl_windef_H_
#endif

#if defined(__GNUC__)
#   define likely(x)     __builtin_expect((x),1)
#   define unlikely(x)   __builtin_expect((x),0)
#else
#   define likely(x)     (x)
#   define unlikely(x)   (x)
#endif

// We do not need defines below for resource processing on windows
#ifndef RC_INVOKED

#define GMTL_ASSERT( x, ... )

//! The namespace dolphin contains all components of the library.

namespace gmtl {

namespace internal {

/** Label for data that may be accessed from different threads, and that may eventually become wrapped
    in a formal atomic type.

    Note that no problems have yet been observed relating to the definition currently being empty,
    even if at least "volatile" would seem to be in order to avoid data sometimes temporarily hiding
    in a register (although "volatile" as a "poor man's atomic" lacks several other features of a proper
    atomic, some of which are now provided instead through specialized functions).

    Note that usage is intentionally compatible with a definition as qualifier "volatile",
    both as a way to have the compiler help enforce use of the label and to quickly rule out
    one potential issue.

    Note however that, with some architecture/compiler combinations, e.g. on Itanium, "volatile"
    also has non-portable memory semantics that are needlessly expensive for "relaxed" operations.

    Note that this must only be applied to data that will not change bit patterns when cast to/from
    an integral type of the same length; tbb::atomic must be used instead for, e.g., floating-point types.

    TODO: apply wherever relevant **/
#define __GMTL_atomic // intentionally empty, see above

#if IMTL_USE_ASSERT
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

}  // namespace gmtl

#endif  // RC_INVOKED

#endif  /* _GMTL_STDDEF_H_ */
