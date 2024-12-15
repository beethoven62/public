/**
 * @addtogroup Application Application
 * @{
 * @file      dmm.c
 * @brief     Dynamic memory manager
 * @details   Dynamic memory manager best fit algorithm.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup dmm Dynamic memory manager
 * @brief     Dynamic memory manager module
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/

#include "dmm_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

dmm_interface_t dmm =
{
    .Init               = &dmm_Init,
    .Alloc              = &dmm_Alloc,
    .Free               = &dmm_Free,
    .Report             = &dmm_Report,
    .GetMaxSize         = &dmm_GetMaxSize,
};

dmm_obj_t dmm_obj =
{
    .is_init            = false,
    .dynamic_mem_start  = NULL,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */
error_code_module_t dmm_Init( dmm_handle_t handle, void* heap, size_t size )
{
    error_code_module_t error = NO_ERROR;
    dynamic_mem_node_t *dynamic_mem_start;
    if ( size > sizeof( dynamic_mem_node_t ) )
    {
        dynamic_mem_start = ( dynamic_mem_node_t * ) heap;
        dynamic_mem_start->size = size - sizeof( dynamic_mem_node_t );
        dynamic_mem_start->used = false;
        dynamic_mem_start->next = NULL;
        dynamic_mem_start->prev = NULL;
        dmm_obj.dynamic_mem_start[ handle ] = dynamic_mem_start;
        dmm_obj.total_size[ handle ] = size;
        dmm_obj.max_size[ handle ] = 0;
        dmm_obj.mutex_handle[ handle ] = os.CreateMutex();

#if SYSVIEW_ENABLED
        SEGGER_SYSVIEW_Start();
        SEGGER_SYSVIEW_HeapDefine( dynamic_mem_start, heap, size, sizeof( dynamic_mem_node_t ) );
#endif
    }
    else
    {
        error = ERROR_DMM_NULL_POINTER;
    }

    return error;
}

void* dmm_Alloc( dmm_handle_t handle, size_t size )
{
    void *result = NULL;

    if ( os.TakeSemaphore( dmm_obj.mutex_handle[ handle ], QUEUE_WAIT_TIME ) )
    {
        size_t aligned_size = align_size( size );
        dynamic_mem_node_t *best_mem_block =
            ( dynamic_mem_node_t * ) find_best_mem_block( dmm_obj.dynamic_mem_start[ handle ], aligned_size );

        // check if we actually found a matching (free, large enough) block
        if ( best_mem_block != NULL ) {
            // subtract newly allocated memory (incl. size of the mem node) from selected block
            best_mem_block->size = best_mem_block->size - aligned_size - sizeof( dynamic_mem_node_t );

            // create new mem node after selected node, effectively splitting the memory region
            dynamic_mem_node_t *mem_node_allocate = (dynamic_mem_node_t *) ( ( ( uint8_t * ) best_mem_block ) +
                                                    sizeof( dynamic_mem_node_t ) +
                                                    best_mem_block->size );
            mem_node_allocate->size = aligned_size;
            mem_node_allocate->used = true;
            mem_node_allocate->next = best_mem_block->next;
            mem_node_allocate->prev = best_mem_block;

            // reconnect the doubly linked list
            if ( best_mem_block->next != NULL)
            {
                best_mem_block->next->prev = mem_node_allocate;
            }
            best_mem_block->next = mem_node_allocate;

            // Calculate and record maximum allocated memory
            calculate_max_size( handle );

            // return pointer to newly allocated memory (right after the new list node)
            result = ( void * )( ( uint8_t * ) mem_node_allocate + sizeof( dynamic_mem_node_t ) );

#if SYSVIEW_ENABLED
            SEGGER_SYSVIEW_HeapAllocEx( dmm_obj.dynamic_mem_start[ handle ], result, aligned_size, handle );
#endif
        }
        os.GiveSemaphore( dmm_obj.mutex_handle[ handle ] );
    }

    return result;
}

void dmm_Free( dmm_handle_t handle, void* ptr )
{
    // move along, nothing to free here
    if ( ptr == NULL )
    {
        return;
    }

    // get mem node associated with pointer
    dynamic_mem_node_t *current_mem_node = ( dynamic_mem_node_t * )( ( uint8_t * ) ptr - sizeof( dynamic_mem_node_t ) );

    // pointer we're trying to free was not dynamically allocated it seems
    if ( current_mem_node == NULL )
    {
        return;
    }

#if SYSVIEW_ENABLED
    SEGGER_SYSVIEW_HeapFree( dmm_obj.dynamic_mem_start[ handle ], ptr );
#endif

    // mark block as unused
    current_mem_node->used = false;

    // merge unused blocks
    if ( os.TakeSemaphore( dmm_obj.mutex_handle[ handle ], QUEUE_WAIT_TIME ) )
    {
        current_mem_node = merge_next_node_into_current( current_mem_node );
        merge_current_node_into_previous( current_mem_node );
        os.GiveSemaphore( dmm_obj.mutex_handle[ handle ] );
    }
}

/*Only for debugging purposes; can be turned off through -NDEBUG flag*/
void dmm_Report( dmm_handle_t handle )
{
    uint32_t percent_size;
    dynamic_mem_node_t *dynamic_mem_start = dmm_obj.dynamic_mem_start[ handle ];

    Log.Print( "Heap: %d\r\n", handle );
    while ( dynamic_mem_start != NULL )
    {
        Log.Print( "Block Size:%6d, Head:%12p, Prev:%12p, Next:%12p, Free: %s\r\n",
                   dynamic_mem_start->size,
                   dynamic_mem_start,
                   dynamic_mem_start->prev,
                   dynamic_mem_start->next,
                   dynamic_mem_start->used ? "No" : "Yes" );
        dynamic_mem_start = dynamic_mem_start->next;
    }

    if ( dmm_obj.total_size[ handle ] != 0 )
    {
        percent_size = ( dmm_obj.max_size[ handle ] * 1000 / dmm_obj.total_size[ handle ] + 5 ) / 10;
    }
    else
    {
        percent_size = 0;
    }
    Log.Print( "Maximum memory used: %d, %d%% usage\r\n",
               dmm_obj.max_size[ handle ],
               percent_size );
}

void *find_best_mem_block( dynamic_mem_node_t *dynamic_mem, size_t size )
{
    // initialize the result pointer with NULL and an invalid block size
    dynamic_mem_node_t *best_mem_block = ( dynamic_mem_node_t * ) NULL;
    uint32_t best_mem_block_size = UINT32_MAX;

    // start looking for the best (smallest unused) block at the beginning
    dynamic_mem_node_t *current_mem_block = dynamic_mem;
    while ( current_mem_block )
    {
        // check if block can be used and is smaller than current best
        if ( ( !current_mem_block->used ) &&
                ( current_mem_block->size >= ( size + sizeof( dynamic_mem_node_t ) ) ) &&
                ( current_mem_block->size <= best_mem_block_size ) )
        {
            // update best block
            best_mem_block = current_mem_block;
            best_mem_block_size = current_mem_block->size;
        }

        // move to next block
        current_mem_block = current_mem_block->next;
    }
    return best_mem_block;
}

void *merge_next_node_into_current( dynamic_mem_node_t *current_mem_node )
{
    dynamic_mem_node_t *next_mem_node = current_mem_node->next;
    if ( next_mem_node != NULL && !next_mem_node->used )
    {
        // add size of next block to current block
        current_mem_node->size += current_mem_node->next->size;
        current_mem_node->size += sizeof( dynamic_mem_node_t );

        // remove next block from list
        current_mem_node->next = current_mem_node->next->next;
        if ( current_mem_node->next != NULL )
        {
            current_mem_node->next->prev = current_mem_node;
        }
    }
    return current_mem_node;
}

void *merge_current_node_into_previous( dynamic_mem_node_t *current_mem_node )
{
    dynamic_mem_node_t *prev_mem_node = current_mem_node->prev;
    if ( prev_mem_node != NULL && !prev_mem_node->used )
    {
        // add size of previous block to current block
        prev_mem_node->size += current_mem_node->size;
        prev_mem_node->size += sizeof( dynamic_mem_node_t );

        // remove current node from list
        prev_mem_node->next = current_mem_node->next;
        if ( current_mem_node->next != NULL )
        {
            current_mem_node->next->prev = prev_mem_node;
        }
    }
    return prev_mem_node;
}

size_t align_size( size_t size )
{
    return size + ( ALIGNMENT - ( size & ( ALIGNMENT - 1 ) ) );
}

void calculate_max_size( dmm_handle_t handle )
{
    uint32_t total_size = 0;
    dynamic_mem_node_t *dynamic_mem_current = dmm_obj.dynamic_mem_start[ handle ];

    while ( dynamic_mem_current != NULL )
    {
        if ( dynamic_mem_current->used )
        {
            total_size += dynamic_mem_current->size;
        }
        dynamic_mem_current = dynamic_mem_current->next;
    }

    if ( total_size > dmm_obj.max_size[ handle ] )
    {
        dmm_obj.max_size[ handle ] = total_size;
    }
}

uint32_t dmm_GetMaxSize( dmm_handle_t handle )
{
    return dmm_obj.max_size[ handle ];
}

void* malloc( size_t size )
{
    return dmm_Alloc( dmm_handle_0, size );
}

void free( void* ptr )
{
    dmm.Free( dmm_handle_0, ptr );
}

/**
 * @} dmm
 */

/**
 * @} Application
 */
