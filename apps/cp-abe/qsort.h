/*
 *  qsort.h
 *  
 *
 *  Created by Daniel Jacobi on 30.11.10.
 *  Copyright 2010 TU Darmstadt. All rights reserved.
 *
 */

#ifndef _QSORT_H_
#define _QSORT_H_

typedef int (*comparison_fn_t) (const void *, const void *);

extern void qsort(void *const pbase, size_t total_elems,
				  size_t size, comparison_fn_t cmp);


#endif
