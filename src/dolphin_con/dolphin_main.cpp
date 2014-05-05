
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
#include <math.h>

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

/*
enum eNumberIndex {
    INVALID_SPECIAL_NUM = -1,
    NORMAL_NUM_INDEX = 0,
    FIRST_SPECIAL_NUM_FIXED_INDEX = 1,
    IS_SPECIAL_NUM = 2
};
//*/

#define USE_STL_STRING                  0

#define DISPLAY_RESULT_TO_SCREEN        0
#define FIZZ_USE_STOPWATCH              1

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
#undef  FIZZ_USE_STOPWATCH
#define FIZZ_USE_STOPWATCH              0
#endif

#define INVALID_SPECIAL_NUM             (-1)
#define NORMAL_NUM_INDEX                0
#define FIRST_SPECIAL_NUM_FIXED_INDEX   1
#define IS_SPECIAL_NUM                  2

#define STRING_ADDR_ALIGNMENT           16UL

typedef unsigned int index_mask_t;

static double s_SolveTotalTime = 0.0;

/// <comment>
///
/// 最难面试的IT公司之ThoughtWorks代码挑战——FizzBuzzWhizz游戏
///
/// From: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
///
/// </comment>

void FizzBuzzWhizz(const int max_number, const unsigned int max_word_type,
                   const string word_list[], int special_num_list[],
                   char **out_sayword_result)
{
    int num;
    unsigned int integer_base10_length;
    unsigned int digital, max_digital;
    unsigned int index;
    unsigned int mask, cur_mask, max_mask;
    int special_num;
    int first_special_num;

    int left_num, right_num;
    int left_start_num, right_start_num;
    int left_num_step, right_num_step;
    int right_max_num;

    index_mask_t *sayword_index_list;

    const int integer_base10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000,
        10000000, 100000000, 1000000000, INT_MAX
    };

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    stop_watch sw;
    sw.start();
#endif  /* FIZZ_USE_STOPWATCH */

    integer_base10_length = _countof(integer_base10) - 1;
#if 0
    // 最大数的数字位数(数字最大长度), 
    max_digital = (int)floor(log((double)max_number) / log(10.0) + 0.001) + 1;
#else
    // 最大数的数字位数(数字最大长度), 可以用log函数求, 但这里我们用查表法, 因为这个表我们后面也要用的
    max_digital = 0;
    for (index = 0; index < integer_base10_length; ++index) {
        if (max_number >= integer_base10[index] && max_number < integer_base10[index + 1]) {
            max_digital = index + 1;
            break;
        }
    }
#endif

    unsigned int max_words_length = 1;
    for (index = 0; index < max_word_type; ++index)
        max_words_length += word_list[index].length();
    if (max_digital > max_words_length)
        max_words_length = (max_digital + STRING_ADDR_ALIGNMENT - 1) & (~STRING_ADDR_ALIGNMENT);
    else
        max_words_length = (max_words_length + STRING_ADDR_ALIGNMENT - 1) & (~STRING_ADDR_ALIGNMENT);

    // 最大mask(bit掩模)值, = 2^(max_word_type) = 2^3 = 8
    max_mask = 1 << max_word_type;

    // 求出mask对应的特殊报数字符串, 共max_mask种(2^3 = 8种)
#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)
    string sayword;
    std::vector<string> sayword_list;

    sayword_list.reserve(max_mask);
    //sayword_list.resize(max_mask);
    for (mask = 0; mask < max_mask; ++mask) {
        sayword = "";
        cur_mask = mask;
        for (index = 0; index < max_word_type; ++index) {
            if ((cur_mask & 1) != 0)
                sayword += word_list[index];
            cur_mask >>= 1;
        }
        sayword_list.push_back(sayword);
        //sayword_list[mask] = sayword;
    }
#else
    char *sayword_list, *cur_sayword_list;
    sayword_list = (char *)_aligned_malloc(max_mask * max_words_length * sizeof(char), STRING_ADDR_ALIGNMENT);
    if (sayword_list == NULL) {
        printf("Error: alloc string array [sayword_list] out of memory\n\n");
        return;
    }
    for (mask = 0; mask < max_mask; ++mask) {
        cur_sayword_list = sayword_list + mask * max_words_length;
        cur_sayword_list[0] = '\0';
        cur_mask = mask;
        for (index = 0; index < max_word_type; ++index) {
            if ((cur_mask & 1) != 0)
                //sayword += word_list[index];
                strcat(cur_sayword_list, word_list[index].c_str());
            cur_mask >>= 1;
        }
        //strcpy(cur_sayword_list, sayword.c_str());
    }
#endif

    // 分配sayword_index_list内存
    sayword_index_list = (index_mask_t *)malloc((max_number + 1) * sizeof(index_mask_t));
    if (sayword_index_list == NULL) {
        printf("Error: alloc array [sayword_index_list] out of memory\n\n");
        return;
    }

    // 所有sayword_index_list的默认值均为NORMAL_NUM_INDEX(0), 即默认是非特殊报数
    for (num = 0; num <= max_number; ++num) {
        sayword_index_list[num] = NORMAL_NUM_INDEX;
    }

    // 计算(合并)和设置所有特殊数的mask值
    for (index = 0; index < max_word_type; ++index) {
        // 取一个特殊数
        special_num = special_num_list[index];
        // 如果特殊数不在[1, 9]范围内, 则认为是无效特殊数, 跳过
        if (special_num >= 1 && special_num <= 9) {
            // 该特殊数的mask值
            mask = 1 << index;
            for (num = special_num; num <= max_number; num += special_num)
                sayword_index_list[num] |= (index_mask_t)mask;
        }
        else {
            special_num_list[index] = INVALID_SPECIAL_NUM;
        }
    }

    // 根据规则5, 设置所有所报数字包含了第一个特殊数(first)的数, 先筛选所有个位数包含first的数,
    // 再筛选所有十位数包含first的数, 依此类推, 百位, 千位..., 直接达到max_number
    // FIRST_SPECIAL_NUM_FIXED_INDEX的值固定为1, 因为第一个特殊数(仅第一个)的mask就是1

    // 第一个特殊数
    first_special_num = special_num_list[0];

    if (first_special_num != INVALID_SPECIAL_NUM) {

        // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数
        for (digital = 0; digital < max_digital; ++digital) {
            right_start_num = first_special_num * integer_base10[digital];
            right_max_num = right_start_num + integer_base10[digital];
            // 右边的步长横为1
            right_num_step = 1;
            // 这里(right_start_num + 0)虽然已经是first_special_num的倍数, 但是因为还要进行左边(高位)的循环, 所以不能省略
            // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
            for (right_num = right_start_num + 0; (right_num < right_max_num && right_num <= max_number); right_num += right_num_step) {
                sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                if (digital < integer_base10_length) {
                    left_num_step = integer_base10[digital + 1];
                    left_start_num = right_num + left_num_step;
                    // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
                    for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step)
                        sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                }
            }
        }

    }

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    sw.stop();
#endif  /* FIZZ_USE_STOPWATCH */

    // 如果想显示快一点, 可以用前面一段代码(题目默认的显示效果),
    // 后面的代码有显示对应的num, 理论上会慢一点(如果追求绝对速度的话)
#if 1  /* 显示结果的样式 */
    // 输出结果
    printf("FizzBuzzWhizz ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)
        if (index == NORMAL_NUM_INDEX)
            printf("%d\n", num);
        else
            printf("%s\n", sayword_list[index].c_str());
#else
        if (index == NORMAL_NUM_INDEX)
            printf("%d\n", num);
        else
            printf("%s\n", &sayword_list[index * max_words_length]);
#endif  /* USE_STL_STRING */
    }
    printf("\n");
#else   /* !显示结果的样式 */
    // 输出结果
    printf("FizzBuzzWhizz ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)
        if (index == NORMAL_NUM_INDEX)
            printf("num = %-8d Sayword is: %d\n", num, num);
        else
            printf("num = %-8d Sayword is: %s\n", num, sayword_list[index].c_str());
#else
        if (index == NORMAL_NUM_INDEX)
            printf("num = %-8d Sayword is: %d\n", num, num);
        else
            printf("num = %-8d Sayword is: %s\n", num, &sayword_list[index * max_words_length]);
#endif  /* USE_STL_STRING */
    }
    printf("\n");
#endif  /* 显示结果的样式 */

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    printf("elapsed time: %0.3f ms\n\n", sw.getMillisec());
#endif  /* FIZZ_USE_STOPWATCH */

#else  /* !DISPLAY_RESULT_TO_SCREEN */

#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)
    char buffer[32];

    // 如果不输出到屏幕, 则输出到字符串数组
    std::vector<string> sayword_result;
    sayword_result.resize(max_number + 1);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX) {
            if (_itoa(num, buffer, 10))
            //    sayword_result.push_back(buffer);
                sayword_result[num] = buffer;
        }
        else {
            //sayword_result.push_back(sayword_list[index].c_str());
            //sayword_result[num] = sayword_list[index].c_str();
            sayword_result[num] = sayword_list[index];
        }
    }
#else
    //char buffer[32];

    // 如果不输出到屏幕, 则输出到字符串数组
    char *sayword_result, *cur_sayword_result;
    index_mask_t *cur_sayword_index_list;
    sayword_result = (char *)_aligned_malloc((max_number + 1) * max_words_length * sizeof(char), STRING_ADDR_ALIGNMENT);
    if (sayword_result == NULL) {
        printf("Error: alloc string array [sayword_result] out of memory\n");
        return;
    }
    cur_sayword_result = sayword_result + max_words_length;
    cur_sayword_index_list = &sayword_index_list[1];
    for (num = 1; num <= max_number; ++num) {
        //index = sayword_index_list[num];
        index = *cur_sayword_index_list++;
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX) {
            _itoa(num, cur_sayword_result, 10);
            //if (_itoa(num, buffer, 10))
                //strcpy(&sayword_result[num * max_words_length], buffer);
            //    strcpy(cur_sayword_result, buffer);
        }
        else {
            //strcpy(&sayword_result[num * max_words_length], sayword_list[index].c_str());
            //strcpy(cur_sayword_result, sayword_list[index].c_str());
            strcpy(cur_sayword_result, &sayword_list[index * max_words_length]);
        }
        cur_sayword_result += max_words_length;
    }

    if (out_sayword_result != NULL) {
        *out_sayword_result = sayword_result;
    }
    else {
        if (sayword_result)
            _aligned_free(sayword_result);
    }
#endif

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    sw.stop();
    //printf("elapsed time: %0.3f ms\n\n", sw.getMillisec());

    s_SolveTotalTime += sw.getMillisec();
#endif  /* FIZZ_USE_STOPWATCH */

#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)
    if (out_sayword_result != NULL) {
        // 输出结果
        printf("FizzBuzzWhizz ReportNumber List:\n\nmax_number = %d\n\n", max_number);
        for (num = 1; num <= max_number; ++num) {
            index = sayword_index_list[num];
            // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
            if (index == NORMAL_NUM_INDEX)
                printf("%d\n", num);
            else
                printf("%s\n", sayword_list[index].c_str());
        }
        printf("\n");
    }
#endif  /* USE_STL_STRING */

#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // 释放内存
#if !defined(USE_STL_STRING) || (USE_STL_STRING == 0)
    if (sayword_list)
        _aligned_free(sayword_list);
#endif
    if (sayword_index_list)
        free(sayword_index_list);
}

void FizzBuzzWhizz_Test(const int max_number)
{
    stop_watch sw;
    int special_num_list[] = { 3, 5, 7 };
    string word_list[] = { "Fizz", "Buzz", "Whizz" };
    char *sayword_result = NULL;

    // 最大word类型
    unsigned int max_word_type = _countof(word_list);

    int max_words_length = 1;
    for (unsigned int index = 0; index < max_word_type; ++index)
        max_words_length += word_list[index].length();

    max_words_length = (max_words_length + STRING_ADDR_ALIGNMENT - 1) & (~STRING_ADDR_ALIGNMENT);

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)

    FizzBuzzWhizz(max_number, max_word_type, word_list, special_num_list, NULL);

#else  /* !DISPLAY_RESULT_TO_SCREEN */

#if defined(USE_STL_STRING) && (USE_STL_STRING != 0)

    sw.start();
    for (int i = 0; i < 10000; ++i) {
        FizzBuzzWhizz(max_number, max_word_type, word_list, special_num_list, NULL);
    }
    sw.stop();

    FizzBuzzWhizz(max_number, max_word_type, word_list, special_num_list, &sayword_result);

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    printf("Solve Elapsed Time: %0.3f ms\n\n", s_SolveTotalTime);
#endif  /* FIZZ_USE_STOPWATCH */
    printf("Total Elapsed Time: %0.3f ms\n\n", sw.getMillisec());

#else  /* !USE_STL_STRING */

    sw.start();
    for (int i = 0; i < 10000; ++i) {
        FizzBuzzWhizz(max_number, max_word_type, word_list, special_num_list, &sayword_result);
        if (sayword_result) {
            _aligned_free(sayword_result);
            sayword_result = NULL;
        }
    }
    sw.stop();

    FizzBuzzWhizz(max_number, max_word_type, word_list, special_num_list, &sayword_result);

    if (sayword_result != NULL) {
        // 输出结果
        printf("FizzBuzzWhizz ReportNumber List:\n\nmax_number = %d\n\n", max_number);
        for (int num = 1; num <= max_number; ++num) {
            printf("%s\n", &sayword_result[num * max_words_length]);
        }
        printf("\n");
    }

#if defined(FIZZ_USE_STOPWATCH) && (FIZZ_USE_STOPWATCH != 0)
    printf("Solve Elapsed Time: %0.3f ms\n\n", s_SolveTotalTime);
#endif  /* FIZZ_USE_STOPWATCH */
    printf("Total Elapsed Time: %0.3f ms\n\n", sw.getMillisec());

    if (sayword_result) {
        _aligned_free(sayword_result);
        sayword_result = NULL;
    }
#endif  /* USE_STL_STRING */

#endif  /* DISPLAY_RESULT_TO_SCREEN */
}

void FizzBuzzWhizz_v2(const int max_number)
{
    int max_word_type, max_mask;
    int num, cur_num, index;
    int mask, cur_mask;

    string sayword;
    std::vector<string> sayword_list;
    unsigned int *sayword_index_list;

    int special_num_list[] = { 3, 5, 7 };
    string word_list[] = { "Fizz", "Buzz", "Whizz" };

    max_word_type = _countof(word_list);
    max_mask = 1 << max_word_type;

    sayword_index_list = (unsigned int *)malloc((max_number + 1) * sizeof(unsigned int));
    if (sayword_index_list == NULL) {
        printf("Error: alloc array [sayword_index_list] out of memory\n\n");
        return;
    }

    sayword_list.clear();

    // get the list of multiple times special numbers sayword
    for (mask = 0; mask < max_mask; ++mask) {
        sayword = "";
        cur_mask = mask;
        for (index = 0; index < max_word_type; ++index) {
            if ((cur_mask & 1) != 0)
                sayword += word_list[index];
            cur_mask >>= 1;
        }
        sayword_list.push_back(sayword);
    }

    // set all index to the default value: NORMAL_NUM_INDEX
    for (num = 1; num <= max_number; ++num) {
        sayword_index_list[num] = NORMAL_NUM_INDEX;
    }

    // get the index by num, if is the first special number, index fixed equal 1 (FIRST_SPECIAL_NUM_FIXED_INDEX)
    for (index = 1; index < max_word_type; ++index) {
        cur_num = special_num_list[index];
        // another special number
        mask = 1 << index;
        for (num = cur_num; num <= max_number; num += cur_num)
            sayword_index_list[num] |= mask;
    }

    // first special number
    cur_num = special_num_list[0];
    for (num = cur_num; num <= max_number; num += cur_num)
        sayword_index_list[num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

    printf("FizzBuzzWhizz Sayword List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        if (index == NORMAL_NUM_INDEX)
            printf("num = %d,\tSayword is: %d\n", num, num);
        else
            printf("num = %d,\tSayword is: %s\n", num, sayword_list[index].c_str());
    }
    printf("\n");

    if (sayword_index_list)
        free(sayword_index_list);
}

int get_type_index(int num)
{
    int index = 0;
    return index;
}

void FizzBuzzWhizz_v1(const int max_number)
{
    int i;
    int max_type, max_mask, max_digital;
    int num, index, flag = 0;
    int mask, cur_mask, cur_num;
    int first_special_num;

    int integer_base10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000,
        10000000, 100000000, 1000000000, INT_MAX
    };

    int special_num[] = { 3, 5, 7 };
    string word_list[] = { "Fizz", "Buzz", "Whizz" };

    std::vector<string> sayword_list;
    string sayword;
    unsigned int *sayword_index_list, *flag_list;
    unsigned int *sayword_mask_list;
    int num_flags[10];
    char buf[32] = { 0 };

    max_type = _countof(word_list);
    max_mask = 1 << max_type;
    max_digital = (int)floor(log((double)max_number) / log(10.0) + 0.001) + 1;
    first_special_num = special_num[0];

    sayword_index_list = (unsigned int *)malloc((max_number + 1) * sizeof(unsigned int));
    if (sayword_index_list == NULL) {
        printf("Error: alloc array [sayword_index_list] out of memory\n\n");
        return;
    }

    sayword_mask_list = (unsigned int *)malloc((max_number + 1) * sizeof(unsigned int));
    if (sayword_mask_list == NULL) {
        printf("Error: alloc array [sayword_mask_list] out of memory\n\n");
        return;
    }

    flag_list = (unsigned int *)malloc(max_digital * sizeof(unsigned int));
    if (flag_list == NULL) {
        printf("Error: alloc array [flag_list] out of memory\n\n");
        return;
    }

    sayword_list.clear();

    // get the number's flag, if is first special number that flag is 2,
    // if is special number that flag is 1, otherwise flag is 0
    for (num = 0; num < 10; ++num) {
        num_flags[num] = 0;
        for (index = 0; index < max_type; ++index) {
            if (num == special_num[index]) {
                num_flags[num] = IS_SPECIAL_NUM + (index == 0) ? 1 : 0;
                break;
            }
        }
    }

    /*
    // get the list of say number only
    for (num = 0; num < max_number; ++num) {
        if (_itoa(num, buf, 10)) {
        //if (itoa_s(num, buf, _countof(buf), 10)) {
            word = buf;
            sayword_list.push_back(word);
        }
    }
    //*/

    // for first special number
    //sayword_list.push_back(word_list[0]);

    // get the list of multiple special numbers sayword
    for (mask = 0; mask < max_mask; ++mask) {
        sayword = "";
        cur_mask = mask;
        for (i = 0; i < max_type; ++i) {
            if ((cur_mask & 1) != 0)
                sayword += word_list[i];
            cur_mask >>= 1;
        }
        sayword_list.push_back(sayword);
    }

    // set all the index to default value: NORMAL_NUM_INDEX
    for (num = 1; num <= max_number; ++num) {
        sayword_index_list[num] = NORMAL_NUM_INDEX;
    }

    for (index = 0; index < max_type; ++index) {
        cur_num = special_num[index];
        if (index == 0) {
            // FIRST_SPECIAL_NUM_FIXED_INDEX
            for (num = cur_num; num <= max_number; num += cur_num)
                sayword_index_list[num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
        }
        else {
            // IS_SPECIAL_NUM
            mask = 1 << index;
            for (num = cur_num; num <= max_number; num += cur_num)
                sayword_index_list[num] |= mask;
        }
    }

#if 0
    /*
    for (i = 0; i < 10; ++i) {
        for (j = 0; j < 10; ++j) {
            num = i * 10 + j;
            flag_1 = num_flags[i];
            flag_2 = num_flags[j];
            if (flag_1 == FIRST_SPECIAL_NUM_FIXED_INDEX || flag_2 == FIRST_SPECIAL_NUM_FIXED_INDEX) {
                sayword_index_list[num] = max_number + 1;
                break;
            }
            else if (flag_1 == NORMAL_NUM_INDEX && flag_2 == NORMAL_NUM_INDEX) {
                sayword_index_list[num] = num;
            }
            else {
                sayword_index_list[num] = max_number + get_type_index(num);
            }
        }
    }
    //*/

    int digital;
    int digital_cnt = 0;
    bool is_first_special_num;
    bool is_normal_num;
    for (num = 1; num < max_number; ++num) {
        if (num == integer_base10[digital_cnt])
            digital_cnt++;
        is_first_special_num = false;
        is_normal_num = true;
        for (index = 0; index < digital_cnt; ++index) {
            digital = num / integer_base10[digital_cnt - index - 1];
            flag = num_flags[digital];
            if (digital == first_special_num) {
                sayword_index_list[num] = max_number + 1;
                is_first_special_num = true;
                break;
            }
            else if (flag == FIRST_SPECIAL_NUM_FIXED_INDEX) {
                sayword_index_list[num] = max_number + 1;
                is_first_special_num = true;
                break;
            }
            else if (flag == IS_SPECIAL_NUM) {
                is_normal_num = false;
            }
            flag_list[index] = flag;
        }
        if (!is_first_special_num) {
            sayword_index_list[num] = max_number + 1;
            break;
        }

        flag_list[i] = 0;
    }
#endif

    printf("FizzBuzzWhizz Sayword List: max_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        if (index == NORMAL_NUM_INDEX)
            printf("num = %d,\tSayword is: %d\n", num, num);
        else
            printf("num = %d,\tSayword is: %s\n", num, sayword_list[index].c_str());
    }
    printf("\n");

    if (sayword_index_list)
        free(sayword_index_list);
    if (sayword_mask_list)
        free(sayword_mask_list);
    if (flag_list)
        free(flag_list);
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

    FizzBuzzWhizz_Test(100);

    //FizzBuzzWhizz(100);

    system("pause");
    return 0;

#ifdef _DEBUG
    test_alexia();
#endif

    printf("cl_runner start...\n\n");
    cl_runner clRunner;
    double usedTime_sw1 = 0.0, usedTime_sw2 = 0.0;
    double usedTime_sw2a = 0.0, usedTime_sw2b = 0.0;
    double usedTime_sw_copyData1 = 0.0, usedTime_sw_copyData2 = 0.0;
#if defined(_DEBUG) || 1
    //usedTime_sw2 = clRunner.test();
    int clError = (int)clRunner.init_cl();
    if (clError == CL_SUCCESS) {
        clError = clRunner.execute("vector_add_gpu.cl");
        usedTime_sw2a = clRunner.native_vector_add_test();
        usedTime_sw_copyData1 = clRunner.getMillisec_Native_CopyData();
        usedTime_sw2b = clRunner.native_vector_mult_test();
        usedTime_sw_copyData2 = clRunner.getMillisec_Native_CopyData();
        printf("\n");
        if (clError == CL_SUCCESS) {
            usedTime_sw1 = clRunner.getMillisec();
            printf("cl kernel: vector_add_gpu.cl\n\n");
            printf("clRunner.getMillisec()             = %0.6f ms.\n", usedTime_sw1);
            printf("clRunner.kernel_ReadBuffer()       = %0.6f ms.\n", clRunner.getMillisec_Kernel_ReadBuffer());
            printf("\n");
            printf("clRunner.native_vector_add_test()  = %0.6f ms.\n", usedTime_sw2a);
            printf("clRunner.native_copy_data1()       = %0.6f ms.\n", usedTime_sw_copyData1);
            printf("clRunner.native_vector_mult_test() = %0.6f ms.\n", usedTime_sw2b);
            printf("clRunner.native_copy_data2()       = %0.6f ms.\n", usedTime_sw_copyData2);
            if (usedTime_sw1 != 0.0)
                printf("clRunner.speed_up()                = %0.3f X\n", usedTime_sw2b / usedTime_sw1);
            else
                printf("clRunner.speed_up()                = ∞ X\n", usedTime_sw2b / usedTime_sw1);
        }
    }
    printf("\n");

    printf("cl_helper start...\n\n");
    cl_helper clHelper;
    clError = clHelper.run_native_vector_add(1048576);
    // CL_RUNAT_DEFAULT, CL_RUNAT_GPU, CL_RUNAT_CPU
    if (clHelper.use_double())
        clError = clHelper.run_vector_add(CL_RUNAT_DEFAULT, "vector_add_gpu.cl", "vector_add_double", 1048576);
    else
        clError = clHelper.run_vector_add(CL_RUNAT_DEFAULT, "vector_add_gpu.cl", "vector_add_float", 1048576);
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
