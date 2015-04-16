#!/usr/bin/python


sse2 = {
    "ISA"         : "sse",
    "ISA_VERSION" : "2",
    "HEADER"      : "#include <emmintrin.h>",
    "BITS"        : 128,
    "VTYPE"       : "__m128i",
    "VADDx8"      : "_mm_add_epi8",
    "VADDx16"     : "_mm_add_epi16",
    "VADDx32"     : "_mm_add_epi32",
    "VADDx64"     : "_mm_add_epi64",
    "VADDSx8"     : "_mm_adds_epi8",
    "VADDSx16"    : "_mm_adds_epi16",
    "VBLEND"      : "_mm_blendv_epi8_rpl",
    "VCMPEQx8"    : "_mm_cmpeq_epi8",
    "VCMPEQx16"   : "_mm_cmpeq_epi16",
    "VCMPEQx32"   : "_mm_cmpeq_epi32",
    "VCMPEQx64"   : "_mm_cmpeq_epi64_rpl",
    "VCMPGTx8"    : "_mm_cmpgt_epi8",
    "VCMPGTx16"   : "_mm_cmpgt_epi16",
    "VCMPGTx32"   : "_mm_cmpgt_epi32",
    "VCMPGTx64"   : "_mm_cmpgt_epi64_rpl",
    "VCMPLTx8"    : "_mm_cmplt_epi8",
    "VCMPLTx16"   : "_mm_cmplt_epi16",
    "VCMPLTx32"   : "_mm_cmplt_epi32",
    "VCMPLTx64"   : "_mm_cmplt_epi64_rpl",
    "VEXTRACTx8"  : "_mm_extract_epi8_rpl",
    "VEXTRACTx16" : "_mm_extract_epi16",
    "VEXTRACTx32" : "_mm_extract_epi32_rpl",
    "VEXTRACTx64" : "_mm_extract_epi64_rpl",
    "VINSERTx8"   : "_mm_insert_epi8_rpl",
    "VINSERTx16"  : "_mm_insert_epi16",
    "VINSERTx32"  : "_mm_insert_epi32_rpl",
    "VINSERTx64"  : "_mm_insert_epi64_rpl",
    "VLOAD"       : "_mm_load_si128",
    "VMAXx8"      : "_mm_max_epi8_rpl",
    "VMAXx16"     : "_mm_max_epi16",
    "VMAXx32"     : "_mm_max_epi32_rpl",
    "VMAXx64"     : "_mm_max_epi64_rpl",
    "VMINx8"      : "_mm_min_epi8_rpl",
    "VMINx16"     : "_mm_min_epi16",
    "VMINx32"     : "_mm_min_epi32_rpl",
    "VMINx64"     : "_mm_min_epi64_rpl",
    "VMOVEMASK"   : "_mm_movemask_epi8",
    "VAND"        : "_mm_and_si128",
    "VANDNOT"     : "_mm_andnot_si128",
    "VOR"         : "_mm_or_si128",
    "VROTATE"     : "_mm_rlli_si128_rpl",
    "VSET0"       : "_mm_setzero_si128",
    "VSET1x8"     : "_mm_set1_epi8",
    "VSET1x16"    : "_mm_set1_epi16",
    "VSET1x32"    : "_mm_set1_epi32",
    "VSET1x64"    : "_mm_set1_epi64x",
    "VSETx8"      : "_mm_set_epi8",
    "VSETx16"     : "_mm_set_epi16",
    "VSETx32"     : "_mm_set_epi32",
    "VSETx64"     : "_mm_set_epi64x",
    "VRSHIFT"     : "_mm_srli_si128",
    "VSHIFT"      : "_mm_slli_si128",
    "VSTORE"      : "_mm_store_si128",
    "VSUBx8"      : "_mm_sub_epi8",
    "VSUBx16"     : "_mm_sub_epi16",
    "VSUBx32"     : "_mm_sub_epi32",
    "VSUBx64"     : "_mm_sub_epi64",
    "VSUBSx8"     : "_mm_subs_epi8",
    "VSUBSx16"    : "_mm_subs_epi16",
    "_mm_blendv_epi8_rpl" : """
static inline __m128i _mm_blendv_epi8_rpl(__m128i a, __m128i b, __m128i mask) {
    a = _mm_andnot_si128(mask, a);
    a = _mm_or_si128(a, _mm_and_si128(mask, b));
    return a;
}
""",
    "_mm_cmpeq_epi64_rpl" : """
static inline __m128i _mm_cmpeq_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]==B.v[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A.v[1] = (A.v[1]==B.v[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A.m;
}
""",
    "_mm_cmpgt_epi64_rpl" : """
static inline __m128i _mm_cmpgt_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]>B.v[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A.v[1] = (A.v[1]>B.v[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A.m;
}
""",
    "_mm_cmplt_epi64_rpl" : """
static inline __m128i _mm_cmplt_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]<B.v[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A.v[1] = (A.v[1]<B.v[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A.m;
}
""",
    "_mm_extract_epi8_rpl" : """
static inline int8_t _mm_extract_epi8_rpl(__m128i a, const int imm) {
    __m128i_8_t A;
    A.m = a;
    return A.v[imm];
}
""",
    "_mm_extract_epi32_rpl" : """
static inline int32_t _mm_extract_epi32_rpl(__m128i a, const int imm) {
    __m128i_32_t A;
    A.m = a;
    return A.v[imm];
}
""",
    "_mm_extract_epi64_rpl" : """
static inline int64_t _mm_extract_epi64_rpl(__m128i a, const int imm) {
    __m128i_64_t A;
    A.m = a;
    return A.v[imm];
}
""",
    "_mm_insert_epi8_rpl" : """
static inline __m128i _mm_insert_epi8_rpl(__m128i a, int8_t i, const int imm) {
    __m128i_8_t A;
    A.m = a;
    A.v[imm] = i;
    return A.m;
}
""",
    "_mm_insert_epi32_rpl" : """
static inline __m128i _mm_insert_epi32_rpl(__m128i a, int32_t i, const int imm) {
    __m128i_32_t A;
    A.m = a;
    A.v[imm] = i;
    return A.m;
}
""",
    "_mm_insert_epi64_rpl" : """
static inline __m128i _mm_insert_epi64_rpl(__m128i a, int64_t i, const int imm) {
    __m128i_64_t A;
    A.m = a;
    A.v[imm] = i;
    return A.m;
}
""",
    "_mm_max_epi8_rpl" : """
static inline __m128i _mm_max_epi8_rpl(__m128i a, __m128i b) {
    __m128i mask = _mm_cmpgt_epi8(a, b);
    a = _mm_and_si128(a, mask);
    b = _mm_andnot_si128(mask, b);
    return _mm_or_si128(a, b);
}
""",
    "_mm_max_epi32_rpl" : """
static inline __m128i _mm_max_epi32_rpl(__m128i a, __m128i b) {
    __m128i mask = _mm_cmpgt_epi32(a, b);
    a = _mm_and_si128(a, mask);
    b = _mm_andnot_si128(mask, b);
    return _mm_or_si128(a, b);
}
""",
    "_mm_max_epi64_rpl" : """
static inline __m128i _mm_max_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]>B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]>B.v[1]) ? A.v[1] : B.v[1];
    return A.m;
}
""",
    "_mm_min_epi8_rpl" : """
static inline __m128i _mm_min_epi8_rpl(__m128i a, __m128i b) {
    __m128i mask = _mm_cmpgt_epi8(b, a);
    a = _mm_and_si128(a, mask);
    b = _mm_andnot_si128(mask, b);
    return _mm_or_si128(a, b);
}
""",
    "_mm_min_epi32_rpl" : """
static inline __m128i _mm_min_epi32_rpl(__m128i a, __m128i b) {
    __m128i mask = _mm_cmpgt_epi32(b, a);
    a = _mm_and_si128(a, mask);
    b = _mm_andnot_si128(mask, b);
    return _mm_or_si128(a, b);
}
""",
    "_mm_min_epi64_rpl" : """
static inline __m128i _mm_min_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]<B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]<B.v[1]) ? A.v[1] : B.v[1];
    return A.m;
}
""",
    "_mm_rlli_si128_rpl" : """
#define _mm_rlli_si128_rpl(a,imm) _mm_or_si128(_mm_slli_si128(a,imm),_mm_srli_si128(a,16-imm))
""",
}


sse41 = {
    "ISA"         : "sse",
    "ISA_VERSION" : "41",
    "HEADER"      : "#include <emmintrin.h>\n#include <smmintrin.h>",
    "BITS"        : 128,
    "VTYPE"       : "__m128i",
    "VADDx8"      : "_mm_add_epi8",
    "VADDx16"     : "_mm_add_epi16",
    "VADDx32"     : "_mm_add_epi32",
    "VADDx64"     : "_mm_add_epi64",
    "VADDSx8"     : "_mm_adds_epi8",
    "VADDSx16"    : "_mm_adds_epi16",
    "VBLEND"      : "_mm_blendv_epi8",
    "VCMPEQx8"    : "_mm_cmpeq_epi8",
    "VCMPEQx16"   : "_mm_cmpeq_epi16",
    "VCMPEQx32"   : "_mm_cmpeq_epi32",
    "VCMPEQx64"   : "_mm_cmpeq_epi64",
    "VCMPGTx8"    : "_mm_cmpgt_epi8",
    "VCMPGTx16"   : "_mm_cmpgt_epi16",
    "VCMPGTx32"   : "_mm_cmpgt_epi32",
    "VCMPGTx64"   : "_mm_cmpgt_epi64_rpl",
    "VCMPLTx8"    : "_mm_cmplt_epi8",
    "VCMPLTx16"   : "_mm_cmplt_epi16",
    "VCMPLTx32"   : "_mm_cmplt_epi32",
    "VCMPLTx64"   : "_mm_cmplt_epi64_rpl",
    "VEXTRACTx8"  : "_mm_extract_epi8",
    "VEXTRACTx16" : "_mm_extract_epi16",
    "VEXTRACTx32" : "_mm_extract_epi32",
    "VEXTRACTx64" : "_mm_extract_epi64",
    "VINSERTx8"   : "_mm_insert_epi8",
    "VINSERTx16"  : "_mm_insert_epi16",
    "VINSERTx32"  : "_mm_insert_epi32",
    "VINSERTx64"  : "_mm_insert_epi64",
    "VLOAD"       : "_mm_load_si128",
    "VMAXx8"      : "_mm_max_epi8",
    "VMAXx16"     : "_mm_max_epi16",
    "VMAXx32"     : "_mm_max_epi32",
    "VMAXx64"     : "_mm_max_epi64_rpl",
    "VMINx8"      : "_mm_min_epi8",
    "VMINx16"     : "_mm_min_epi16",
    "VMINx32"     : "_mm_min_epi32",
    "VMINx64"     : "_mm_min_epi64_rpl",
    "VMOVEMASK"   : "_mm_movemask_epi8",
    "VAND"        : "_mm_and_si128",
    "VANDNOT"     : "_mm_andnot_si128",
    "VOR"         : "_mm_or_si128",
    "VROTATE"     : "_mm_rlli_si128_rpl",
    "VSET0"       : "_mm_setzero_si128",
    "VSET1x8"     : "_mm_set1_epi8",
    "VSET1x16"    : "_mm_set1_epi16",
    "VSET1x32"    : "_mm_set1_epi32",
    "VSET1x64"    : "_mm_set1_epi64x",
    "VSETx8"      : "_mm_set_epi8",
    "VSETx16"     : "_mm_set_epi16",
    "VSETx32"     : "_mm_set_epi32",
    "VSETx64"     : "_mm_set_epi64x",
    "VRSHIFT"     : "_mm_srli_si128",
    "VSHIFT"      : "_mm_slli_si128",
    "VSTORE"      : "_mm_store_si128",
    "VSUBx8"      : "_mm_sub_epi8",
    "VSUBx16"     : "_mm_sub_epi16",
    "VSUBx32"     : "_mm_sub_epi32",
    "VSUBx64"     : "_mm_sub_epi64",
    "VSUBSx8"     : "_mm_subs_epi8",
    "VSUBSx16"    : "_mm_subs_epi16",
    "_mm_cmpgt_epi64_rpl" : """
static inline __m128i _mm_cmpgt_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]>B.v[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A.v[1] = (A.v[1]>B.v[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A.m;
}
""",
    "_mm_cmplt_epi64_rpl" : """
static inline __m128i _mm_cmplt_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]<B.v[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A.v[1] = (A.v[1]<B.v[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A.m;
}
""",
    "_mm_max_epi64_rpl" : """
static inline __m128i _mm_max_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]>B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]>B.v[1]) ? A.v[1] : B.v[1];
    return A.m;
}
""",
    "_mm_min_epi64_rpl" : """
static inline __m128i _mm_min_epi64_rpl(__m128i a, __m128i b) {
    __m128i_64_t A;
    __m128i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]<B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]<B.v[1]) ? A.v[1] : B.v[1];
    return A.m;
}
""",
    "_mm_rlli_si128_rpl" : """
#define _mm_rlli_si128_rpl(a,imm) _mm_alignr_epi8(a, a, 16-imm)
""",
}


avx2 = {
    "ISA"         : "avx",
    "ISA_VERSION" : "2",
    "HEADER"      : "#include <immintrin.h>",
    "BITS"        : 256,
    "VTYPE"       : "__m256i",
    "VADDx8"      : "_mm256_add_epi8",
    "VADDx16"     : "_mm256_add_epi16",
    "VADDx32"     : "_mm256_add_epi32",
    "VADDx64"     : "_mm256_add_epi64",
    "VADDSx8"     : "_mm256_adds_epi8",
    "VADDSx16"    : "_mm256_adds_epi16",
    "VBLEND"      : "_mm256_blendv_epi8",
    "VCMPEQx8"    : "_mm256_cmpeq_epi8",
    "VCMPEQx16"   : "_mm256_cmpeq_epi16",
    "VCMPEQx32"   : "_mm256_cmpeq_epi32",
    "VCMPEQx64"   : "_mm256_cmpeq_epi64",
    "VCMPGTx8"    : "_mm256_cmpgt_epi8",
    "VCMPGTx16"   : "_mm256_cmpgt_epi16",
    "VCMPGTx32"   : "_mm256_cmpgt_epi32",
    "VCMPGTx64"   : "_mm256_cmpgt_epi64",
    "VCMPLTx8"    : "_mm256_cmplt_epi8_rpl",
    "VCMPLTx16"   : "_mm256_cmplt_epi16_rpl",
    "VCMPLTx32"   : "_mm256_cmplt_epi32_rpl",
    "VCMPLTx64"   : "_mm256_cmplt_epi64_rpl",
    "VEXTRACTx8"  : "_mm256_extract_epi8",
    "VEXTRACTx16" : "_mm256_extract_epi16",
    "VEXTRACTx32" : "_mm256_extract_epi32",
    "VEXTRACTx64" : "_mm256_extract_epi64",
    "VINSERTx8"   : "_mm256_insert_epi8",
    "VINSERTx16"  : "_mm256_insert_epi16",
    "VINSERTx32"  : "_mm256_insert_epi32",
    "VINSERTx64"  : "_mm256_insert_epi64",
    "VLOAD"       : "_mm256_load_si256",
    "VMAXx8"      : "_mm256_max_epi8",
    "VMAXx16"     : "_mm256_max_epi16",
    "VMAXx32"     : "_mm256_max_epi32",
    "VMAXx64"     : "_mm256_max_epi64_rpl",
    "VMINx8"      : "_mm256_min_epi8",
    "VMINx16"     : "_mm256_min_epi16",
    "VMINx32"     : "_mm256_min_epi32",
    "VMINx64"     : "_mm256_min_epi64_rpl",
    "VMOVEMASK"   : "_mm256_movemask_epi8",
    "VAND"        : "_mm256_and_si256",
    "VANDNOT"     : "_mm256_andnot_si256",
    "VOR"         : "_mm256_or_si256",
    "VROTATE"     : "_mm256_rlli_si256_rpl",
    "VSET0"       : "_mm256_setzero_si256",
    "VSET1x8"     : "_mm256_set1_epi8",
    "VSET1x16"    : "_mm256_set1_epi16",
    "VSET1x32"    : "_mm256_set1_epi32",
    "VSET1x64"    : "_mm256_set1_epi64x",
    "VSETx8"      : "_mm256_set_epi8",
    "VSETx16"     : "_mm256_set_epi16",
    "VSETx32"     : "_mm256_set_epi32",
    "VSETx64"     : "_mm256_set_epi64x",
    "VRSHIFT"     : "_mm256_srli_si256_rpl",
    "VSHIFT"      : "_mm256_slli_si256_rpl",
    "VSTORE"      : "_mm256_store_si256",
    "VSUBx8"      : "_mm256_sub_epi8",
    "VSUBx16"     : "_mm256_sub_epi16",
    "VSUBx32"     : "_mm256_sub_epi32",
    "VSUBx64"     : "_mm256_sub_epi64",
    "VSUBSx8"     : "_mm256_subs_epi8",
    "VSUBSx16"    : "_mm256_subs_epi16",
    "_mm256_cmplt_epi8_rpl" : """
#define _mm256_cmplt_epi8_rpl(a,b) _mm256_cmpgt_epi8(b,a)
""",
    "_mm256_cmplt_epi16_rpl" : """
#define _mm256_cmplt_epi16_rpl(a,b) _mm256_cmpgt_epi16(b,a)
""",
    "_mm256_cmplt_epi32_rpl" : """
#define _mm256_cmplt_epi32_rpl(a,b) _mm256_cmpgt_epi32(b,a)
""",
    "_mm256_cmplt_epi64_rpl" : """
#define _mm256_cmplt_epi64_rpl(a,b) _mm256_cmpgt_epi64(b,a)
""",
    "_mm256_max_epi64_rpl" : """
static inline __m256i _mm256_max_epi64_rpl(__m256i a, __m256i b) {
    __m256i_64_t A;
    __m256i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]>B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]>B.v[1]) ? A.v[1] : B.v[1];
    A.v[2] = (A.v[2]>B.v[2]) ? A.v[2] : B.v[2];
    A.v[3] = (A.v[3]>B.v[3]) ? A.v[3] : B.v[3];
    return A.m;
}
""",
    "_mm256_min_epi64_rpl" : """
static inline __m256i _mm256_min_epi64_rpl(__m256i a, __m256i b) {
    __m256i_64_t A;
    __m256i_64_t B;
    A.m = a;
    B.m = b;
    A.v[0] = (A.v[0]<B.v[0]) ? A.v[0] : B.v[0];
    A.v[1] = (A.v[1]<B.v[1]) ? A.v[1] : B.v[1];
    A.v[2] = (A.v[2]<B.v[2]) ? A.v[2] : B.v[2];
    A.v[3] = (A.v[3]<B.v[3]) ? A.v[3] : B.v[3];
    return A.m;
}
""",
    "_mm256_srli_si256_rpl" : """
#define _mm256_srli_si256_rpl(a,imm) _mm256_or_si256(_mm256_slli_si256(_mm256_permute2x128_si256(a, a, _MM_SHUFFLE(3,0,0,1)), 16-imm), _mm256_srli_si256(a, imm))
""",
    "_mm256_slli_si256_rpl" : """
#define _mm256_slli_si256_rpl(a,imm) _mm256_alignr_epi8(a, _mm256_permute2x128_si256(a, a, _MM_SHUFFLE(0,0,3,0)), 16-imm)
""",
    "_mm256_rlli_si256_rpl" : """
#define _mm256_rlli_si256_rpl(a,imm) _mm256_alignr_epi8(a, _mm256_permute2x128_si256(a, a, _MM_SHUFFLE(0,0,0,1)), 16-imm)
""",
}


isa = {
        "sse2"  : sse2,
        "sse41" : sse41,
        "avx2"  : avx2,
}