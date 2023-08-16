/******************************************************************************
 * @file: ll_functions.c
 *
 * zznie
 * Zinnia Nie
 * 908 319 4044
 * N/A
 *
 * @creator: Zinnia (zznie@wisc.edu)
 * @modified: 7/10/2022
 *****************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll_structs.h"

#include "ll_functions.h"

// Set this to 1 to enable dbgprintf statements, make sure to set it back to 0 
// before submitting!
#define DEBUG               0 
#define dbgprintf(...)      if (DEBUG) { printf(__VA_ARGS__); }

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// ADD ANY HELPER FUNCTIONS YOU MIGHT WRITE HERE 

    /* 
     * Helper function to allocate memory for one new node in the linked list and
     * set the fields in the Node to the given values.
     *
     * @return A valid pointer to a Node on success, NULL on failure
     */
    Node *node_init(size_t key, char *value, size_t n) {
        Node *to_add = (Node *) malloc(sizeof(Node));
        // null means no space left to allocate
        if (!to_add) {
            return NULL;
        }

        to_add->data = (char *) malloc(n); // pointer to a space for all of value
        if (!(to_add->data)) { // check if there is enough space
            free(to_add); // free the allocated node space
            return NULL;
        }

        // initalize Node
        to_add->size = n;
        to_add->key = key;
        memcpy(to_add->data, value, n);

        return to_add;
    }

        /* 
     * Helper function that will loop through a linked list until it either reaches
     * an existing value, or points to the value right before the spot of the value to
     * find. Assumes the linked list is sorted by key. Used for inserting in the middle
     * of a linked list.
     *
     * @param L A pointer to a Linked List struct
     * @param key The key identifier for the data being inserted
     * @param ret_prev An int acting as a boolean for whether to return the node, or the 
     *                 previous node when the key is found so this function can be used 
     *                 for remove and insert
     * @return A valid pointer to a Node
     */
    Node *loop_to(LL *L, size_t key, int ret_prev) {
        if (!L || !(L->head)) { // LL doesn't exist or is empty
            return NULL;
        }
        int increment = 10;
        if (L->length < increment) {
            increment = L->length;
        }
        Node *prev = L->head;
        Node *next_node = L->head;

        if (L->length > increment) {
            while (next_node->next->key > prev->key) {
                // skip forward
                next_node = next_node->next->next->next->next->next->next->next->next->next;
                // if passed the key to find then move on
                if (next_node->key >= key) {
                    break;
                // if passed the beginning of the array then move on
                } else if (prev->key > next_node->key) {
                    break;
                }
                prev = next_node;
            }
        }

        next_node = prev->next;
        for (int i=0; i<increment; i++) {
            if (next_node->key == key) { // return an equal key
                if (ret_prev) {
                    return &(*prev);
                }
                return &(*next_node);
            } else if (next_node->key > key) { // key is greater than wanted key, return prev
                 return &(*prev);
            }
            // increment both pointers
            prev = next_node;
            next_node = next_node->next;    
        }

        return &(*prev); 
    }

    /* 
     * Helper function that will push a node at the head of a circular linked list and
     * adjust the pointers in the linked list structure accordingly.
     *
     * @param L A pointer to a Linked List struct
     * @param node A pointer to a Node struct to be pushed at the head
     * @return 1 on success, 0 on failure
     */
    int push_head(LL *L, Node *node) {
        if (!L || !node) {
            return 0;
        }
        node->next = L->head;
        L->head = node;
        L->tail->next = L->head; // continue circular array
        L->length++;
        return 1;
    }

    /* 
     * Helper function that will insert a node at the tail of a circular linked list and
     * adjust the pointers in the linked list structure accordingly.
     *
     * @param L A pointer to a Linked List struct
     * @param node A pointer to a Node struct to be inserted at the tail
     * @return 1 on success, 0 on failure
     */
    int insert_tail(LL *L, Node *node) {
        if (!L || !node) {
            return 0;
        }
        L->tail->next = node;
        node->next = L->head;
        L->tail = node;
        L->length++;
        return 1;
    }

/******************************************************************************
 * Verificaiton functions
 *****************************************************************************/

/* 
 * This function allocates all the memory needed for created a new linked list,
 * and initializes any necessary fields of the LL struct. The implementation 
 * of this function directly depends on how you design your LL struct. 
 *
 * @return A valid pointer on success, NULL on failure
 */
LL *LL_Init() { 
    LL *list = (LL *) malloc(sizeof(LL));
    if (!list) {
        return NULL;
    }
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;    
}

/* 
 * Insert the (key, value) pair into the linkeded list. If a node with a key 
 * that matches the argument key, the current value stored with that key should
 * be overwrriten by the data in the value argument. Note, you will likely 
 * need to free the old memory or reallocate the amount of memory. Hint: You 
 * will likely also need to store the value of n for later use. 
 *
 * Note: Overwriting the value for an existing value is considered a success. 
 *
 * @param L A pointer to a Linked List struct (may be NULL)
 * @param key The key identifier for the data being inserted
 * @param payload A pointer to the data to write into the node
 * @param n The number of bytes that param value is valid for 
 * @return 1 on success, 0 on failure
 */
int LL_Insert(LL *L, size_t key, char *value, size_t n) {
    if (!L || !value) {
        return 0;
    }

    // first case: empty list
    if (!(L->head)) {
        Node *to_add = node_init(key, value, n);
        if (!to_add) {
            return 0;
        }
        L->head = to_add;
        L->tail = to_add;
        L->length++;
        to_add->next = L->head; // set up circular array
        return 1;
    } 

    // second case: push at head of list or insert at the end
    if (key < L->head->key) {
        Node *to_add = node_init(key, value, n);
        return push_head(L, to_add);
    } else if (key > L->tail->key) {
        Node *to_add = node_init(key, value, n);
        return insert_tail(L, to_add);
    }

    Node *found = loop_to(L, key, 0); // want the current node if the key is found
    if (found) {
        // third case: equal key exists
        if (found->key == key) {
            found->data = (char *) realloc(found->data, n);
            if (!(found->data)) { // check if there is enough space
                return 0;
            }
            memcpy(found->data, value, n);
            found->size = n;
            return 1;
        }

        // fourth case: Node found is previous node
        Node *to_add = node_init(key, value, n);
        if (!to_add) {
            return 0; 
        }
        to_add->next = found->next;
        found->next = to_add;
        L->length++;
        return 1;
    }

    return 0;
}

/* 
 * Getter function to return the size of the linked list.
 *
 * @return The number of nodes in the linked list struct.
 */
size_t LL_Size(LL *L) {
    if (!L) {
        return 0;
    }
    return L->length;
}

/* 
 * Function should fill in the to_fill argument with the bytes of the value
 * found at node whose key is equal to the key argument. If no node in the 
 * linked list has a key value equal to the key argument, no bytes should be 
 * written to the to_fill argument, and you should return 0.
 *
 * This function should detect if the value in the linked list is too large to
 * fit into the to_fill argument. In such a case, no bytes should be written,
 * and this function should return 0.
 *
 * @param L A pointer to a Linked List struct (may be NULL)
 * @param key The search key
 * @param to_fill A pointer to an externally allocated array to be filled in
 * @param n A pointer initally set to the size of to_fill and to be set to 
 *          the actual number of bytes written upon success
 * @return 1 on success, 0 on failure
 */
int LL_Get(LL *L, size_t key, char *to_fill, size_t *n) {
    if (!L || !to_fill || !n || !(L->length)) {
        return 0;
    }

    if (L->head->key == key) {
        if (L->head->size > *n) {
            return 0;
        }
        memcpy(to_fill, L->head->data, L->head->size);
        *n = L->head->size;
        return 1;
    } else if (L->tail->key == key) {
        if (L->tail->size > *n) {
            return 0;
        }
        memcpy(to_fill, L->tail->data, L->tail->size);
        *n = L->tail->size;
        return 1;
    }

    // find the node
    Node *found = loop_to(L, key, 0);
    if (found) {
        if (found->size > *n) {
            return 0;
        }
        if (found->key == key) {
            memcpy(to_fill, found->data, found->size);
            *n = found->size;
            return 1;
        }
    }

    return 0;
}

/* 
 * Function should remove a node from the liked list if the key exists in such 
 * a node. When the node is removed, all memory allocated for that node should
 * be freed. 
 *
 * @param L A pointer to a Linked List struct (may be NULL)
 * @param key The search key
 * @return 1 on successful removal, 0 on failure
 */
int LL_Remove(LL *L, size_t key) {
    if (!L || !(L->head)) {
        return 0;
    }

    // Case 0: one element in list
    if (L->length == 1) {
        free(L->head->data);
        free(L->head);
        L->head = NULL;
        L->tail = NULL;
        L->length--;
        return 1;
    }

    // Case 1: remove head and adjust pointers
    if (L->head->key == key) {
        Node *actual = L->head;
        L->head = L->head->next; // move head pointer to next node
        L->tail->next = L->head; // tail points to new head
        free(actual->data);
        free(actual);
        L->length--;
        return 1; 
    }

    Node *found = loop_to(L, key, 1); // want the previous node
    if (found) {
        Node *actual = found->next;
        if (actual->key == key) { // found the node

            // Case 2: remove tail and adjust pointers
            if (actual->key == L->tail->key) {
                L->tail = found; // set tail pointer to the previous node
            }

            // Case 3: remove node in middle of list
            found->next = actual->next;
            free(actual->data);
            free(actual);
            L->length--;
            return 1;
        }
    }

    // otherwise did not find key in the LL and return failure
    return 0;
}

/* 
 * Function to completely free a linked list. 
 *
 * @param L A pointer to the linked list to be freed (may be NULL)
 */
void LL_Free(LL *L) {
    if (!L) {
        return;
    }
    // if LL is not empty
    if (L->head) {
        Node *next_free;
        if (L->length > 1) {
            for (int i=0; i<L->length-1; i++) { // length-1 prevents pointer being NULL
                next_free = L->head->next;
                free(L->head->data);
                free(L->head);
                L->head = next_free;
            }
        }
        free(L->head->data);
        free(L->head);
    }
    free(L);
    return;
}
