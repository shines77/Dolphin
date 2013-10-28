
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <objbase.h>
#include <gmtl/gmtl.h>
#include <dolphin/dolphin.h>
#include <mmsystem.h>

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

// 辗转相减法, 简单高效, 清晰快捷, 无除法运算
int GetMaxCommonDivide(int n, int m)
{
    while (n != m) {
        if (n > m) {
            n = n - m;
        }
        else {
            n = m - n;
        }
    }

    return n;
}

// 辗转相减法, 简单高效, 清晰快捷, 无除法运算
int GetMaxCommonDivide_new(int n, int m)
{
    while (n != m) {
        if (n > m) {
            n = n - m;
        }
        else {
            m = m - n;
        }
    }

    return n;
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
    gcd = GetMaxCommonDivide_new(7, 5);
    printf("GCD(7, 5) = %d\n\n", gcd);

    int m, n, p, p1, p2;
    n = 20; p = 2;
    p1 = 2; p2 = 4;
    //m = GetMaxEntry(n, p);
    //printf("GetMaxEntry(%d, %d) = %d\n\n", n, p, m);

    m = HashMask_GetMaxLength(n, p1, p2, 5);
    //m = HashMask_GetMaxLength(n, p, (1 << 11) - 1);
    printf("HashMask_GetMaxLength(%d, %d, %d) = %d\n\n", n, p1, p2, m);

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

    my_random::srand();
    sys_random::srand();

    my_random  my_random_;
    sys_random sys_random_;
    my_random_.rand();
    sys_random_.rand();
    printf("my_random [0, 100]      = %d\n", my_random_.rand(0, 100));
    printf("my_random [-1000, 1000] = %d\n", my_random_.rand(-1000, 1000));
    printf("sys_random[0, 100]      = %d\n", sys_random_.rand(0, 100));
    printf("sys_random[-1000, 1000] = %d\n", sys_random_.rand(-1000, 1000));
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

    system("pause");
    return 0;
}
