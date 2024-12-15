/** @file app_priv.h
 *
 * @brief       Application module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup App
 * @{
 */

#ifndef __APP_PRIV_H__
#define __APP_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "app.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init ;
    const char *                hw_version ;
    uint8_t                     hw_version_id ;
    uint16_t                    prev_value_dco;
} app_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @return      Error code.
 */
static error_code_module_t app_Init( void );

/**
 * @brief       Return queue handle of log task.
 * @return      Log queue handle.
 */
static QueueHandle_t app_GetLogHandle( void );

/**
 * @brief       Return queue handle of command line interface task.
 * @return      CLI queue handle.
 */
static QueueHandle_t app_GetCliQHandle( void );

/**
 * @brief       Return queue handle of serial LTE modem task.
 * @return      SLM queue handle.
 */
static QueueHandle_t app_GetSlmQHandle( void );

/**
 * @brief       Return queue handle of file system task.
 * @return      FS queue handle.
 */
static QueueHandle_t app_GetFsQHandle( void );

/**
 * @brief       Return queue handle of file system task.
 * @return      MQTT client queue handle.
 */
static QueueHandle_t app_GetMqttQHandle( void );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       Application thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static void app_Thread( void * parameter_ptr );

#endif /* __APP_PRIV_H__ */

/**
 * @}
 */
