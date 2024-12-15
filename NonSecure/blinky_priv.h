/** @file blinky_priv.h
 *
 * @brief       Blinking LED demo module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup Blinky
 * @{
 */

#ifndef __BLINKY_PRIV_H__
#define __BLINKY_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "blinky.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
} blinky_obj_t ;

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
static error_code_module_t blinky_Init( void );

/**
 * @brief       Configure the timing and maximum number of LEDs to light up.
 * @param[in]   timing      Minimum time between LEDs, in milliseconds.
 * @param[in]   nleds       Maximum number of LEDs to light.
 */
static void blinky_Configure( uint32_t time_ms, uint32_t n_leds );

/**
 * @brief       Enable/Disable publishing to MQTT broker.
 * @param[in]   enable      Enable = true, Disable = false.
 */
static void blinky_EnableMQTT( bool enable );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       LED blinking demo thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static void blinky_Thread( void * parameter_ptr );

#endif /* __BLINKY_PRIV_H__ */

/**
 * @}
 */
