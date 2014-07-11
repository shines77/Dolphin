
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <mm3dnow.h>
#include <windows.h>

#include "ms1b.h"

// ²Î¿¼:
//
// http://www.gamedev.net/topic/229831-nearest-power-of-2/page-2
//

#if _DEBUG
const unsigned times = 1000000;
#else
const unsigned times = 10000000;
#endif

static __int64 s_qpc_freq = 0;

// Visual C
#define inline __forceinline

unsigned rdtsc(void)
{
    unsigned timer;
    #if defined(_MSC_VER)
        /* Visual C++ inline assembly code */
        __asm {
            rdtsc
            mov timer, eax
        };
    #elif defined(__GNUC__)
        /* GCC inline assembly code */
        register unsigned lo asm("eax"), hi asm("edx");
        __asm volatile (".byte 15,49" : : : "eax", "edx");
        hi;
        timer = lo;
    #endif
    return timer;
}

#define nInnerLoops 128
#define nOuterLoops 128
static unsigned pArray[nInnerLoops];

#define ms1b_test(F, N) {                       \
    int i, j, k = 0;                            \
    unsigned tmin = nInnerLoops * 100000UL;     \
    float timing;                               \
    int *p = (int *)pArray;                     \
    for (j = 0; j < nOuterLoops; j++) {         \
        unsigned t = rdtsc();                   \
        for (i = 0; i < nInnerLoops; i += 4) {  \
            k += F(p[i + 0]);                   \
            k += F(p[i + 1]);                   \
            k += F(p[i + 2]);                   \
            k += F(p[i + 3]);                   \
        }                                       \
        t = rdtsc() - t;                        \
        tmin = (t < tmin) ? t : tmin;           \
    }                                           \
    timing = (float)tmin / (float)nInnerLoops;  \
    printf("%-20s : F(%d) = %d,\t%0.3f tick\t%d\n", #F, N, F(N), timing, k); \
}

// Naive version
inline unsigned nextpow2_Naive(unsigned n)
{
    unsigned x;
    if (n == 0) return 0;
    x = 1;
    while (x < n) x <<= 1;
    return x;
}

// Shifts
inline unsigned nextpow2_Logical(unsigned x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

inline unsigned nextpow2_Recursive(unsigned x)
{
    unsigned y = ((x - 1) & x);
    return y ? nextpow2_Recursive(y) : (x << 1);
}

//inline int __fastcall _ftoi(float x)
inline int __stdcall _ftoi(float x)
{
    int r;
    __asm fld x
    __asm fistp dword ptr r;
    return r;
}

// IEEE trick
inline unsigned nextpow2_IEEE(unsigned x)
{
    static const unsigned _MantissaMask = (1UL << 23UL) - 1UL;
    unsigned irep;
    *(float *)&irep = (float)x;
    irep = (irep + _MantissaMask) & ~_MantissaMask;
    x = (unsigned)_ftoi(*(float *)&irep);
    return x;
}

/*
__declspec(naked)
int __fastcall nextpow2_BSR(int x)
{
    __asm
    {
        dec ecx
        mov eax, 2
        bsr ecx, ecx
        rol eax, cl
        ret
    }
}
*/

//  Find a way for pure inline asm ???
inline unsigned nextpow2_BSR(unsigned x)
{
    unsigned shift;
    __asm {
        mov ecx, x
        dec ecx
        //mov eax, 2

        bsr ecx, ecx
        mov shift, ecx
        //rol eax, cl
    }
    return (2 << shift);
}

inline unsigned nextpow2_3DNow(unsigned x)
{
    static const __m64 _MantissaMask64 = {0x00000000007FFFFFL};
    register int y;
    register __m64 t;
    _m_femms();
    t = _mm_cvtsi32_si64(x);
    t = _m_pi2fd(t);
    t = _m_paddd(t, _MantissaMask64);
    t = _m_pandn(_MantissaMask64, t);
    t = _m_pf2id(t);
    y = _mm_cvtsi64_si32(t);
    _m_femms();

    __asm {
        emms;
    }
    return y;
}

unsigned ms1b_test2(ms1b_func f)
{
    __int64 before;
    __int64 after;

    srand(345678UL);

    QueryPerformanceCounter((LARGE_INTEGER *)&before);

    for (int i = 0; i < times; i++) {
        f(rand());
    }

    QueryPerformanceCounter((LARGE_INTEGER *)&after);

    __int64 diff = after - before;
    __int64 milliseconds;
    if (s_qpc_freq != 0)
        milliseconds = diff * 1000 / s_qpc_freq;
    else
        milliseconds = 0;
    return (unsigned) milliseconds;
}

int ms1b_main(int argc, char* argv[])
{
    QueryPerformanceFrequency((LARGE_INTEGER *)&s_qpc_freq);

    /* Init array */
    {
        int i;
        for (i = 0; i < nInnerLoops; i++)
            pArray[i] = rand();
    }

    printf("ftoi(1.1) = %d \n", _ftoi(1.1f));

    ms1b_test(nextpow2_Naive, 865);

    ms1b_test(nextpow2_Logical, 865);
    ms1b_test(nextpow2_Recursive, 865);
    ms1b_test(nextpow2_IEEE, 865);
    ms1b_test(nextpow2_BSR, 865);
    //ms1b_test(nextpow2_3DNow, 865);

    printf("\n");

    //getch();
    system("pause");

    return 0;
}
