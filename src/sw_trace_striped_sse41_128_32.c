/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2015 Battelle Memorial Institute.
 */
#include "config.h"

#include <stdint.h>
#include <stdlib.h>

#include <emmintrin.h>
#include <smmintrin.h>

#include "parasail.h"
#include "parasail/memory.h"
#include "parasail/internal_sse.h"

#define SWAP(A,B) { __m128i* tmp = A; A = B; B = tmp; }
#define SWAP3(A,B,C) { __m128i* tmp = A; A = B; B = C; C = tmp; }

#define NEG_INF (INT32_MIN/(int32_t)(2))

static inline int32_t _mm_hmax_epi32_rpl(__m128i a) {
    a = _mm_max_epi32(a, _mm_srli_si128(a, 8));
    a = _mm_max_epi32(a, _mm_srli_si128(a, 4));
    return _mm_extract_epi32(a, 0);
}


static inline void arr_store(
        __m128i *array,
        __m128i vH,
        int32_t t,
        int32_t seglen,
        int32_t d)
{
    _mm_store_si128(array + (d*seglen+t), vH);
}

#define FNAME parasail_sw_trace_striped_sse41_128_32
#define PNAME parasail_sw_trace_striped_profile_sse41_128_32

parasail_result_t* FNAME(
        const char * const restrict s1, const int s1Len,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap, const parasail_matrix_t *matrix)
{
    parasail_profile_t *profile = parasail_profile_create_sse_128_32(s1, s1Len, matrix);
    parasail_result_t *result = PNAME(profile, s2, s2Len, open, gap);
    parasail_profile_free(profile);
    return result;
}

parasail_result_t* PNAME(
        const parasail_profile_t * const restrict profile,
        const char * const restrict s2, const int s2Len,
        const int open, const int gap)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    int32_t end_query = 0;
    int32_t end_ref = 0;
    const int s1Len = profile->s1Len;
    const parasail_matrix_t *matrix = profile->matrix;
    const int32_t segWidth = 4; /* number of values in vector unit */
    const int32_t segLen = (s1Len + segWidth - 1) / segWidth;
    __m128i* const restrict vProfile = (__m128i*)profile->profile32.score;
    __m128i* restrict pvHStore = parasail_memalign___m128i(16, segLen);
    __m128i* restrict pvHLoad =  parasail_memalign___m128i(16, segLen);
    __m128i* const restrict pvE = parasail_memalign___m128i(16, segLen);
    __m128i* restrict pvEaStore = parasail_memalign___m128i(16, segLen);
    __m128i* restrict pvEaLoad = parasail_memalign___m128i(16, segLen);
    __m128i* const restrict pvHT = parasail_memalign___m128i(16, segLen);
    __m128i* restrict pvHMax = parasail_memalign___m128i(16, segLen);
    __m128i vGapO = _mm_set1_epi32(open);
    __m128i vGapE = _mm_set1_epi32(gap);
    __m128i vZero = _mm_setzero_si128();
    int32_t score = NEG_INF;
    __m128i vMaxH = vZero;
    __m128i vMaxHUnit = vZero;
    int32_t maxp = INT32_MAX - (int32_t)(matrix->max+1);
    /*int32_t stop = profile->stop == INT32_MAX ?  INT32_MAX : (int32_t)profile->stop;*/
    //parasail_result_t *result = parasail_result_new_trace(segLen*segWidth, s2Len, 4);
    parasail_result_t *result = parasail_result_new_trace(segLen, s2Len, sizeof(__m128i));
    __m128i vTZero = _mm_set1_epi32(PARASAIL_ZERO);
    __m128i vTIns  = _mm_set1_epi32(PARASAIL_INS);
    __m128i vTDel  = _mm_set1_epi32(PARASAIL_DEL);
    __m128i vTDiag = _mm_set1_epi32(PARASAIL_DIAG);

    /* initialize H and E */
    parasail_memset___m128i(pvHStore, vZero, segLen);
    parasail_memset___m128i(pvE, _mm_set1_epi32(-open), segLen);
    parasail_memset___m128i(pvEaStore, _mm_set1_epi32(-open), segLen);

    for (i=0; i<segLen; ++i) {
        arr_store(result->trace_ins_table, vTDiag, i, segLen, 0);
    }

    /* outer loop over database sequence */
    for (j=0; j<s2Len; ++j) {
        __m128i vEF_opn;
        __m128i vE;
        __m128i vE_ext;
        __m128i vF;
        __m128i vF_ext;
        __m128i vFa;
        __m128i vFa_ext;
        __m128i vH;
        __m128i vH_dag;
        const __m128i* vP = NULL;

        /* Initialize F value to 0.  Any errors to vH values will be
         * corrected in the Lazy_F loop. */
        vF = _mm_sub_epi32(vZero,vGapO);

        /* load final segment of pvHStore and shift left by 4 bytes */
        vH = _mm_load_si128(&pvHStore[segLen - 1]);
        vH = _mm_slli_si128(vH, 4);

        /* Correct part of the vProfile */
        vP = vProfile + matrix->mapper[(unsigned char)s2[j]] * segLen;

        if (end_ref == j-2) {
            /* Swap in the max buffer. */
            SWAP3(pvHMax,  pvHLoad,  pvHStore)
            SWAP(pvEaLoad,  pvEaStore)
        }
        else {
            /* Swap the 2 H buffers. */
            SWAP(pvHLoad,  pvHStore)
            SWAP(pvEaLoad,  pvEaStore)
        }

        /* inner loop to process the query sequence */
        for (i=0; i<segLen; ++i) {
            vE = _mm_load_si128(pvE + i);

            /* Get max from vH, vE and vF. */
            vH_dag = _mm_add_epi32(vH, _mm_load_si128(vP + i));
            vH_dag = _mm_max_epi32(vH_dag, vZero);
            vH = _mm_max_epi32(vH_dag, vE);
            vH = _mm_max_epi32(vH, vF);
            /* Save vH values. */
            _mm_store_si128(pvHStore + i, vH);

            {
                __m128i cond_zero = _mm_cmpeq_epi32(vH, vZero);
                __m128i case1 = _mm_cmpeq_epi32(vH, vH_dag);
                __m128i case2 = _mm_cmpeq_epi32(vH, vF);
                __m128i vT = _mm_blendv_epi8(
                        _mm_blendv_epi8(vTIns, vTDel, case2),
                        _mm_blendv_epi8(vTDiag, vTZero, cond_zero),
                        case1);
                _mm_store_si128(pvHT + i, vT);
                arr_store(result->trace_table, vT, i, segLen, j);
            }
            vMaxH = _mm_max_epi32(vH, vMaxH);
            vEF_opn = _mm_sub_epi32(vH, vGapO);

            /* Update vE value. */
            vE_ext = _mm_sub_epi32(vE, vGapE);
            vE = _mm_max_epi32(vEF_opn, vE_ext);
            _mm_store_si128(pvE + i, vE);
            {
                __m128i vEa = _mm_load_si128(pvEaLoad + i);
                __m128i vEa_ext = _mm_sub_epi32(vEa, vGapE);
                vEa = _mm_max_epi32(vEF_opn, vEa_ext);
                _mm_store_si128(pvEaStore + i, vEa);
                if (j+1<s2Len) {
                    __m128i cond = _mm_cmpgt_epi32(vEF_opn, vEa_ext);
                    __m128i vT = _mm_blendv_epi8(vTIns, vTDiag, cond);
                    arr_store(result->trace_ins_table, vT, i, segLen, j+1);
                }
            }

            /* Update vF value. */
            vF_ext = _mm_sub_epi32(vF, vGapE);
            vF = _mm_max_epi32(vEF_opn, vF_ext);
            {
                __m128i cond = _mm_cmpgt_epi32(vEF_opn, vF_ext);
                __m128i vT = _mm_blendv_epi8(vTDel, vTDiag, cond);
                if (i+1<segLen) {
                    arr_store(result->trace_del_table, vT, i+1, segLen, j);
                }
            }

            /* Load the next vH. */
            vH = _mm_load_si128(pvHLoad + i);
        }

        /* Lazy_F loop: has been revised to disallow adjecent insertion and
         * then deletion, so don't update E(i, i), learn from SWPS3 */
        vFa_ext = vF_ext;
        vFa = vF;
        for (k=0; k<segWidth; ++k) {
            __m128i vHp = _mm_load_si128(&pvHLoad[segLen - 1]);
            vHp = _mm_slli_si128(vHp, 4);
            vEF_opn = _mm_slli_si128(vEF_opn, 4);
            vEF_opn = _mm_insert_epi32(vEF_opn, -open, 0);
            vF_ext = _mm_slli_si128(vF_ext, 4);
            vF_ext = _mm_insert_epi32(vF_ext, NEG_INF, 0);
            vF = _mm_slli_si128(vF, 4);
            vF = _mm_insert_epi32(vF, -open, 0);
            vFa_ext = _mm_slli_si128(vFa_ext, 4);
            vFa_ext = _mm_insert_epi32(vFa_ext, NEG_INF, 0);
            vFa = _mm_slli_si128(vFa, 4);
            vFa = _mm_insert_epi32(vFa, -open, 0);
            for (i=0; i<segLen; ++i) {
                vH = _mm_load_si128(pvHStore + i);
                vH = _mm_max_epi32(vH,vF);
                _mm_store_si128(pvHStore + i, vH);
                {
                    __m128i vT;
                    __m128i case1;
                    __m128i case2;
                    __m128i cond;
                    vHp = _mm_add_epi32(vHp, _mm_load_si128(vP + i));
                    vHp = _mm_max_epi32(vHp, vZero);
                    case1 = _mm_cmpeq_epi32(vH, vHp);
                    case2 = _mm_cmpeq_epi32(vH, vF);
                    cond = _mm_andnot_si128(case1,case2);
                    vT = _mm_load_si128(pvHT + i);
                    vT = _mm_blendv_epi8(vT, vTDel, cond);
                    _mm_store_si128(pvHT + i, vT);
                    arr_store(result->trace_table, vT, i, segLen, j);
                }
                vMaxH = _mm_max_epi32(vH, vMaxH);
                /* Update vF value. */
                {
                    __m128i cond = _mm_cmpgt_epi32(vEF_opn, vFa_ext);
                    __m128i vT = _mm_blendv_epi8(vTDel, vTDiag, cond);
                    arr_store(result->trace_del_table, vT, i, segLen, j);
                }
                vEF_opn = _mm_sub_epi32(vH, vGapO);
                vF_ext = _mm_sub_epi32(vF, vGapE);
                {
                    __m128i vT;
                    __m128i cond;
                    __m128i vEa = _mm_load_si128(pvEaLoad + i);
                    __m128i vEa_ext = _mm_sub_epi32(vEa, vGapE);
                    vEa = _mm_max_epi32(vEF_opn, vEa_ext);
                    _mm_store_si128(pvEaStore + i, vEa);
                    cond = _mm_cmpgt_epi32(vEF_opn, vEa_ext);
                    vT = _mm_blendv_epi8(vTIns, vTDiag, cond);
                    if (j+1<s2Len) {
                        arr_store(result->trace_ins_table, vT, i, segLen, j+1);
                    }
                }
                if (! _mm_movemask_epi8(
                            _mm_or_si128(
                                _mm_cmpgt_epi32(vF_ext, vEF_opn),
                                _mm_cmpeq_epi32(vF_ext, vEF_opn))))
                    goto end;
                /*vF = _mm_max_epi32(vEF_opn, vF_ext);*/
                vF = vF_ext;
                vFa_ext = _mm_sub_epi32(vFa, vGapE);
                vFa = _mm_max_epi32(vEF_opn, vFa_ext);
                vHp = _mm_load_si128(pvHLoad + i);
            }
        }
end:
        {
        }

        {
            __m128i vCompare = _mm_cmpgt_epi32(vMaxH, vMaxHUnit);
            if (_mm_movemask_epi8(vCompare)) {
                score = _mm_hmax_epi32_rpl(vMaxH);
                /* if score has potential to overflow, abort early */
                if (score > maxp) {
                    result->saturated = 1;
                    break;
                }
                vMaxHUnit = _mm_set1_epi32(score);
                end_ref = j;
            }
        }

        /*if (score == stop) break;*/
    }

    if (score == INT32_MAX) {
        result->saturated = 1;
    }

    if (result->saturated) {
        score = 0;
        end_query = 0;
        end_ref = 0;
    }
    else {
        if (end_ref == j-1) {
            /* end_ref was the last store column */
            SWAP(pvHMax,  pvHStore)
        }
        else if (end_ref == j-2) {
            /* end_ref was the last load column */
            SWAP(pvHMax,  pvHLoad)
        }
        /* Trace the alignment ending position on read. */
        {
            int32_t *t = (int32_t*)pvHMax;
            int32_t column_len = segLen * segWidth;
            end_query = s1Len - 1;
            for (i = 0; i<column_len; ++i, ++t) {
                if (*t == score) {
                    int32_t temp = i / segWidth + i % segWidth * segLen;
                    if (temp < end_query) {
                        end_query = temp;
                    }
                }
            }
        }
    }

    result->score = score;
    result->end_query = end_query;
    result->end_ref = end_ref;
    result->flag = PARASAIL_FLAG_SW | PARASAIL_FLAG_STRIPED
        | PARASAIL_FLAG_TRACE
        | PARASAIL_FLAG_BITS_32 | PARASAIL_FLAG_LANES_4;

    parasail_free(pvHMax);
    parasail_free(pvHT);
    parasail_free(pvEaLoad);
    parasail_free(pvEaStore);
    parasail_free(pvE);
    parasail_free(pvHLoad);
    parasail_free(pvHStore);

    return result;
}


