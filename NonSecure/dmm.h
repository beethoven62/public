/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      dmm.h
 * @brief     Dynamic memory module (public header).
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup dmm
 * @{
 */
#ifndef __DMM_H__
#define __DMM_H__

/***************************************************************************************************************************
 * Includes
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "eelcodes.h"
#include "log.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */
/**
 * @brief Enumeration of heap handles to independent heaps.
 */
typedef enum
{
    dmm_handle_0,           /*!< system heap accessed via malloc() and free() functions */
    dmm_handle_1,           /*!< used for modem specific transfers */
    dmm_handle_2,           /*!< user defined */
    dmm_handle_3,           /*!< user defined */
} dmm_handle_t;

/**
 * Specifies the public interface functions of the watchdog monitor.
 */
typedef struct
{
    error_code_module_t ( *Init )( dmm_handle_t handle, void* heap, size_t size );
    void* ( *Alloc )( dmm_handle_t handle, size_t numbytes );
    void ( *Free )( dmm_handle_t handle, void* allocptr );
    void ( *Report )( dmm_handle_t handle );
    uint32_t ( *GetMaxSize )( dmm_handle_t handle );
} const dmm_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern dmm_interface_t dmm;

#endif /* __DMM_H__ */

/**
 * @} dmm
 */

/**
 * @} Driver
 */
