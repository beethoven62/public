/** @file spimtest_priv.h
 *
 * @brief       SPI master test module (private header)
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup spimtest
 * @{
 */

#ifndef __SPIMTEST_PRIV_H__
#define __SPIMTEST_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "spimtest.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define BUF_LEN                 ( 256 )
#define SPIM_WAIT_TIME          ( 50 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} spimtest_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the SPI master demo thread.
 * @return      Error code
 */
static error_code_module_t spimtest_Init( void );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       SPI master demo thread function. Executes and responds to requests.
 * @param[in]   pvParameters    Initial parameters passed into thread at start of thread.
 */
static void spimtest_Thread( void * parameter_ptr );

#endif /* __SPIMTEST_PRIV_H__ */

/**
 * @}
 */
