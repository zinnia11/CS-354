/******************************************************************************
 * @file: mem.c
 *
 * zznie
 * Zinnia Nie
 * 908 319 4044
 * N/A
 *
 * @creator: Zinnia (zznie@wisc.edu)
 * @modified: 8/5/2022
 *****************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "header.h"
#include "mem.h"

// Set this to 1 to enable dbgprintf  and dbgassert statements, make sure to 
// set it back to 0 before submitting!
#define DEBUG               0 
#define dbgprintf(...)      if (DEBUG) { printf(__VA_ARGS__); }
#define dbgassert(...)      if (DEBUG) { assert(__VA_ARGS__); }

/******************************************************************************
 * Global Variables
 *****************************************************************************/
void *heap_start;
void *heap_end; // points to the last footer of the heap
size_t heap_size;

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// ADD ANY HELPER FUNCTIONS YOU MIGHT WRITE HERE 

    /*
     * Loop from the top of the heap to find an free chunk that is large enough.
     * Used in Mem_Alloc
     * 
     * @param start A pointer to the starting position to loop from. This pointer 
     *              should be pointing to a header.
     * @param size Payload of the block.
     * @param fill_prev A void pointer to a pointer that is filled with the previous
     *                  block of free memory for maintaining an explicit list
     * @return pointer to a block of memory on success, NULL on failure
     */
    void *loop_top(void *start, unsigned size, void **fill_prev) {
        if (!start) {
            return NULL;
        }

        // find first free block
        while ((*((Header *) start) % 2)) { // if last bit is 1 (allocated)
            // move pointer ahead (allocated)
            start += (*((Header *) start) - 1); 
            // occurs if heap is full
            if (start > heap_end) {
                return NULL;
            }
        }

        // find prev free block 
        void *first = heap_end;
        *(fill_prev) = start;
        while (first > start + *((Header *) start)) {
            if (!(*((Footer *) first) % 2)) {
                *(fill_prev) = first - (*((Footer *) first) - sizeof(Header));
                break;
            }
            first -= (*((Footer *) first) - 1); 
        }

        first = start;
        // find a big enough size block
        do {
            // add header/footer size to the wanted payload
            if (*((Header *) start) >= size) {
                // return pointer to a header
                return start;
            } 
            // move the pointer ahead (free)
            start = *((void **) (start + sizeof(Header))); 
            *(fill_prev) = first;
        } while (start != first && start < heap_end && start);

        return NULL;
    }

    /*
     * Loop to find the correct position to place a newly freed block in address order. 
     * Used in Mem_Free.
     * 
     * @param free A pointer to the header of the block to be freed (it is still allocated)
     * @return pointer to a previous block of memory on success, NULL on failure
     */
    void *loop_bottom(void *free) {
        void *move = free;
        move -= sizeof(Header); // move to the footer of the prev block
        // if prev is free
        if (move > heap_start && !(*((Footer *) move) % 2)) { // move up another block for coalescing
            move -= *((Footer *) move); 
        }
        // create a bottom bound if next block is a free block
        void *bound = free + *((Header *) free); 
        if (bound < heap_end && !(*((Header *) bound) % 2)) { 
            bound += *((Header *) free); 

        }

        // find first free block prev to the given block
        while ((*((Footer *) move) % 2)) { // if last bit is 1 (allocated)
            // move pointer up (allocated)
            move -= (*((Footer *) move) - 1); 
            // out of bounds
            if (move < heap_start) {
                // free blocks might be behind the newly freed block
                // point to footer of last block
                move = heap_end;
                while ((*((Header *) move) % 2)) { 
                    // move pointer back (allocated)
                    move -= (*((Header *) move) - 1); 
                    // when it passes bound, no other free blocks in heap
                    if (move <= bound) {
                        return free;
                    }
                }
                break; // only want this to happen once
            }
        }

        // move to header
        move -= (*((Footer *) move) - sizeof(Footer));
        return move;
    }

/******************************************************************************
 * Memory Allocator functions
 *****************************************************************************/

/*
 * This function initializes the heap space for future calls to Mem_Alloc and 
 * Mem_Free. You may safely assume that this function is only called once in 
 * a given program.
 *
 * @param Heap_Start : A pointer to the start of the heap space
 * @param Heap_Size : The size the heap
 * @return void
 */
void Mem_Init(void *Heap_Start, size_t Heap_Size) {
    if (!Heap_Start) {
        return;
    }

    // Register the start of the heap to the global variable
    heap_start = Heap_Start;

    // Register the size of the heap to the global variable
    heap_size = Heap_Size;

    /* BEGIN MODIFYING CODE HERE */

    // (Extra Credit) Set up prologue blocks
    void *extras = Heap_Start - MIN_BLOCK_SIZE;
    *((Header *) extras) = MIN_BLOCK_SIZE + 1;
    extras = Heap_Start - sizeof(Footer);
    *((Footer *) extras) = MIN_BLOCK_SIZE + 1;

    // (Extra Credit) Set up epilogue block
    extras = Heap_Start + heap_size;
    *((Footer *) extras) = 0x1;

    // (Mandatory) Set up heap space
    void *ptr = heap_start + (16 - ((unsigned long) heap_start % 16)); // add for alignment
    ptr = ptr - sizeof(Header); // find header
    if (ptr < Heap_Start) { // maintain heap_start to point to a header
        heap_start = ptr;
    }

    // store the value: change the alignment and then add the Header size
    Header val = heap_size - (16 - ((unsigned long) heap_start % 16)) + sizeof(Header);
    *((Header *) ptr) = val;
    if (heap_size % 2) { // if odd, ensure LSB is 0
        val = heap_size - 1;
        *((Header *) ptr) = val;
    }

    // start the explicit list
    ptr += sizeof(Header);
    *((void **) ptr) = heap_start;

    // footer
    ptr = heap_start + heap_size; // move pointer to the very end of the heap
    ptr = ptr - sizeof(Footer);
    heap_end = ptr;

    *((Footer *) ptr) = val;

    /* Your first task is initialize the header for the heap. The heap may 
     * contain garbage values, your job is to mark the first header to denote
     * that heap can be treated as one large free block. To do so, you can use
     * the following steps:
     *
     * 1) Find the pointer that will be returned on the first call to malloc.
     *    Remember, this pointer must be aligned to a 16 byte boundary.
     * 2) Subtract sizeof(Header) bytes to get a pointer to the first header
     * 3) Set this header to the correct value. Ensure the LSB is 0 to mark
     *    the block as free.
     * 4) (Optional) if you are using footers, ensure you set the last 
     *    sizeof(Header) bytes to the same value as the header.
     */
   
    /* END MODIFYING CODE HERE */
    return;
}

/*
 * This function  allows a user to request space on the heap. The type of param
 * payload is defined in mem.h and may not be changed. If param payload is ever
 * 0, this function should return NULL immediately. 
 *
 * @param payload : The number of bytes the user wants on the heap
 * @return A 16-byte aligned pointer to payload bytes on the heap on success,
 *         NULL on failure
 */
void* Mem_Alloc(Payload payload){
    /* BEGIN MODIFYING CODE HERE */
    // First, verify non-zero payload size, payload is not greater than heap size
    if (!payload || payload > heap_size - sizeof(Header) - sizeof(Footer)) {
        return NULL;
    }

    // Determine what size block is needed to satisfy this payload request.
    // Remember, user pointers must be 16-byte aligned, and the minimum block
    // size is 16 bytes. 
    unsigned block = payload + (sizeof(Header) + sizeof(Footer)); // add space needed for header/footer
    // change payload to multiple of 16
    if ((block % 16)) {
        block = block + (16 - (block % 16)); 
    } 
    // Search the heap space for a free block that can satisfy the request.
    // Search should be done in address order.
        // if found, determine in the block can be split
            // if split is possible, split block and set headers
            // else allocate directly into block
        // else return NULL 
    void *loc_prev = NULL; // to find the explicit list
    void *returner = loop_top(heap_start, block, &(loc_prev)); // should return a pointer to a header
    if (!returner || *((Header *) returner) < block || *((Header *) returner) % 2) {
        return NULL;
    }

    void *place = returner;
    unsigned leftover = *((Header *) returner) - block; // leftover space on the block
    if (leftover >= 16) { // if another min 16 block can be created
        *((Header *) returner) = block + 1; // make allocated
        place += (block - sizeof(Footer)); // allocate the footer
        *((Footer *) place) = block + 1;

        // find the split
        place += sizeof(Footer);
        *((Header *) place) = leftover;

        // fix explicit list
        if (loc_prev != returner) {
            place += sizeof(Header);
            *((void **) place) = *((void**) (returner + sizeof(Header))); 
        } else {
            place += sizeof(Header);
            *((void **) place) = place - sizeof(Header);
        }
        loc_prev += sizeof(Header);
        *((void **) loc_prev) = place - sizeof(Header); 

        // allocate the footer
        place -= sizeof(Header);
        place += (leftover - sizeof(Footer)); // allocate the footer
        *((Footer *) place) = leftover;
    } else {
        *((Header *) returner) = block + 1; // make allocated
        place += (block - sizeof(Footer)); // allocate the footer
        *((Footer *) place) = block + 1;

        // fix explicit list 
        loc_prev += sizeof(Header);
        *((void **) loc_prev) =  *((void **) (returner + sizeof(Header))); 
    }

    returner += sizeof(Header);
    return returner;
}


/*
 * This function  allows a user to tell the memory allocator that they finished
 * using space that they had requested on the heap.
 *
 * @param ptr: A pointer
 * @return 0 on error, 1 on success 
 */
int Mem_Free(void *ptr) {
    // If ptr is null, return 0 immediately
    if (!ptr) {
        return 0;
    }
    // If ptr is not in heap, return 0 immediately
    if ((unsigned long) ptr > (unsigned long) heap_end || 
        (unsigned long) ptr < (unsigned long) heap_start) {
        return 0;
    }
    // If ptr is not 16 byte aligned
    if (((unsigned long) ptr % 16)) {
        return 0;
    }
    // If ptr is not after a Header, return 0 immediately
    // First, search through the allocated blocks to see if these ptr is indeed
    // one that was returned by a call to Mem_Alloc
    void *move = heap_start;
    ptr -= sizeof(Header); // move up to the header
    // while the pointer is less than the address of the end of the heap
    while ((unsigned long) move < (unsigned long) heap_end && 
        (unsigned long) move != (unsigned long) ptr) {
        // move the pointer down a block
        if (*((Header *) move) % 2) {
            move += *((Header *) move) - 1;
        } else {
            move += *((Header *) move);
        }
    }
    // ptr is not a block allocated
    if ((unsigned long) move != (unsigned long) ptr) {
        return 0;
    }
    // If the header before ptr is not allocated, return 0 immediately 
    if (!(*((Header *) ptr) % 2)) {
        return 0;
    }

    // before freeing, find location to place freed block (prev block header)
    void *loc_prev = loop_bottom(ptr);

    // Free the block
    *((Header *) ptr) = *((Header *) ptr) - 1; 
    // move to the footer
    ptr += *((Header *) ptr) - sizeof(Footer);
    *((Footer *) ptr) = *((Footer *) ptr) - 1;
    // ptr points to the footer of the just freed block
    ptr -= (*((Footer *) ptr) - sizeof(Footer)); // move ptr to header


    short coalesce_up = 0;
    short coalesce_down = 0;
    // Coalesce adjacent free blocks 
    void *prev = ptr - sizeof(Footer); // footer of the prev block
    void *next = ptr + *((Header *) ptr); // header of the next block
    // prev and next have to be in the heap
    if (prev >= heap_start) {
        if (!(*((Footer *) prev) % 2)) { // prev is free
            coalesce_up = 1;
            // move to header of prev block
            prev -= (*((Footer *) prev) - sizeof(Footer));
            *((Header *) prev) += *((Header *) ptr); // add the size of the newly freed block

            ptr += (*((Header *) ptr) - sizeof(Footer)); // move to footer
            *((Footer *) ptr) = *((Header *) prev);
            ptr -= (*((Footer *) ptr) - sizeof(Footer)); // move ptr to header

            // explicit list: if coalesced with the prev block, then no need to change any
            // pointers, the prev's next and the next are all still correct
        }
    }
    if (next <= heap_end) {
        if (!(*((Header *) next) % 2)) { // next is free
            coalesce_down = 1;
            void *old_next = next;

            *((Header *) ptr) += *((Header *) next); // add the size of the newly freed block
            next += *((Header *) next) - sizeof(Footer); // move next to the footer
            *((Footer *) next) = *((Header *) ptr);

            // explicit list: if coalesced with the next block, move the next block's next
            // pointer to right after the header
            ptr += sizeof(Header);
            *((void **) ptr) = *((void **) (old_next + sizeof(Header))); 
            if (!coalesce_up) { // if coalesced up, then this step is done
                loc_prev += sizeof(Header);
                *((void **) loc_prev) = ptr - sizeof(Header);
            }
        }
    }

    // if did not coalesce, connect blocks in explicit list
    if (!coalesce_up && !coalesce_down) {
        ptr += sizeof(Header);
        // ptr has a pointer to the header of next free block
        *((void **) ptr) = *((void **) (loc_prev + sizeof(Header)));
        loc_prev += sizeof(Header);
        *((void **) loc_prev) = ptr - sizeof(Header); // prev block contains ptr
    }

    return 1;
}

