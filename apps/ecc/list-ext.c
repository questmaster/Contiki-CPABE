/*
 *  list-ext.c
 *  
 *
 *  Created by Daniel Jacobi on 20.01.11.
 *  Copyright 2011 TU Darmstadt. All rights reserved.
 *
 */

#include <stdlib.h>
#include "list-ext.h"

struct list {
	struct list *next;
};


/*
 * @brief Return item at position index.
 */
void * list_index(list_t list, uint8_t index) {
	struct list *l;
	uint8_t n = 0;
	
	for(l = *list; l != NULL; l = l->next) {
		if (n != index) {
			++n;
		} else {
			break;
		}
		
	}
	
	return l;
} 

