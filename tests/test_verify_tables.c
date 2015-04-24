#include "config.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "kseq.h"
KSEQ_INIT(int, read)

#include "parasail.h"
#include "parasail_internal.h"
#include "parasail_cpuid.h"

#include "blosum_lookup.h"
#include "function_lookup.h"

static int verbose = 0;

typedef struct gap_score {
    int open;
    int extend;
} gap_score_t;

gap_score_t gap_scores[] = {
    {10,1},
    {10,2},
    {14,2},
    {40,2},
    {INT_MIN,INT_MIN}
};

static inline void parse_sequences(
        const char *filename,
        char ***strings_,
        unsigned long **sizes_,
        unsigned long *count_)
{
    FILE* fp;
    kseq_t *seq = NULL;
    int l = 0;
    char **strings = NULL;
    unsigned long *sizes = NULL;
    unsigned long count = 0;
    unsigned long memory = 1000;

    fp = fopen(filename, "r");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }
    strings = malloc(sizeof(char*) * memory);
    sizes = malloc(sizeof(unsigned long) * memory);
    seq = kseq_init(fileno(fp));
    while ((l = kseq_read(seq)) >= 0) {
        strings[count] = strdup(seq->seq.s);
        if (NULL == strings[count]) {
            perror("strdup");
            exit(1);
        }
        sizes[count] = seq->seq.l;
        ++count;
        if (count >= memory) {
            char **new_strings = NULL;
            unsigned long *new_sizes = NULL;
            memory *= 2;
            new_strings = realloc(strings, sizeof(char*) * memory);
            if (NULL == new_strings) {
                perror("realloc");
                exit(1);
            }
            strings = new_strings;
            new_sizes = realloc(sizes, sizeof(unsigned long) * memory);
            if (NULL == new_sizes) {
                perror("realloc");
                exit(1);
            }
            sizes = new_sizes;
        }
    }
    kseq_destroy(seq);
    fclose(fp);

    *strings_ = strings;
    *sizes_ = sizes;
    *count_ = count;
}

static inline unsigned long binomial_coefficient(
        unsigned long n,
        unsigned long k)
{
    /* from http://blog.plover.com/math/choose.html */
    unsigned long r = 1;
    unsigned long d;
    if (k > n) {
        return 0;
    }
    for (d = 1; d <= k; d++) {
        r *= n--;
        r /= d;
    }
    return r;
}

static inline void k_combination2(
        unsigned long pos,
        unsigned long *a,
        unsigned long *b)
{
    double s;
    double i = floor(sqrt(2.0 * pos)) - 1.0;
    if (i <= 1.0) {
        i = 1.0;
    }
    s = i * (i - 1.0) / 2.0;
    while (pos - s >= i) {
        s += i;
        i += 1;
    }
    *a = (unsigned long)(pos - s);
    *b = (unsigned long)(i);
}

static inline int diff_array(
        unsigned long s1Len,
        unsigned long s2Len,
        int *a,
        int *b)
{
    unsigned long i = 0;
    unsigned long size = s1Len * s2Len;
    for (i=0; i<size; ++i) {
        if (a[i] != b[i]) return 1;
    }
    return 0;
}

static void check_functions(
        funcs_t f,
        char **sequences,
        unsigned long *sizes,
        unsigned long pair_limit)
{
    func_t *functions = f.fs;
    unsigned long blosum_index = 0;
    unsigned long gap_index = 0;
    unsigned long function_index = 0;
    unsigned long pair_index = 0;
    parasail_function_t reference_function = NULL;

    printf("checking %s functions\n", f.name);
    for (blosum_index=0; NULL!=blosums[blosum_index].pointer; ++blosum_index) {
        //printf("\t%s\n", blosums[blosum_index].name);
        for (gap_index=0; INT_MIN!=gap_scores[gap_index].open; ++gap_index) {
            int open = gap_scores[gap_index].open;
            int extend = gap_scores[gap_index].extend;
            //printf("\t\topen=%d extend=%d\n", open, extend);
            reference_function = functions[0].pointer;
            for (function_index=1;
                    NULL!=functions[function_index].pointer;
                    ++function_index) {
                //printf("\t\t\t%s\n", functions[function_index].name);
                unsigned long saturated = 0;
#pragma omp parallel for
                for (pair_index=0; pair_index<pair_limit; ++pair_index) {
                    parasail_result_t *reference_result = NULL;
                    parasail_result_t *result = NULL;
                    unsigned long a = 0;
                    unsigned long b = 1;
                    k_combination2(pair_index, &a, &b);
                    //printf("\t\t\t\tpair=%lu (%lu,%lu)\n", pair_index, a, b);
                    reference_result = reference_function(
                            sequences[a], sizes[a],
                            sequences[b], sizes[b],
                            open, extend,
                            blosums[blosum_index].pointer);
                    result = functions[function_index].pointer(
                            sequences[a], sizes[a],
                            sequences[b], sizes[b],
                            open, extend,
                            blosums[blosum_index].pointer);
                    if (result->saturated) {
                        /* no point in comparing a result that saturated */
                        parasail_result_free(reference_result);
                        parasail_result_free(result);
#pragma omp atomic
                        saturated += 1;
                        continue;
                    }
                    if (reference_result->score != result->score) {
#pragma omp critical(printer)
                        {
                            printf("%s(%lu,%lu,%d,%d,%s) wrong score (%d!=%d)\n",
                                    functions[function_index].name,
                                    a, b, open, extend,
                                    blosums[blosum_index].name,
                                    reference_result->score, result->score);
                        }
                    }
                    if (diff_array(
                                sizes[a], sizes[b],
                                reference_result->score_table,
                                result->score_table)) {
#pragma omp critical(printer)
                        {
                            printf("%s(%lu,%lu,%d,%d,%s) bad score table\n",
                                    functions[function_index].name,
                                    a, b, open, extend,
                                    blosums[blosum_index].name);
                        }
                    }
                    if (reference_result->matches_table
                            && diff_array(
                                sizes[a], sizes[b],
                                reference_result->matches_table,
                                result->matches_table)) {
#pragma omp critical(printer)
                        {
                            printf("%s(%lu,%lu,%d,%d,%s) bad matches table\n",
                                    functions[function_index].name,
                                    a, b, open, extend,
                                    blosums[blosum_index].name);
                        }
                    }
                    if (reference_result->similar_table
                            && diff_array(
                                sizes[a], sizes[b],
                                reference_result->similar_table,
                                result->similar_table)) {
#pragma omp critical(printer)
                        {
                            printf("%s(%lu,%lu,%d,%d,%s) bad similar table\n",
                                    functions[function_index].name,
                                    a, b, open, extend,
                                    blosums[blosum_index].name);
                        }
                    }
                    if (reference_result->length_table
                            && diff_array(
                                sizes[a], sizes[b],
                                reference_result->length_table,
                                result->length_table)) {
#pragma omp critical(printer)
                        {
                            printf("%s(%lu,%lu,%d,%d,%s) bad length table\n",
                                    functions[function_index].name,
                                    a, b, open, extend,
                                    blosums[blosum_index].name);
                        }
                    }
                    parasail_result_free(reference_result);
                    parasail_result_free(result);
                }
                if (verbose && saturated) {
                    printf("%s %d %d %s saturated %lu times\n",
                            functions[function_index].name,
                            open, extend,
                            blosums[blosum_index].name,
                            saturated);
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    unsigned long i = 0;
    unsigned long seq_count = 0;
    unsigned long limit = 0;
    char **sequences = NULL;
    unsigned long *sizes = NULL;
    char *endptr = NULL;
    char *filename = NULL;
    int c = 0;
    int test_scores = 1;
    int test_stats = 0;

    while ((c = getopt(argc, argv, "f:n:vsS")) != -1) {
        switch (c) {
            case 'f':
                filename = optarg;
                break;
            case 'n':
                errno = 0;
                seq_count = strtol(optarg, &endptr, 10);
                if (errno) {
                    perror("strtol");
                    exit(1);
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                test_stats = 1;
                break;
            case 'S':
                test_scores = 0;
                break;
            case '?':
                if (optopt == 'f' || optopt == 'n') {
                    fprintf(stderr,
                            "Option -%c requires an argument.\n",
                            optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n",
                            optopt);
                }
                else {
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                }
                exit(1);
            default:
                fprintf(stderr, "default case in getopt\n");
                exit(1);
        }
    }

    if (filename) {
        parse_sequences(filename, &sequences, &sizes, &seq_count);
    }
    else {
        fprintf(stderr, "no filename specified\n");
        exit(1);
    }

    limit = binomial_coefficient(seq_count, 2);
    printf("%lu choose 2 is %lu\n", seq_count, limit);


#if HAVE_SSE2
    if (parasail_can_use_sse2()) {
        if (test_scores) {
            check_functions(nw_table_sse2, sequences, sizes, limit);
            check_functions(sg_table_sse2, sequences, sizes, limit);
            check_functions(sw_table_sse2, sequences, sizes, limit);
        }
        if (test_stats) {
            check_functions(nw_stats_table_sse2, sequences, sizes, limit);
            check_functions(sg_stats_table_sse2, sequences, sizes, limit);
            check_functions(sw_stats_table_sse2, sequences, sizes, limit);
        }
    }
#endif

#if HAVE_SSE41
    if (parasail_can_use_sse41()) {
        if (test_scores) {
            check_functions(nw_table_sse41, sequences, sizes, limit);
            check_functions(sg_table_sse41, sequences, sizes, limit);
            check_functions(sw_table_sse41, sequences, sizes, limit);
        }
        if (test_stats) {
            check_functions(nw_stats_table_sse41, sequences, sizes, limit);
            check_functions(sg_stats_table_sse41, sequences, sizes, limit);
            check_functions(sw_stats_table_sse41, sequences, sizes, limit);
        }
    }
#endif

#if HAVE_AVX2
    if (parasail_can_use_avx2()) {
        if (test_scores) {
            check_functions(nw_table_avx2, sequences, sizes, limit);
            check_functions(sg_table_avx2, sequences, sizes, limit);
            check_functions(sw_table_avx2, sequences, sizes, limit);
        }
        if (test_stats) {
            check_functions(nw_stats_table_avx2, sequences, sizes, limit);
            check_functions(sg_stats_table_avx2, sequences, sizes, limit);
            check_functions(sw_stats_table_avx2, sequences, sizes, limit);
        }
    }
#endif

#if HAVE_KNC
    {
        if (test_scores) {
            check_functions(nw_table_knc, sequences, sizes, limit);
            check_functions(sg_table_knc, sequences, sizes, limit);
            check_functions(sw_table_knc, sequences, sizes, limit);
        }
        if (test_stats) {
            check_functions(nw_stats_table_knc, sequences, sizes, limit);
            check_functions(sg_stats_table_knc, sequences, sizes, limit);
            check_functions(sw_stats_table_knc, sequences, sizes, limit);
        }
    }
#endif

    for (i=0; i<seq_count; ++i) {
        free(sequences[i]);
    }
    free(sequences);
    free(sizes);

    return 0;
}

