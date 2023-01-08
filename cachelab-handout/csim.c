/*
 * Name: 马江岩
 * ID: 2000012707
 *
 * csim.c - A cache simulator that takes a valgrind memory trace as input,
 *     simulates the hit/miss behaviour of a cache memory on this trace, and
 *     outputs the total number of hits, misses, and evictions.
 */

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

int s, E, b, hit_count = 0, miss_count = 0, eviction_count = 0;
bool v = false;
FILE *trace;
typedef struct
{
    int time;
    unsigned tag;
} block;
block *cache;

/*
 * print_help - Print usage info.
 */
void print_help(char *file)
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", file);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

/*
 * update_cache - Update cache memory and hit/miss/eviction count.
 */
void update_cache(char operation, unsigned long address, unsigned size,
                  int time)
{
    unsigned tag = address >> b >> s;
    int set = address >> b ^ tag << s;
    block *cache_row = cache + E * set;
    block *evicted = cache_row;
    if (operation == 'M')hit_count++;
    for (int i = 0; i < E; i++)
        if (cache_row[i].time && cache_row[i].tag == tag)
        {
            hit_count++;
            cache_row[i].time = time;
            if (v && operation == 'M')
                printf("%c %lx,%d hit hit\n", operation, address, size);
            else if (v)
                printf("%c %lx,%d hit\n", operation, address, size);
            return;
        }
        else if (!cache_row[i].time)
        {
            miss_count++;
            cache_row[i].time = time;
            cache_row[i].tag = tag;
            if (v && operation == 'M')
                printf("%c %lx,%d miss hit\n", operation, address, size);
            else if (v)
                printf("%c %lx,%d miss\n", operation, address, size);
            return;
        }
        else if (cache_row[i].time < evicted->time)
            evicted = cache_row + i;
    miss_count++;
    eviction_count++;
    evicted->time = time;
    evicted->tag = tag;
    if (v && operation == 'M')
        printf("%c %lx,%d miss eviction hit\n", operation, address, size);
    else if (v)
        printf("%c %lx,%d miss eviction\n", operation, address, size);
}

/*
 * main - Read command line arguments, calculate hit/miss/eviction count and
 *     print them out.
 */
int main(int argc, char *argv[])
{
    int opt;
    bool missing_s = true, missing_E = true, missing_b = true;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
        switch (opt)
        {
        case 'h':
            print_help(argv[0]);
            return 0;
        case 'v':
            v = true;
            break;
        case 's':
            s = atoi(optarg);
            if (s > 0)missing_s = false;
            break;
        case 'E':
            E = atoi(optarg);
            if (E > 0)missing_E = false;
            break;
        case 'b':
            b = atoi(optarg);
            if (b > 0)missing_b = false;
            break;
        case 't':
            trace = fopen(optarg, "r");
            if (trace == NULL)
            {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                exit(1);
            }
            break;
        case '?':
            print_help(argv[0]);
            exit(1);
        }
    if (missing_s || missing_E || missing_b)
    {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_help(argv[0]);
        exit(1);
    }
    cache = (block *)malloc(sizeof(block) * E << s);
    if (cache == NULL)exit(1);
    memset(cache, 0, sizeof(block) * E << s);
    char operation;
    unsigned long address;
    unsigned size;
    int time = 0;
    while (fscanf(trace, "%s%lx,%d\n", &operation, &address, &size) > 0)
        update_cache(operation, address, size, ++time);
    fclose(trace);
    free(cache);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
