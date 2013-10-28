
#ifndef _DOL_HASH_MASKE_H_
#define _DOL_HASH_MASKE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <dolphin/dol_stddef.h>

namespace dolphin {

unsigned int popcount32( unsigned int num );

unsigned int popcount64( unsigned int high, unsigned int low );

int GetMaxEntry(int n, int p);

unsigned int HashMask_GetMaxLength(int n, int p1, int p2, uint64 start = 0);

}  // namespace dolphin

#endif  /* _DOL_HASH_MASKE_H_ */
