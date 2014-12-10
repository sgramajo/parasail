/**
 * @file
 *
 * @author jeff.daily@pnnl.gov
 *
 * Copyright (c) 2014 Battelle Memorial Institute.
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
#ifndef _PARASAIL_INTERNAL_H_
#define _PARASAIL_INTERNAL_H_

#include "config.h"

#include "parasail.h"

void* parasail_memalign(size_t alignment, size_t size);
int * restrict parasail_memalign_int(size_t alignment, size_t size);
parasail_result_t* parasail_result_new();
parasail_result_t* parasail_result_new_table1(const int a, const int b);
parasail_result_t* parasail_result_new_table4(const int a, const int b);

#endif /* _PARASAIL_INTERNAL_H_ */