/**
 * @addtogroup Driver Driver
 * @{
 *
 * @file      uart.h
 * @brief     Driver for universal asynchronous receiver/transmitter driver module (public header)
 * @author    Johnas Cukier
 * @date      January 2023
 */

/**
 * @addtogroup uart
 * @{
 */
#ifndef __UART_H__
#define __UART_H__

/***************************************************************************************************************************
 * Includes
 */

#include <nrfx_uarte.h>
#include "os.h"
#include "app.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

#define NO_PIN      ( 0xff )

/***************************************************************************************************************************
 * Public data structures and typedefs
 */

typedef enum
{
    uart_debug_port,        /*!< Debug UART port */
    uart_util_port,         /*!< Utility UART port (modem trace) */
} uart_type_t;

typedef struct
{
    error_code_module_t ( *Init )( uart_type_t port, uint8_t id, uint8_t tx_pin, uint8_t rx_pin, uint8_t cts_pin, uint8_t rts_pin, int baudrate );
    int ( *Transmit )( uart_type_t port, uint8_t *data, uint8_t len );
    size_t ( *Receive )( uart_type_t port, uint8_t *data, uint8_t len );
} const uart_driver_t;

/***************************************************************************************************************************
 * Public variables
 */

extern uart_driver_t uart;

#endif /* __UART_H__ */

/**
 * @} uart
 */

/**
 * @} Driver
 */
