/** @file uart_priv.h
 *
 * @brief     Driver for universal asynchronous receiver/transmitter driver module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup uart
 * @{
 */

#ifndef __UART_PRIV_H__
#define __UART_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "uart.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

#define UARTE_PORTS_MAX             ( 2 )
#define UARTE_RXBUF_SIZE            ( 1 )
#define UARTE_RXBUF_STREAM_SIZE     ( 128 )

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    nrfx_uarte_t                    uarte;
    nrfx_uarte_config_t             uarte_config;
    StreamBufferHandle_t            rx_handle;
    uint8_t                         rx_buffer;
    bool                            tx_done;
} uart_port_t;

typedef struct
{
    bool                            is_init;
    bool                            is_init_uarte1;
    bool                            is_init_uarte2;
    uart_port_t                     uart_port[ UARTE_PORTS_MAX ];
    SemaphoreHandle_t               mutex_handle[ UARTE_PORTS_MAX ];
} uart_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize and start the UART interface driver thread.
 * @param[in]   port         UART interface abstract id (debug or utility port).
 * @param[in]   id           UART interface physical id (0, 1, 2, 3).
 * @param[in]   tx_pin       Transmit pin.
 * @param[in]   rx_pin       Receive pin.
 * @param[in]   rts_pin      Ready to send pin.
 * @param[in]   cts_pin      Clear to send pin.
 * @return      Error code.
 */
static error_code_module_t uart_Init( uart_type_t port, uint8_t id, uint8_t tx_pin, uint8_t rx_pin, uint8_t cts_pin, uint8_t rts_pin, int baudrate );

/**
 * @brief       UART interface transmit function.
 * @param[in]   port         UART interface abstract id (debug or utility port).
 * @param[in]   *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 * @return      Number of bytes transmitted.
 */
static int uart_Transmit( uart_type_t port, uint8_t *data, uint8_t len );

/**
 * @brief       UART interface receive function.
 * @param[in]   port         UART interface abstract id (debug or utility port).
 * @param[out]  *data        Pointer to data buffer.
 * @param[in]   len          Length of data buffer.
 * @return      Number of bytes received.
 */
static size_t uart_Receive( uart_type_t port, uint8_t *data, uint8_t len );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       UART interface receiver interrupt callback function.
 * @param[in]   *p_event     Pointer to event type( tx/rx complete, error), data, nbytes transferred.
 * @param[in]   *p_context   Pointer to event context ( UARTn ).
 */
static void uart_EventHandler( nrfx_uarte_event_t const *p_event, void *p_context);

#endif /* __UART_PRIV_H__ */

/**
 * @}
 */
