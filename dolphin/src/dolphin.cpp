
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <objbase.h>
#include "../../gmtl/include/gmtl.h"
#include "../../dolphin/include/dolphin.h"
#include <mmsystem.h>

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
    for (int i=0; i<LOOP_COUNT; i++) {
        REPEAT_1000(__asm { nop };)
        if ((i % 500) == 0) {
            //SwitchToThread();
            //Sleep(1);
        }
    }
    ReadTSC(end);
    total = end - start;

    printf("nop:   clocks per instruction %4.2f\n", (double)total/(double)FACTOR);

    ReadTSC(start);
    total = 0;;
    for (int i=0; i<LOOP_COUNT; i++) {
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

    printf("pause: clocks per instruction %4.2f\n", (double)total/(double)FACTOR);
}

// շת�����, �򵥸�Ч, �������, �޳�������   
int GetMaxCommonDivide(int n, int m)
{  
    while (n != m) {
        if (n > m)
            n = n - m;
        else
            n = m - n;
    }

    return n;
}

// շת�����, �򵥸�Ч, �������, �޳�������
int GetMaxCommonDivide_new(int n, int m)
{
    while (n != m) {
        if (n > m)
            n = n - m;
        else
            m = m - n;
    }

    return n;
}

// _tmain() ������� tchar.h, main()��Unicode�汾
int _tmain(int argc, _TCHAR* argv[])
{
    for (int i=0; i<2; i++) {
        //asm_pause_test();
    }

    int gcd;
    gcd = GetMaxCommonDivide_new(7, 5);
    printf("GCD(7, 5) = %d\n\n", gcd);

    //system("pause");
    //return 0;

    /*
    MMRESULT mresult = timeBeginPeriod(1);
    if (TIMERR_NOERROR == mresult) {
        printf("timeBeginPeriod: TIMERR_NOERROR.\n");
    }
    //*/

    tickcount_t t1, t2, tnow;
    itimer_t it1, it2;
    tickcount_t::interval_t interval1, interval2;
    itimer_t::interval_t interval3, interval4;
    t1.begin();
    it1.begin();
    Sleep(500);
    t2.begin();
    it2.begin();
    interval1 = t2 - t1;
    tnow = tickcount_t::now();
    interval1 = tnow - t1;
    interval1 = t1.end();
    interval3 = it1.end();

    double f_interval = interval1.seconds();

    void *pbuffer = _aligned_malloc(31+24, 32);
    char *pchar = (char *)pbuffer;
    pchar[54] = 0;

    GxString str1, str2;
    str1.SetLength(13, 'A');
    str2 = str1;

    cache_aligned_t cache_align, c1, c2;
    cache_align.SetAlignSize(32);
    cache_align.Malloc(31+24);
    c1 = c2 = cache_align;
    //c1 = c2 = 2;

    task_t task;
    task.set_depth(0);
    task.spawn();
    task_t::spawn(task);
    //task_base_t::spawn(task);

    task_scheduler_t task_scheduler(task_scheduler_t::MTL_AUTOMATIC);
    task_scheduler.initialize(4);

    bitboard::init_square_mask();
    bitboard bb, bb2;
    bb2.init(1, 1);
    bb.init(0ULL);
    bb.init(0, 0);
    bb.init(bb2);
    bb = bb2;
    bb = 2;

    printf("\r\n");
    printf("elapsed time1 = %0.6f\r\n", interval1.seconds());
    printf("elapsed time2 = %0.6f\r\n", interval3.seconds());
    printf("\r\n");

    //system("pause");
    //printf("\r\n");

    ///*
    itimer_t::interval_t itimval;
    it1.begin();
    for (int i=0; i<10; i++) {
        //asm_pause_test();
    }
    itimval = it1.end();
    //*/

    printf("\r\n");
    printf("elapsed time = %0.6f\r\n", itimval.seconds());
    printf("\r\n");

    aligned_space_t<itimer_t, 20> asp;
    itimer_t* iii = asp[0];
    iii = asp.begin();
    iii = asp.end();
    iii = asp[0];
    iii = asp.elements(0);
    int i223 = (int)asp.size();

    my_random::srand();

    my_random random;
    random.random();
    printf("my_random[0, 100]      = %d\n", my_random::random(0, 100));
    printf("my_random[-1000, 1000] = %d\n", my_random::random(-1000, 1000));
    printf("my_rand[0, 100]        = %d\n", my_random::rand(0, 100));
    printf("my_rand[-1000, 1000]   = %d\n", my_random::rand(-1000, 1000));
    printf("\r\n");

    hash_table_t hash_table;
    hash_table.setup_hash(22);
    void *pHashTable = (void *)hash_table.hash_table();
    
    printf("pHashTable(ptr)\t = 0x%08X\n", pHashTable);
    printf("hash_bits\t = %d,\nhash_entries\t = 0x%08X (%d),\nhash_sizes\t = 0x%08X (%d)\n",
        hash_table.hash_bits(),
        hash_table.hash_entries(),
        hash_table.hash_entries(),
        hash_table.hash_sizes(),
        hash_table.hash_sizes()
        );
    printf("\n");
    printf("alloc_ptr\t = 0x%08X,\ndata_ptr\t = 0x%08X\n",
        (void *)hash_table.alloc_ptr(),
        (void *)hash_table.data_ptr()
        );
    printf("\r\n");
    hash_table.free_hash();

    /*
    mresult = timeEndPeriod(1);
    if (TIMERR_NOERROR == mresult) {
        printf("timeBeginPeriod: TIMERR_NOERROR.\n");
    }
    //*/

    system("pause");
    return 0;
}
