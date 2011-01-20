/*
 *  list-ext.h
 *  
 *
 *  Created by Daniel Jacobi on 20.01.11.
 *  Copyright 2011 TU Darmstadt. All rights reserved.
 *
 */

#ifndef _LIST_EXT_H_
#define _LIST_EXT_H_

#include <list.h>

/*
 * @brief These two are needed for the list to work with my setup.
 */ 
#define MYLIST(name) \
name = (list_t)&LIST_CONCAT(name,_list)


/*
 * @brief Return item at position index.
 */
extern void * list_index(list_t list, uint8_t index);


#endif
