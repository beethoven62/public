/** @file dmm_priv.h
 *
 * @brief       Dynamic memory module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup dmm
 * @{
 */

#ifndef __DMM_PRIV_H__
#define __DMM_PRIV_H__

/*
 * @note
 * IMPLEMENTAION AND EXPLANATION:
 *
 * The heap manager was implemented using a linked list of metadata_t structs or blocks of memory. These linked lists are doubly linked and
 * contain a metadata_t* header to indicate the size of the blocks (integer), whether the block is allocated (isFree=1) or free (isFree=0) (integer),
 * and the pointers to the previous and next blocks. Although we added the isFree integer into our header, it does not change the value of METADATA_T_ALIGNED
 * as that is rounded to 32 or 64 depending on the machine (dmm.h). However it does affect our test_basic results since it adds an additional 8 bytes into
 * the first block of the malloc call. This means that test_basic would not run in the default MAX_HEAP_SIZE of 1024 bytes heap but would work properly
 * by increasing the heap to a 1032 or more bytes due to having extra
 *
 * Using a doubly linked list containing all of the blocks (free and not free thus implicit free-list) was beneficial to the coalesce, free and split
 * operations primarily because it allowed for simpler implementation. For instance, the free command was done by changing the header of that block at
 * the particular address so that the header denotes that the block is free. Coalescing between two blocks was done by checking if the previous block and
 * the current block, or the current block and next block were both free and merged the two blocks if such was the case. This is far more easier than making
 * a linked list of only free blocks (explicit free-list) since we can just check the headers of adjacent blocks and coalesce them accordingly if both are free.
 *
 * Since the project was implemented using first fit algorithm, the dmalloc call was done by iterating through the linked lists of blocks until we found
 * the first free block that had more or equal bytes than the desired size of the malloc. If the free block and the desired size of malloc were equal,
 * we changed the header to indicate that the block became allocated or not free. If the free block size was bigger, a split operation is needed.
 * Since we used first fit algorithm, we changed the header of the free block to become allocated. We then created a new header for the remaining
 * space of the block which is casted to be free. The allocated block then points next to this free block of remaining space which now points to the old
 * next call of the now allocated block.
 *
 * One major downside of the doubly linked list is that you have to iterate more than the number of free blocks since we obviously included the allocated
 * blocks into the linked list. Despite the longer runtime in the malloc call, the free and coalesce remains O(1) since the algorithm only deals with
 * that desired block to be freed.  If we created a linked list of only free blocks, then we can shorten the run time of malloc since it would iterate
 * through less blocks however coalesce would become a O(n) algorithm as we would have to iterate through the free block list to determine our coalesced blocks.
 *
 * freelist maintains all the blocks which are not in use; freelist is kept
 * always sorted to improve the efficiency of coalescing
 */

/***************************************************************************************************************************
 * Includes
 */

#include "dmm.h"

/***************************************************************************************************************************
 * Private constants and macros
 */
#define NHEAPS                      ( 4 )
#define ALIGNMENT                   ( 4 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */
typedef struct dynamic_mem_node {
    uint32_t size;
    bool used;
    struct dynamic_mem_node *next;
    struct dynamic_mem_node *prev;
} dynamic_mem_node_t;

typedef struct
{
    bool                        is_init;
    dynamic_mem_node_t          *dynamic_mem_start[ NHEAPS ];
    uint32_t                    total_size[ NHEAPS ];
    uint32_t                    max_size[ NHEAPS ];
    SemaphoreHandle_t           mutex_handle[ NHEAPS ];
} dmm_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
* @brief       Initialize the module.
* @param[in]   handle     handle ID: dmm_handle_x (x = 0, 1, 2, or 3)
* @param[in]   heap            pointer to block of memory (aligned to 8 byte boundary)
* @param[in]   size            size of memory block in bytes
* @return      Error code.
*/
static error_code_module_t dmm_Init( dmm_handle_t handle, void* heap, size_t size );

/**
 * @brief       Allocate memory.
 * @param[in]   handle     handle ID: dmm_handle_x (x = 0, 1, 2, or 3)
 * @param[in]   numbytes        size of memory block to allocate in bytes
 * @return      Pointer to allocated memory block (NULL if error).
 */
static void* dmm_Alloc( dmm_handle_t handle, size_t numbytes );

/**
 * @brief       Free memory.
 * @param[in]   handle     Handle of heap to report about.
 * @param[in]   ptr             Pointer to memory block to be freed.
 */
static void dmm_Free( dmm_handle_t handle, void* ptr );

/**
 * @brief       Print free memory list.
 * @param[in]   handle     Handle of heap to report about.
 */
static void dmm_Report( dmm_handle_t handle );

/**
 * @brief       Print free memory list.
 * @param[in]   handle     Handle of heap to report about.
 * @return      Maximum bytes used since initialized
 */
static uint32_t dmm_GetMaxSize( dmm_handle_t handle );

/***************************************************************************************************************************
 * Private prototypes
 */
void *find_best_mem_block(dynamic_mem_node_t *dynamic_mem, size_t size);
void *merge_next_node_into_current(dynamic_mem_node_t *current_mem_node);
void *merge_current_node_into_previous(dynamic_mem_node_t *current_mem_node);
size_t align_size( size_t size );
void calculate_max_size( dmm_handle_t handle );

#endif /* __DMM_PRIV_H__ */

/**
 * @}
 */
