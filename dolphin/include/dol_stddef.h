
#ifndef _DOL_STDDEF_H_
#define _DOL_STDDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

// Platform Initialization ==================================================

#if defined( _MSC_VER ) || defined( __MINGW__ ) || defined( _WINDOWS ) || defined( _WIN32 )
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  define _CRT_SECURE_NO_WARNINGS
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

#if defined( _MSC_VER )
#  include "./vs/stdint.h"
#  define snprintf _snprintf
#else
#  include <stdint.h>
#endif

#include "colour.h"
#include "board.h"

#if defined(__GNUC__)
#  define likely(x)     __builtin_expect((x),1)
#  define unlikely(x)   __builtin_expect((x),0)
#else
#  define likely(x)     (x)
#  define unlikely(x)   (x)
#endif

/* Define function attributes directive when available */
#if __GNUC__ >= 3
#define	REGPARM(num)	__attribute__((regparm(num)))
#else
#if defined (_MSC_VER) || defined(__BORLANDC__)
#define	REGPARM(num)	__fastcall
#else
#define	REGPARM(num)
#endif
#endif

// We do not need defines below for resource processing on windows
#ifndef RC_INVOKED

#define __MY_ASSERT( x, ... )

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

#endif  // _DOL_STDDEF_H_
