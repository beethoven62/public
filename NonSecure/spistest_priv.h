/** @file spistest_priv.h
 *
 * @brief       SPI slave test module (private header)
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup spistest
 * @{
 */

#ifndef __SPISTEST_PRIV_H__
#define __SPISTEST_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "spistest.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define BUF_LEN                 ( 256 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} spistest_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the SPI slave demo thread.
 * @return      Error code
 */
static error_code_module_t spistest_Init( void );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       SPI slave demo thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr   Initial parameters passed into thread at start of thread.
 */
static void spistest_Thread( void * parameter_ptr );

#endif /* __SPISTEST_PRIV_H__ */

/**
 * @}
 */
