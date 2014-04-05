
/**********************************************************
   Use Visual Leak Detector(vld) for Visual C++,
   Homepage: http://vld.codeplex.com/
 **********************************************************/
#ifdef _MSC_VER
#include <vld.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <objbase.h>
#include <gmtl/gmtl.h>
#include <dolphin/dolphin.h>
#include <dolphin/cl_runner.h>
#include <dolphin/cl_helper.h>
#include <mmsystem.h>

#include "ms1b.h"
#include "dolphin_main.h"

//#include <platform/icd_test_log.h>

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#define ReadTSC(x)              \
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
#define FACTOR          ((double)LOOP_COUNT * 1000.0)

#define CLOCKSPERINSTRUCTION(start, end)    (((double)end - (double)start) / (FACTOR))

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

int fibonacci(int n)
{
#if 0
    if (n >= 2)
        return fibonacci(n - 1) + fibonacci(n - 2);
    else
        return n;
#else
    if (n < 2)
        return n;
    else
        return fibonacci(n - 1) + fibonacci(n - 2);
#endif
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

typedef struct accept_pair {
    bool is_accept_state;
    bool is_strict_end;
    //char app_name[0];
    char *app_name;
} accept_pair;

void test_alexia()
{
    char *s = "Alexia";
    accept_pair *ap;
    //ap = (accept_pair*)malloc(sizeof(accept_pair) + sizeof(s));
    ap = (accept_pair*)malloc(sizeof(accept_pair) + strlen(s) + 1);
    ap->app_name = (char *)((char *)ap + sizeof(accept_pair));
    strcpy(ap->app_name, s);

    printf("app name: %s\n\n", ap->app_name);

    free(ap);
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

#if 0
#ifndef _DEBUG
    const int n = 40;
    int fib;
    stop_watch sw;
    sw.start();
    fib = fibonacci(n);
    sw.stop();
    printf("fibonacci(%d) = %d\n", n, fib);
    printf("UsedTime = %0.3f ms\n\n", sw.getMillisec());
#endif
#endif

    //ms1b_main(0, NULL);
    //ms1b2_main(0, NULL);

#ifdef _DEBUG
    test_alexia();
#endif

    cl_runner clRunner;
    double usedTime_sw1 = 0.0, usedTime_sw2 = 0.0;
    double usedTime_sw2a = 0.0, usedTime_sw2b = 0.0;
    double usedTime_sw_copyData1 = 0.0, usedTime_sw_copyData2 = 0.0;
#if defined(_DEBUG) || 1
    //usedTime_sw2 = clRunner.test();
    int clError = (int)clRunner.init_cl();
    if (clError == CL_SUCCESS) {
        clError = clRunner.execute("vector_add_gpu.cl");
        usedTime_sw2a = clRunner.native_test1();
        usedTime_sw_copyData1 = clRunner.getMillisec_Native_CopyData();
        usedTime_sw2b = clRunner.native_test2();
        usedTime_sw_copyData2 = clRunner.getMillisec_Native_CopyData();
        printf("\n");
        if (clError == CL_SUCCESS) {
            usedTime_sw1 = clRunner.getMillisec();
            printf("cl kernel: vector_add_gpu.cl\n\n");
            printf("clRunner.getMillisec()      = %0.6f ms.\n", usedTime_sw1);
            printf("clRunner.kernel_ReadBuffer()= %0.6f ms.\n", clRunner.getMillisec_Kernel_ReadBuffer());
            printf("clRunner.native_test1()     = %0.6f ms.\n", usedTime_sw2a);
            printf("clRunner.native_CopyData1() = %0.6f ms.\n", usedTime_sw_copyData1);
            printf("clRunner.native_test2()     = %0.6f ms.\n", usedTime_sw2b);
            printf("clRunner.native_CopyData2() = %0.6f ms.\n", usedTime_sw_copyData2);
            if (usedTime_sw1 != 0.0)
                printf("clRunner.speed_up()         = %0.3f X\n", usedTime_sw2b / usedTime_sw1);
            else
                printf("clRunner.speed_up()         = ∞ X\n", usedTime_sw2b / usedTime_sw1);
        }
    }
    printf("\n");

    cl_helper clHelper;
    clError = clHelper.run_native_vector_add(1048576);
    if (clHelper.use_double())
        clError = clHelper.run_cl_vector_add(CL_RUNAT_DEFAULT, "vector_add_gpu.cl", "vector_add_double", 1048576);
    else
        clError = clHelper.run_cl_vector_add(CL_RUNAT_DEFAULT, "vector_add_gpu.cl", "vector_add_float", 1048576);
    if (clError == CL_SUCCESS) {
        usedTime_sw1 = clHelper.getMillisec();
        usedTime_sw2 = clHelper.getMillisec_Native();
        printf("\n");
        printf("cl kernel: vector_add_gpu.cl\n\n");
        printf("clHelper.getMillisec()       = %0.6f ms.\n", usedTime_sw1);
        printf("clHelper.kernel_ReadBuffer() = %0.6f ms.\n", clHelper.getMillisec_Kernel_ReadBuffer());
        /*
        printf("clHelper.native_test()       = %0.6f ms.\n", usedTime_sw2);
        printf("clHelper.native_CopyData()   = %0.6f ms.\n", clHelper.getMillisec_Native_CopyData());
        if (usedTime_sw1 != 0.0)
            printf("clHelper.speed_up()          = %0.3f X\n", usedTime_sw2 / usedTime_sw1);
        else
            printf("clHelper.speed_up()          = ∞ X\n",    usedTime_sw2 / usedTime_sw1);
        //*/
    }
    printf("\n");

    /*
    clHelper.reset_stopwatches();
    clError = clHelper.run_native_matrix_mul(1024, 1024, 1024);
    if (clHelper.use_double())
        clError = clHelper.run_cl_matrix_mul(CL_RUNAT_DEFAULT, "matrix_mul_gpu.cl", "matrix_mul_double", 1024, 1024, 1024);
    else
        clError = clHelper.run_cl_matrix_mul(CL_RUNAT_DEFAULT, "matrix_mul_gpu.cl", "matrix_mul_float", 1024, 1024, 1024);
    if (clError == CL_SUCCESS) {
        usedTime_sw1 = clHelper.getMillisec();
        usedTime_sw2 = clHelper.getMillisec_Native();
        printf("\n");
        printf("cl kernel: matrix_mul_gpu.cl\n\n");
        printf("clHelper.getMillisec()       = %0.6f ms.\n", usedTime_sw1);
        printf("clHelper.kernel_ReadBuffer() = %0.6f ms.\n", clHelper.getMillisec_Kernel_ReadBuffer());
        printf("clHelper.native_test()       = %0.6f ms.\n", usedTime_sw2);
        printf("clHelper.native_CopyData()   = %0.6f ms.\n", clHelper.getMillisec_Native_CopyData());
        if (usedTime_sw1 != 0.0)
            printf("clHelper.speed_up()          = %0.3f X\n", usedTime_sw2 / usedTime_sw1);
        else
            printf("clHelper.speed_up()          = ∞ X\n",    usedTime_sw2 / usedTime_sw1);
    }
    printf("\n");
    //*/
#endif

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

    /*

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

    printf("\n");
    printf("elapsed time1 = %0.6f\n", interval1.seconds());
    printf("elapsed time2 = %0.6f\n", interval3.seconds());

    stop_watch sw1, sw2;
    sw1.start();
    Sleep(500);
    sw1.stop();

    sw1.start();
    sw2.start();
    Sleep(500);
    sw1.stop();
    sw2.stop();

    printf("\n");
    printf("stop_watch sw1 = %0.6f Millisec(s)\n", sw1.getMillisec());
    printf("stop_watch sw1 = %0.6f Millisec(s) [getTotalMillisec()]\n", sw1.getTotalMillisec());
    printf("stop_watch sw2 = %0.6f Millisec(s)\n", sw2.getMillisec());
    printf("\n");

    StopWatch sw3, sw4;
    sw3.start();
    Sleep(500);
    sw3.stop();

    sw3.start();
    sw4.start();
    Sleep(500);
    sw3.stop();
    sw4.stop();

    printf("\n");
    printf("stop_watch sw3 = %0.1f Millisec(s)\n", sw3.getMillisec());
    printf("stop_watch sw3 = %0.1f Millisec(s) [getTotalMillisec()]\n", sw3.getTotalMillisec());
    printf("stop_watch sw4 = %0.1f Millisec(s)\n", sw4.getMillisec());
    printf("\n");

    itimer::interval itimval_;
    it1.begin();
    for (int i = 0; i < 10; i++) {
        //asm_pause_test();
    }
    itimval_ = it1.end();

    printf("\n");
    printf("elapsed time (ReadTSC) = %0.6f\n", itimval_.seconds());
    printf("\n");
    //*/

    void *pbuffer = _aligned_malloc(31 + 24, 32);
    char *pchar = (char *)pbuffer;
    pchar[54] = 0;

    GxString str1, str2;
    str1.set_length(13, 'A');
    str2 = str1;

    cache_aligned cache_align, c1, c2;
    cache_align.set_align_size(32);
    cache_align.mem_alloc(31 + 24);
    c1 = c2 = cache_align;
    //c1 = c2 = 2;

    task task;
    task.set_depth(0);
    task.spawn();
    task::spawn(task);
    //task_base_t::spawn(task);

    task_scheduler task_scheduler(task_scheduler::MTL_AUTOMATIC);
    task_scheduler.initialize(4);

    dolphin::init_flip_mask();

    bitboard::init_bitboard();
    bitboard bb1, bb2, bb3;
    bb2.init(1, 1);
    bb1.init(0ULL);
    bb1.init(0, 0);
    bb1.init(bb2);
    bb1 = bb2;
    bb1 = 2;

    bb1.set(12);
    bb1.default(OPENNING_POS_1_BLACK);
    bb2.default(OPENNING_POS_ALL);

    bb1.set(bb2);

    bb1.empty();
    bb1.set(bitboard::make_pos(3, 3));
    bb1.or (bitboard::make_pos(4, 4));

    bb1.reverse();
    int n_popcount_1 = bb1.popcount();

    bb3.default(OPENNING_POS_TEST);
    bb3.rotate(RT_MIRROR_DIAG_1);
    //bb3.rotate(RT_REVERSE);
    //bb3.rotate(RT_ROTATE_LT);
    int n_popcount_3 = bb3.popcount();

    printf("n_popcount_1 = %d, n_popcount_3 = %d\n", n_popcount_1, n_popcount_3);
    printf("\n");

    ubitboard ubb1;
    ubb1.low  = 0;
    ubb1.high = 0;

    bitboards bbs1, bbs2;
    bbs1.set(bb1, bb2);

    //system("pause");
    //printf("\n");

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
    hash_table_.setup_hash_bits(22);
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
