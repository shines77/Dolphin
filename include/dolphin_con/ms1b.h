
#ifndef _DOLPHIN_MS1B_H_
#define _DOLPHIN_MS1B_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned (* ms1b_func)(unsigned);
typedef unsigned (__fastcall * ms1b2_func)(unsigned);

unsigned ms1b_test2(ms1b_func f);
unsigned ms1b2_test(ms1b2_func f);

// some declear
int ms1b_main(int argc, char* argv[]);

int ms1b2_main(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

namespace dolphin_console {

// some functions

}  // namespace dolphin_console

#endif // _DOLPHIN_MS1B_H_
