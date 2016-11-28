/* 
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  trans.c in terms of its data cache performance.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "cachelab.h"

#include <string.h>

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Declaration of struct, variables and functions
 * ■■■■                                                                 ■■■■■■ */
#define MISS 10
#define HIT 20
#define MISS_AND_EVICTION 30
#define MISS_AND_HIT 40
#define MISS_EVICTION_AND_HIT 50

typedef struct {
	int valid; 	/* valid bit per line */
	int tag; 	/* tag of a line */
	int access_time; 	/* 取值大小为1~line_num, 为1时说明是要evict的 */
}line;
typedef struct {
	line *lines;
}set;
typedef struct {
	int set_num; 	/* cache sets num */
	int line_num; 	/* lines per set */
	set *sets;
}simu_cache;

int flag = 0;

int initCache(simu_cache *cache, int s, int E);
int parse_traces(simu_cache *cache, char *line_buf, int s, int E, int b, int flag);
int getSetBits(int addr, int s, int b);
int getTagBits(int addr, int s, int b);
int updateAccessTime(simu_cache *cache, int selset, int cl);


/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
    char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Implementation in Main function
 * ■■■■                                                                 ■■■■■■ */

    FILE *pf;
	char line_buf[100];

	int state;
	simu_cache cache;

	initCache(&cache, s, E);
	pf = fopen(trace_file, "r");
	if (!pf) {
		printf("Error: Cann't open file %s!\n", trace_file);
		return -1;
	}
	while (NULL != fgets(line_buf, 100, pf)) {
		if (' ' == line_buf[0]) {
			line_buf[strlen(line_buf)-1] = '\0';
			state = parse_traces(&cache, line_buf, s, E, b, flag);
			if (1 == flag) {
				switch (state) {
				case HIT:
					printf("%s hit\n", line_buf+1);
					break;
				case MISS:
					printf("%s miss\n", line_buf+1);
					break;
				case MISS_AND_HIT:
					printf("%s miss hit\n", line_buf+1);
					break;
				case MISS_AND_EVICTION:
					printf("%s miss eviction\n", line_buf+1);
					break;
				case MISS_EVICTION_AND_HIT:
					printf("%s miss eviction hit\n", line_buf+1);
					break;
				default:
					break;
				}
			}
		}
	}
	fclose(pf);

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Implementation of custom functions
 * ■■■■                                                                 ■■■■■■ */

int initCache(simu_cache *cache, int s, int E)
{
	int i, j;
	cache->set_num = (2 << s);
	cache->line_num = E;
	cache->sets = (set *)malloc(cache->set_num * sizeof(set));
	if (!cache->sets) {
		fprintf(stderr, "Memory error!\n");
		exit(-1);
	}
	for (i = 0; i < cache->set_num; i++) {
		cache->sets[i].lines = (line *)malloc(cache->line_num * sizeof(line));
		if (!cache->sets[i].lines) {
			fprintf(stderr, "Memory Error!\n");
			exit(-1);
		}
		for (j = 0; j < cache->line_num; j++) {
			cache->sets[i].lines[j].valid = 0;
		}
	}
	return 0;
}

/**
 * 分析内存读取命令
 */
int parse_traces(simu_cache *cache, char *line_buf, int s, int E, int b, int flag)
{
	int i;
	char opt;
	int addr;
	int selset, tag;
	sscanf(line_buf, " %c %x", &opt, &addr);
	selset = getSetBits(addr, s, b);
	tag = getTagBits(addr, s, b);
	for (i = 0; i < cache->line_num; i++) {
		if (1 == cache->sets[selset].lines[i].valid && 
				tag == cache->sets[selset].lines[i].tag) {
			/* Hit immediately */
			if ('M' == opt) {
				++hit_count;
				++hit_count;
			} else {
				++hit_count;
			}
			updateAccessTime(cache, selset, i);
			return HIT;
		}
	}

	/* Not Hit */
	++miss_count;
	for (i = 0; i < cache->line_num; i++) {
		if (0 == cache->sets[selset].lines[i].valid) { 				
			/* there is a empty line, No eviction */
			cache->sets[selset].lines[i].valid = 1;
			cache->sets[selset].lines[i].tag = tag;
			updateAccessTime(cache, selset, i);
			if ('M' == opt) {
				++hit_count;
				return MISS_AND_HIT;
			} else {
				return MISS;	
			}
		}
	}

	/* Need eviction */
	++eviction_count;
	for (i = 0; i < cache->line_num; i++) {
		if (1 == cache->sets[selset].lines[i].access_time) {
			cache->sets[selset].lines[i].valid = 1;
			cache->sets[selset].lines[i].tag = tag;
			updateAccessTime(cache, selset, i);
			if ('M' == opt) {
				++hit_count;
				return MISS_EVICTION_AND_HIT;
			} else {
				return MISS_AND_EVICTION;
			}
		}

	}

	return 0;
}

/**
 * 返回地址addr选择的容器set序号
 */
int getSetBits(int addr, int s, int b)
{
	int mask;
	mask = 0x7fffffff >> (31 - s);
	addr = addr >> b;
	return (mask & addr);
}

/**
 * 得到addr的tag位
 */
int getTagBits(int addr, int s, int b)
{
	int mask;
	mask = 0x7fffffff >> (31 - s - b);
	addr = addr >> (s + b);
	return (mask & addr);
}

/**
 * 更新lines的访问时间
 */
int updateAccessTime(simu_cache *cache, int selset, int cl)
{
	int i;
	for (i = 0; i < cache->line_num; i++) {
		if (1 == cache->sets[selset].lines[i].valid && 
			cache->sets[selset].lines[i].access_time > 
				cache->sets[selset].lines[cl].access_time) {
			--cache->sets[selset].lines[i].access_time;	
		}
	}
	cache->sets[selset].lines[cl].access_time = cache->line_num;
	return 0;
}

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */