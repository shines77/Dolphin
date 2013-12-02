
#include <dolphin/hash_mask.h>
#include <dolphin/hash_table.h>
#include <dolphin/my_random.h>
#include <dolphin/sys_random.h>
#include <windows.h>

#define HASHMASK_EMPTY32   ((unsigned int)(-1))
#define HASHMASK_EMPTY64   ((unsigned __int64)(-1))

#define HASHMASK_USE_RANDOM_LIST    1
#define HASHMASK_USE_RANDOM_START   1

namespace dolphin {

unsigned int *mask_list_max     = NULL;
unsigned int  max_mask_length   = 0;
unsigned int  max_mask_count    = 0;

unsigned int popcount32( unsigned int num )
{
    const unsigned long m1 = 0x55555555UL;
    const unsigned long m2 = 0x33333333UL;
    unsigned int a, n1;

    a = num - ((num >> 1) & m1);
    n1 = (a & m2) + ((a >> 2) & m2);
    n1 = (n1 & 0x0F0F0F0FUL) + ((n1 >> 4) & 0x0F0F0F0FUL);
    n1 = (n1 & 0x0000FFFFUL) +  (n1 >> 16);
    n1 = (n1 & 0x000000FFUL) +  (n1 >> 8);

    return n1;
}

unsigned int popcount64( unsigned int high, unsigned int low )
{
    const unsigned long m1 = 0x55555555UL;
    const unsigned long m2 = 0x33333333UL;
    unsigned int a, b, n1, n2;

    a = high - ((high >> 1) & m1);
    n1 = (a & m2) + ((a >> 2) & m2);
    n1 = (n1 & 0x0F0F0F0FUL) + ((n1 >> 4) & 0x0F0F0F0FUL);
    n1 = (n1 & 0x0000FFFFUL) +  (n1 >> 16);
    n1 = (n1 & 0x000000FFUL) +  (n1 >> 8);

    b = low - ((low >> 1) & m1);
    n2 = (b & m2) + ((b >> 2) & m2);
    n2 = (n2 & 0x0F0F0F0FUL) + ((n2 >> 4) & 0x0F0F0F0FUL);
    n2 = (n2 & 0x0000FFFFUL) +  (n2 >> 16);
    n2 = (n2 & 0x000000FFUL) +  (n2 >> 8);

    return n1 + n2;
}

int GetMaxEntry(int n, int p)
{
    int i, j, k;
    int max = 0, m;
    unsigned int max_num = 1 << n;
    unsigned int mask = max_num - 1;
    unsigned int *entry, *entry_set1, *entry_set2, *entry_result;
    entry = (unsigned int *)malloc(max_num * sizeof(unsigned int));
    if (entry == NULL)
        return 0;

    entry_set1 = (unsigned int *)malloc(max_num * sizeof(unsigned int));
    if (entry_set1 == NULL)
        return 0;

    entry_set2 = (unsigned int *)malloc(max_num * sizeof(unsigned int));
    if (entry_set2 == NULL)
        return 0;

    entry_result = (unsigned int *)malloc(max_num * sizeof(unsigned int));
    if (entry_result == NULL)
        return 0;

    unsigned int num, val, popcnt;
    unsigned int start = 0;
    unsigned int end = -1;
    unsigned int now;
    for (num=0; num<max_num; ++num) {
        val = num ^ start;
        popcnt = popcount32(val);
        if (popcnt == p) {
            end = num;
            break;
        }
    }

    while (end == -1 || (start < end && end != -1)) {
        m = 1;
        entry[0] = start;
        now = entry[0];
        for (i=1; i<(int)max_num; ++i)
            entry[i] = (unsigned int)-1;

        int finished = 0;
        do {
            // 搜索符合的集合
            if (m <= 1) {
                // 搜寻第一个匹配的数
                int found_first = 0;
                now = entry[m - 1];
                for (i=0; i<m; ++i) {
                    for (num=0; num<max_num; ++num) {
                        int skip = 0;
                        for (j=0; j<m; ++j) {
                            if (num == entry[j]) {
                                skip = 1;
                                break;
                            }
                        }
                        if (skip == 1)
                            continue;
                        val = num ^ now;
                        popcnt = popcount32(val);
                        if (popcnt == p) {
                            entry[m] = num;
                            m++;
                            found_first = 1;
                            break;
                        }
                    }
                    if (found_first == 1)
                        break;
                }
                if (m <= 1)
                    finished = 1;
            }
            else {
                // 继续搜索下一个匹配的数, m >= 2
                int length1, length2;
                length1 = 0;
                for (j=0; j<(int)max_num; ++j) {
                    entry_set1[j] = (unsigned int)-1;
                }
                
                for (i=0; i<m; ++i) {
                    // 搜寻一个数
                    length2 = 0;
                    for (j=0; j<(int)max_num; ++j) {
                        entry_set2[j] = (unsigned int)-1;
                    }
                    now = entry[i];
                    for (num=0; num<max_num; ++num) {
                        int skip = 0;
                        for (j=0; j<m; ++j) {
                            if (num == entry[j]) {
                                skip = 1;
                                break;
                            }
                        }
                        if (skip == 1)
                            continue;
                        val = num ^ now;
                        popcnt = popcount32(val);
                        if (popcnt == p) {
                            if (i == 0) {
                                entry_set1[length1] = num;
                                length1++;
                            }
                            else {
                                for (j=0; j<length1; ++j) {
                                    unsigned int prev = entry_set1[j];
                                    if (prev != (unsigned int)-1 && num == prev) {
                                        entry_set2[length2] = num;
                                        length2++;
                                    }
                                }
                            }
                        }
                    }
                    // 记录entry
                    if (i != 0 && length2 > 0) {
                        if (i < m - 1) {
                            for (j=0; j<(int)max_num; ++j) {
                                entry_set1[i] = (unsigned int)-1;
                            }
                            for (j=0; j<length2; ++j) {
                                entry_set1[i] = entry_set2[i];
                            }
                        }
                        else {
                            if (entry_set2[0] != (unsigned int)-1) {
                                entry[m] = entry_set2[0];
                                m++;
                            }
                        }
                    }
                    // 输出一组结果
                    if (i != 0 && length2 <= 0) {
                        printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
                        printf("m = %d, (n = %d, p = %d)\n\n", m, n, p);
                        system("pause");
                        for (j=0; j<m; ++j) {
                            num = entry[j];
                            for (k=n-1; k>=0; --k) {
                               if (((k+1) % 4) == 0)
                                   printf(" ");
                               if ((num & (1 << k)) != 0)
                                   printf("1");
                               else
                                   printf("0");
                            }
                            printf("\n");
                        }
                        printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
                    }
                    if ((i == 0 && length1 <= 0) || (i != 0 && length2 <= 0)) {
                        finished = 1;
                        break;
                    }
                }
            }
        } while (finished == 0);

        if (m > max) {
            max = m;
            for (j=0; j<(int)max_num; ++j) {
                entry_result[i] = (unsigned int)-1;
            }
            for (j=0; j<m; ++j) {
                entry_result[j]= entry[j];
            }
        }
        start++;
        //system("pause");
    }

    if (entry != NULL)
        free(entry);
    if (entry_set1 != NULL)
        free(entry_set1);
    if (entry_set2 != NULL)
        free(entry_set2);
    if (entry_result != NULL)
        free(entry_result);

    return max;
}

/// <comment>
/// 输出mask_list集合
/// </comment>
void HashMask_DisplayMaskList(int n, int p1, int p2, unsigned int *mask_list, unsigned int offset,
                              unsigned int length, unsigned int depth, unsigned int index, bool puase = true)
{
    unsigned int i;
    unsigned int num;
    int j;
    printf("\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    printf("m = %d, depth = %d, index = %d (n = %d, p1 = %d, p2 = %d)\n\n", length, depth, index, n, p1, p2);
    if (puase)
        system("pause");
    for (i=0; i<length; ++i) {
        num = mask_list[i + offset];
        for (j=n-1; j>=0; --j) {
           if (((j + 1) % 4) == 0)
               printf(" ");
           if ((num & (1 << j)) != 0)
               printf("1");
           else
               printf("0");
        }
        printf("\n");
    }
    printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
}

/// <comment>
/// 搜寻所有匹配的数的集合
/// </comment>
unsigned int HashMask_GetMaskSet32(int n, int p1, int p2, unsigned int mask_prev,
                                   unsigned int *mask_list, unsigned int length,
                                   unsigned int *mask_set, unsigned int set_sizes,
                                   unsigned int **mask_set_new)
{
    unsigned int set_sizes_new = 0;
    unsigned int count;
    unsigned int max_num = 1 << n;
    unsigned int max_mask = (1 << n) - 1;
    unsigned int i;

    unsigned int num, index;
    unsigned int hash_val;
    unsigned int *mask_set_list = NULL;
    unsigned int min_mask_list, max_mask_list;
    int popcnt;
    int skip;

    min_mask_list = max_mask;
    max_mask_list = 0;

    for (i=0; i<length; ++i) {
        num = mask_list[i + 1];
        if (num < min_mask_list)
            min_mask_list = num;
        if (num > max_mask_list)
            max_mask_list = num;
    }

    if (mask_set == NULL) {
        // 先计算集合的长度
        count = 0;
        for (num=0; num<max_num; ++num) {
            if (num == mask_prev)
                continue;
            if (num >= min_mask_list && num <= max_mask_list) {
                skip = 0;
                for (i=0; i<length; ++i) {
                    if (num == mask_list[i + 1]) {
                        skip = 1;
                        break;
                    }
                }
                if (skip == 1)
                    continue;
            }
            hash_val = num ^ mask_prev;
            popcnt = (int)popcount32(hash_val);
            if (popcnt >= p1 && popcnt <= p2) {
                count++;
            }
        }

        // 如果长度不为0, 则记录集合元素
        if (count > 0) {
            mask_set_list = (unsigned int *)malloc(count * sizeof(unsigned int));
            if (mask_set_list == NULL) {
                if (mask_set_new != NULL)
                    *mask_set_new = NULL;
                return 0;
            }

            for (num=0; num<max_num; ++num) {
                if (num == mask_prev)
                    continue;
                if (num >= min_mask_list && num <= max_mask_list) {
                    skip = 0;
                    for (i=0; i<length; ++i) {
                        if (num == mask_list[i + 1]) {
                            skip = 1;
                            break;
                        }
                    }
                    if (skip == 1)
                        continue;
                }
                hash_val = num ^ mask_prev;
                popcnt = (int)popcount32(hash_val);
                if (popcnt >= p1 && popcnt <= p2) {
                    mask_set_list[set_sizes_new] = num;
                    set_sizes_new++;
                }
            }

            if (mask_set_new != NULL)
                *mask_set_new = mask_set_list;
        }
        else {
            if (mask_set_new != NULL)
                *mask_set_new = NULL;
        }
    }
    else {
        // 先计算集合的长度
        count = 0;
        for (index=0; index<set_sizes; ++index) {
            num = mask_set[index];
            if (num == mask_prev)
                continue;
            if (num >= min_mask_list && num <= max_mask_list) {
                skip = 0;
                for (i=0; i<length; ++i) {
                    if (num == mask_list[i + 1]) {
                        skip = 1;
                        break;
                    }
                }
                if (skip == 1)
                    continue;
            }
            hash_val = num ^ mask_prev;
            popcnt = (int)popcount32(hash_val);
            if (popcnt >= p1 && popcnt <= p2) {
                count++;
            }
        }

        // 如果长度不为0, 则记录集合元素
        if (count > 0) {
            mask_set_list = (unsigned int *)malloc(count * sizeof(unsigned int));
            if (mask_set_list == NULL) {
                if (mask_set_new != NULL)
                    *mask_set_new = NULL;
                return 0;
            }

            for (index=0; index<set_sizes; ++index) {
                num = mask_set[index];
                if (num == mask_prev)
                    continue;
                if (num >= min_mask_list && num <= max_mask_list) {
                    skip = 0;
                    for (i=0; i<length; ++i) {
                        if (num == mask_list[i + 1]) {
                            skip = 1;
                            break;
                        }
                    }
                    if (skip == 1)
                        continue;
                }
                hash_val = num ^ mask_prev;
                popcnt = (int)popcount32(hash_val);
                if (popcnt >= p1 && popcnt <= p2) {
                    mask_set_list[set_sizes_new] = num;
                    set_sizes_new++;
                }
            }

            if (mask_set_new != NULL)
                *mask_set_new = mask_set_list;
        }
        else {
            if (mask_set_new != NULL)
                *mask_set_new = NULL;
        }
    }

#if HASHMASK_USE_RANDOM_LIST
    if (set_sizes_new > 0) {
        unsigned int *mask_set_rand;
        int rnd_index;
        mask_set_rand = (unsigned int *)malloc(set_sizes_new * sizeof(unsigned int));
        if (mask_set_rand == NULL)
            return 0;
        for (i=0; i<set_sizes_new; ++i) {
            rnd_index = sys_random::rand() % set_sizes_new;
            while (mask_set_list[rnd_index] == HASHMASK_EMPTY32) {
                rnd_index++;
                if (rnd_index >= (int)set_sizes_new)
                    rnd_index = 0;
            }
            if (rnd_index < (int)set_sizes_new) {
                mask_set_rand[i] = mask_set_list[rnd_index];
                mask_set_list[rnd_index] = HASHMASK_EMPTY32;
            }
        }
        if (mask_set_new != NULL) {
            if (mask_set_list != NULL)
                free(mask_set_list);
            *mask_set_new = mask_set_rand;
        }
    }
#endif

    return set_sizes_new;
}

int HashMask_FindList32(int n, int p1, int p2, unsigned int mask_prev,
                        unsigned int **pmask_list, unsigned int cap_sizes,
                        unsigned int *mask_set, unsigned int set_sizes,
                        unsigned int depth, unsigned int index)
{
    int ret = 0;
    unsigned int i;
    unsigned int max_num = 1 << n;
    unsigned int max_mask = (1 << n) - 1;
    //unsigned int num;

    unsigned int *mask_list = NULL;
    unsigned int *mask_set_new = NULL;
    unsigned int set_sizes_new;

    if (pmask_list == NULL)
        return 0;

    mask_list = *pmask_list;

    if (depth < 1)
        depth = 1;

    if (depth == 1) {

        mask_list[depth] = mask_prev;
        depth++;

        set_sizes_new = HashMask_GetMaskSet32(n, p1, p2, mask_prev, mask_list, depth - 1, mask_set, set_sizes, &mask_set_new);
        if (set_sizes_new > 0) {
            mask_prev = mask_set_new[0];
            mask_list[depth] = mask_prev;
            depth++;

            // 重新调整mask_list的大小
            if (depth >= cap_sizes) {
                cap_sizes += 256;
                unsigned int *mask_list_new;
                mask_list_new = (unsigned int *)malloc(cap_sizes * sizeof(unsigned int));
                if (mask_list_new == NULL)
                    return -1;

                for (i=0; i<depth; ++i)
                    mask_list_new[i] = mask_list[i];
                for (i=depth; i<cap_sizes; ++i)
                    mask_list_new[i] = HASHMASK_EMPTY32;

                free(mask_list);
                mask_list = mask_list_new;
                *pmask_list = mask_list;
            }

            ret = HashMask_FindList32(n, p1, p2, mask_prev, pmask_list, cap_sizes, mask_set_new, set_sizes_new, depth, 0);
            if (pmask_list != NULL)
                mask_list = *pmask_list;
            depth--;
            mask_list[depth] = HASHMASK_EMPTY32;
        }

        depth--;
        mask_list[depth] = HASHMASK_EMPTY32;
    }
    else {
        set_sizes_new = HashMask_GetMaskSet32(n, p1, p2, mask_prev, mask_list, depth - 1, mask_set, set_sizes, &mask_set_new);
        if (set_sizes_new > 0) {

            for (index=0; index<set_sizes_new; ++index) {
                mask_prev = mask_set_new[index];
                mask_list[depth] = mask_prev;
                depth++;

                // 重新调整mask_list的大小
                if (depth >= cap_sizes) {
                    cap_sizes += 256;
                    unsigned int *mask_list_new;
                    mask_list_new = (unsigned int *)malloc(cap_sizes * sizeof(unsigned int));
                    if (mask_list_new == NULL)
                        return -1;

                    for (i=0; i<depth; ++i)
                        mask_list_new[i] = mask_list[i];
                    for (i=depth; i<cap_sizes; ++i)
                        mask_list_new[i] = HASHMASK_EMPTY32;

                    free(mask_list);
                    mask_list = mask_list_new;
                    *pmask_list = mask_list;
                }

                ret = HashMask_FindList32(n, p1, p2, mask_prev, pmask_list, cap_sizes, mask_set_new, set_sizes_new, depth, index);
                if (pmask_list != NULL)
                    mask_list = *pmask_list;
                depth--;
                mask_list[depth] = HASHMASK_EMPTY32;
                if (ret < 0)
                    break;
            }

            if (mask_set_new != NULL)
                free(mask_set_new);

            if (ret >= 0)
                ret = 1;
        }
    }

    if (set_sizes_new <= 0) {
        // 找到一组mask_list集合
        unsigned int length = depth - 1;
        if (length > max_mask_length) {
            mask_list[0] = length;
            max_mask_length = length;
            max_mask_count = 0;

            if (mask_list_max != NULL)
                free(mask_list_max);
            mask_list_max = (unsigned int *)malloc(length * sizeof(unsigned int));
            if (mask_list_max == NULL)
                return -1;

            for (i=0; i<length; ++i)
                mask_list_max[i] = mask_list[i + 1];

            // 输出mask_list集合
            HashMask_DisplayMaskList(n, p1, p2, mask_list_max, 0, length, depth, index, false);
        }

        max_mask_count++;
        for (i=0; i<4; ++i)
            printf("\b\b\b\b\b\b\b\b\b\b");
        printf("max_mask_count = %d\t\t", max_mask_count);

#if HASHMASK_USE_RANDOM_LIST
        if (((max_mask_count % 32) == 0) && (max_mask_count != 0)) {
            // do nothing
#else
        if (((max_mask_count % 4096) == 0) && (max_mask_count != 0)) {
            // 输出mask_list集合
            HashMask_DisplayMaskList(n, p1, p2, mask_list, 1, length, depth, index, false);
#endif
            ret = -1;
            max_mask_count = 0;
        }

        // 输出mask_list集合
        //HashMask_DisplayMaskList(n, p1, p2, mask_list, 1, length, depth, index);
    }

    return ret;
}

unsigned int HashMask_GetMaxLength32(int n, int p1, int p2, unsigned int start)
{
    unsigned int nMaxLength = 0, length;
    unsigned int i;
    unsigned int max_num = 1 << n;
    unsigned int max_mask = (1 << n) - 1;

    unsigned int cap_sizes = MIN(256, max_num);
    unsigned int *mask_list;
    mask_list = (unsigned int *)malloc(cap_sizes * sizeof(unsigned int));
    if (mask_list == NULL)
        return -1;

    for (i=0; i<cap_sizes; ++i)
        mask_list[i] = HASHMASK_EMPTY32;

    mask_list_max = NULL;
    mask_list[0] = 0;

    unsigned int end, num;
    unsigned int hash_val, mask_prev;
    int popcnt;

#if HASHMASK_USE_RANDOM_LIST
    int pause = 0;
    int loop;
    for (loop=0; loop<32; loop++) {
        mask_prev = start;
        max_mask_count = 0;
#endif

#if HASHMASK_USE_RANDOM_START
        start = sys_random::rand() & ((1 << ((n / 3) + 1)) - 1);
#endif
        // get the end of search
        end = start;
        mask_prev = start;
        for (num=0; num<max_num; ++num) {
            hash_val = num ^ mask_prev;
            popcnt = (int)popcount32(hash_val);
            if (popcnt >= p1 && popcnt <= p2 && num > start) {
                end = num;
                break;
            }
        }

        do {
            int ret = HashMask_FindList32(n, p1, p2, mask_prev, &mask_list, cap_sizes, NULL, 0, 1, 0);
            if (ret == 1 || ret < 0) {
                length = mask_list[0];
                if (length > nMaxLength) {
                    nMaxLength = length;
                    max_mask_length = length;

#if HASHMASK_USE_RANDOM_LIST
                    pause = 1;

                    // 输出mask_list集合
                    HashMask_DisplayMaskList(n, p1, p2, mask_list_max, 0, length, 0, 0, false);
#endif
                }

#if HASHMASK_USE_RANDOM_LIST
                if (pause != 1)
                    printf("\n\n");

                printf(">>> loop = %d, max_mask_length = %d.\n\n", loop, max_mask_length);
                if (pause == 1) {
                    Beep(262, 200);
                    Sleep(2000);
                    //system("pause");
                    printf("\n");
                    pause = 0;
                }
#else
                // 输出mask_list集合
                HashMask_DisplayMaskList(n, p1, p2, mask_list_max, 0, length, 0, 0, false);
                max_mask_length = 0;
#endif
            }
            // search next num
            mask_prev++;
        } while (mask_prev <= end);
#if HASHMASK_USE_RANDOM_LIST
    }

    // 输出mask_list集合
    HashMask_DisplayMaskList(n, p1, p2, mask_list_max, 0, length, 0, 0, false);
#endif
    system("pause");

    nMaxLength = mask_list[0];

    if (mask_list != NULL)
        free(mask_list);

    if (mask_list_max != NULL)
        free(mask_list_max);

    return nMaxLength;
}

unsigned int HashMask_GetMaxLength64(int n, int p1, int p2, uint64 start)
{
    int nMaxLength = 0;
    return nMaxLength;
}

unsigned int HashMask_GetMaxLength(int n, int p1, int p2, uint64 start)
{
    int temp;
    if (p1 > p2) {
        temp = p1;
        p1 = p2;
        p2 = temp;
    }

#if HASHMASK_USE_RANDOM_START
    start = sys_random::rand() & ((1 << ((n / 3) + 1)) - 1);
#endif

    if (n <= 32)
        return HashMask_GetMaxLength32(n, p1, p2, (unsigned int)start);
    else
        return HashMask_GetMaxLength64(n, p1, p2, start);
}

}  // namespace dolphin
