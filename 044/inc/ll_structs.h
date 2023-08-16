/******************************************************************************
 * @file: ll_structs.h
 *
 * Header file for data structures to implement a user defined linked list.
 *
 * @creator: Bujji Setty (selagamsetty@wisc.edu)
 * @modified: 11/21/21 Bujji Setty (selagamsetty@wis.edu)
 *
 *****************************************************************************/

#ifndef __LL_STRUCTS_H__
#define __LL_STRUCTS_H__

// Add whatever struct definitions you may need here

typedef struct linked_node {
    size_t size;
    size_t key;
    char *data;
    struct linked_node *next;
} Node;

typedef struct linked_list {
    // Todo fill this with whatever data you need
    Node *head;
    Node *tail;
    size_t length;
} LL;

#endif //__LL_FUNCTIONS_H__
