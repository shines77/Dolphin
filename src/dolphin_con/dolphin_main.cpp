
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <objbase.h>
#include <gmtl/gmtl.h>
#include <dolphin/dolphin.h>
#include <mmsystem.h>

#include "ms1b.h"
#include "dolphin_main.h"

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#define ReadTSC( x )            \
    __asm cpuid                 \
    __asm rdtsc                 \
    __asm mov dword ptr x, eax  \
    __asm mov dword ptr x + 4, edx

//////////////////////////////////////////////////////////////////////////
// From:  http://software.intel.com/en-us/forums/showthread.php?t=48371
//////////////////////////////////////////////////////////////////////////

#define LOOP_COUNT      160000
#define REPEAT_25(x)    x x x x x x x x x x x x x x x x x x x x x x x x x
#define REPEAT_100(x)   REPEAT_25(x) REPEAT_25(x) REPEAT_25(x) REPEAT_25(x)
#define REPEAT_1000(x)  REPEAT_100(x) REPEAT_100(x) REPEAT_100(x) REPEAT_100(x) REPEAT_100(x) \
    REPEAT_100(x) REPEAT_100(x) REPEAT_100(x) REPEAT_100(x) REPEAT_100(x)
#define FACTOR          ((double)LOOP_COUNT*1000.0)

#define CLOCKSPERINSTRUCTION(start,end) ((double)end-(double)start)/(FACTOR)

using namespace gmtl;
using namespace dolphin;
using namespace std;

interface abs_math;

void asm_pause_test(void)
{
    __int64 start, end, total;
    total = 0;
#if !_WIN64
    ReadTSC(start);
    for (int i = 0; i < LOOP_COUNT; i++) {
        REPEAT_1000(__asm { nop };)
        if ((i % 500) == 0) {
            //SwitchToThread();
            //Sleep(1);
        }
    }
    ReadTSC(end);
    total = end - start;

    printf("nop:   clocks per instruction %4.2f\n", (double)total / (double)FACTOR);

    ReadTSC(start);
    total = 0;;
    for (int i = 0; i < LOOP_COUNT; i++) {
        REPEAT_1000(__asm { pause };)
        if ((i % 500) == 0) {
            //SwitchToThread();
            //Sleep(1);
        }
    }
    ReadTSC(end);
#else
    start = 0;
    end = 0;
#endif /* !_WIN64 */
    total = end - start;

    printf("pause: clocks per instruction %4.2f\n", (double)total / (double)FACTOR);
}

// 辗转相减法, 简单高效, 清晰快捷, 无除法运算, 要求m, n不能为负数
int GetMaxCommonDivide(unsigned m, unsigned n)
{
    while (m != n) {
        if (m == 0)
            return n;
        if (n == 0)
            return m;
        if (m > n) {
            m = m - n;
        }
        else {
            n = n - m;
        }
    }

    return m;
}

//
// 求GCD(最大公约数MaxCommonDenominator)的stein算法(递归版), 要求m, n不能为负数
// The maximum common denominator algorithm
//
// 可参阅: http://www.cnblogs.com/drizzlecrj/archive/2007/09/14/892340.html
//         http://blog.163.com/zhoumhan_0351/blog/static/3995422720097236954783/
//         http://www.baike.com/wiki/Stein%E7%AE%97%E6%B3%95
//
int gcd_stein(unsigned m, unsigned n)
{
    if (m == 0)
        return n;
    if (n == 0)
        return m;

    if (m == n)
        return m;

    if (((m & 1) & (n & 1)) == 0) {     // 若m, n均为偶数
       return gcd_stein(m >> 1, n >> 1) << 1;
    }
    else if ((m & 1) == 0) {            // 若m为偶数, n为奇数
        return gcd_stein(m >> 1, n);
    }
    else if ((n & 1) == 0) {            // 若m为奇数, n为偶数
        return gcd_stein(m, n >> 1);
    }
    else {  
        // m, n均为奇数, 返回gcd_stein(abs(m - n) / 2, min(m, n));
        if (m > n) {
            return gcd_stein((m - n) >> 1, n);
        }
        else {
            return gcd_stein((n - m) >> 1, m); // or return gcd_stein(m, (n - m) >> 1);
        }
    }
}

//
// 求GCD(最大公约数MaxCommonDenominator)的stein算法(非递归fast版), 要求m, n不能为负数
// The maximum common denominator algorithm
//
// 可参阅: http://www.cnblogs.com/drizzlecrj/archive/2007/09/14/892340.html
//         http://blog.163.com/zhoumhan_0351/blog/static/3995422720097236954783/
//         http://www.baike.com/wiki/Stein%E7%AE%97%E6%B3%95
//
int gcd_stein_fast(unsigned m, unsigned n)
{
    unsigned int s = 0;

    if (m == 0)
        return n;
    if (n == 0)
        return m;

    while (m != n) {
        if (((m & 1) & (n & 1)) == 0) {     // 若m, n均为偶数
           // return gcd_stein_fast(m >> 1, n >> 1) << 1;
           m >>= 1; n >>= 1; ++s;
           while ((((m & 1) & (n & 1)) == 0)) {
               m >>= 1; n >>= 1; ++s;
           }
        }
        else if ((m & 1) == 0) {            // 若m为偶数, n为奇数
            // return gcd_stein_fast(m >> 1, n);
            m >>= 1;
            while ((m & 1) == 0)
                m >>= 1;
            if (m == 1)
                return m << s;
        }
        else if ((n & 1) == 0) {            // 若m为奇数, n为偶数
            // return gcd_stein_fast(m, n >> 1);
            n >>= 1;
            while ((n & 1) == 0)
                n >>= 1;
            if (n == 1)
                return n << s;
        }
        else {
            if (m == 1 || n == 1)
                return 1 << s;
            do {
                // m, n均为奇数, 返回gcd_stein(abs(m - n) / 2, min(m, n));
                if (m > n) {
                    // return gcd_stein_fast((m - n)/2, n);
                    m = (m - n) >> 1;
                    while ((m & 1) == 0)
                        m >>= 1;
                    if (m == 1)
                        return m << s;
                }
                else {
                    // return gcd_stein_fast((n - m)/2, m); // or return gcd_stein_fast(m, (n - m)/2);
                    n = (n - m) >> 1;
                    while ((n & 1) == 0)
                        n >>= 1;
                    if (n == 1)
                        return n << s;
                }
            } while (m != n);
        }
    }

    return m << s;
}

// _tmain() 必须包含 tchar.h, main()的Unicode版本
int _tmain(int argc, _TCHAR *argv[])
{
#if _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    my_random::srand();
    sys_random::srand();

    for (int i = 0; i < 2; i++) {
        //asm_pause_test();
    }

    int gcd;
    gcd = GetMaxCommonDivide(7, 5);
    printf("GCD(7, 5) = %d\n\n", gcd);

    ms1b_main();

    ms1b2_main(0, NULL);

#if 0
    int m, n, p, p1, p2;
    n = 20; p = 2;
    p1 = 2; p2 = 4;
    //m = GetMaxEntry(n, p);
    //printf("GetMaxEntry(%d, %d) = %d\n\n", n, p, m);

    m = HashMask_GetMaxLength(n, p1, p2, 5);
    //m = HashMask_GetMaxLength(n, p, (1 << 11) - 1);
    printf("HashMask_GetMaxLength(%d, %d, %d) = %d\n\n", n, p1, p2, m);
#endif

    //system("pause");
    //return 0;

    /*
    MMRESULT mresult = timeBeginPeriod(1);
    if (TIMERR_NOERROR == mresult) {
        printf("timeBeginPeriod: TIMERR_NOERROR.\n");
    }
    //*/

    tickcount t1, t2, tnow;
    itimer it1, it2;
    tickcount::interval interval1, interval2;
    itimer::interval interval3, interval4;
    t1.begin();
    it1.begin();
    Sleep(500);
    t2.begin();
    it2.begin();
    interval1 = t2 - t1;
    tnow = tickcount::now();
    interval1 = tnow - t1;
    interval1 = t1.end();
    interval3 = it1.end();

    double f_interval = interval1.seconds();

    void *pbuffer = _aligned_malloc(31 + 24, 32);
    char *pchar = (char *)pbuffer;
    pchar[54] = 0;

    GxString str1, str2;
    str1.set_length(13, 'A');
    str2 = str1;

    cache_aligned cache_align, c1, c2;
    cache_align.set_align_size(32);
    cache_align.malloc_mem(31 + 24);
    c1 = c2 = cache_align;
    //c1 = c2 = 2;

    task task;
    task.set_depth(0);
    task.spawn();
    task::spawn(task);
    //task_base_t::spawn(task);

    task_scheduler task_scheduler(task_scheduler::MTL_AUTOMATIC);
    task_scheduler.initialize(4);

    bitboard::init_square_mask();
    bitboard bb, bb2;
    bb2.init(1, 1);
    bb.init(0ULL);
    bb.init(0, 0);
    bb.init(bb2);
    bb = bb2;
    bb = 2;

    printf("\n");
    printf("elapsed time1 = %0.6f\n", interval1.seconds());
    printf("elapsed time2 = %0.6f\n", interval3.seconds());
    printf("\n");

    //system("pause");
    //printf("\n");

    ///*
    itimer::interval itimval_;
    it1.begin();
    for (int i = 0; i < 10; i++) {
        //asm_pause_test();
    }
    itimval_ = it1.end();
    //*/

    printf("\n");
    printf("elapsed time (ReadTSC) = %0.6f\n", itimval_.seconds());
    printf("\n");

    aligned_space<itimer, 20> asp;
    itimer *iii = asp[0];
    iii = asp.begin();
    iii = asp.end();
    iii = asp[0];
    iii = asp.elements(0);
    int i223 = (int)asp.size();

    //my_random::srand();
    //sys_random::srand();

    my_random  my_random_;
    sys_random sys_random_;
    mt_random  mt_random_;
    mt_random_.srand();

    my_random_.rand();
    sys_random_.rand();
    mt_random_.rand();
    printf("my_random [0, 100]      = %d\n", my_random_.rand(0, 100));
    printf("my_random [-1000, 1000] = %d\n", my_random_.rand(-1000, 1000));
    printf("sys_random[0, 100]      = %d\n", sys_random_.rand(0, 100));
    printf("sys_random[-1000, 1000] = %d\n", sys_random_.rand(-1000, 1000));
    printf("mt_random[0, 100]       = %d\n", mt_random_.rand(0, 100));
    printf("mt_random[-1000, 1000]  = %d\n", mt_random_.rand(-1000, 1000));
    printf("\n");

    hash_table hash_table_;
    hash_table_.setup_hash(22);
    void *pHashTable = (void *)hash_table_.get_hash_table_ptr();

    printf("pHashTable(ptr)\t = 0x%08X\n", pHashTable);
    printf("hash_bits\t = %d,\nhash_entries\t = 0x%08X (%d),\nhash_sizes\t = 0x%08X (%d)\n",
           hash_table_.get_hash_bits(),
           hash_table_.get_hash_entries(),
           hash_table_.get_hash_entries(),
           hash_table_.get_hash_sizes(),
           hash_table_.get_hash_sizes()
          );
    printf("\n");
    printf("alloc_ptr\t = 0x%08X,\ndata_ptr\t = 0x%08X\n",
           (void *)hash_table_.get_alloc_ptr(),
           (void *)hash_table_.get_data_ptr()
          );
    printf("\n");
    hash_table_.free_hash();

    /*
    mresult = timeEndPeriod(1);
    if (TIMERR_NOERROR == mresult) {
        printf("timeBeginPeriod: TIMERR_NOERROR.\n");
    }
    //*/

    if (pbuffer) {
        _aligned_free(pbuffer);
    }

#if 0
    printf("BoardCast Send the \"WM_SETTINGCHANGE\" message to all processes and windows.\n");

    DWORD dwRet = 0;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, (LPARAM)"Environment", SMTO_NOTIMEOUTIFNOTHUNG, INFINITE, &dwRet);

    printf("Done.\n\n");
#endif

    system("pause");
    return 0;
}
