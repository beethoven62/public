/**
 * @addtogroup Driver
 * @{
 * @file      uart.c
 * @brief     Driver for universal asynchronous receiver/transmitter driver module
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup  uart UART driver module
 * @defgroup  uart UART driver module
 * @brief     Universal asynchronous receiver/transmitter interface driver
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "uart_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

uart_driver_t uart =
{
    .Init               = &uart_Init,
    .Transmit           = &uart_Transmit,
    .Receive            = &uart_Receive,
};

uart_obj_t uart_obj =
{
    .is_init            = false,
#if NRFX_UARTE1_ENABLED
    .is_init_uarte1     = false,
#else
    .is_init_uarte1     = true,
#endif
#if NRFX_UARTE2_ENABLED
    .is_init_uarte2     = false,
#else
    .is_init_uarte2     = true,
#endif
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t uart_Init( uart_type_t port, uint8_t id, uint8_t tx_pin, uint8_t rx_pin, uint8_t cts_pin, uint8_t rts_pin, int baudrate )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( uart_obj.is_init == false )
    {
        /* Initialize UART id */
        switch ( id )
        {
        case 1:
#if NRFX_UARTE1_ENABLED
            uart_obj.uart_port[ port ].uarte.p_reg = NRF_UARTE1;
            uart_obj.uart_port[ port ].uarte.drv_inst_idx = NRFX_UARTE1_INST_IDX;
#if !defined(NRFX_CONFIG_API_VER_2_10) || NRFX_CONFIG_API_VER_2_10 == 0
            NVIC_ClearPendingIRQ( UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn );
            NVIC_EnableIRQ( UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn );
#else
            NVIC_ClearPendingIRQ( SPIM1_SPIS1_TWIM1_TWIS1_UARTE1_IRQn );
            NVIC_EnableIRQ( SPIM1_SPIS1_TWIM1_TWIS1_UARTE1_IRQn );
#endif
#endif
            uart_obj.is_init_uarte1 = true;
            uart_obj.mutex_handle[ port ] = os.CreateMutex();
            break;
        case 2:
#if NRFX_UARTE2_ENABLED
            uart_obj.uart_port[ port ].uarte.p_reg = NRF_UARTE2;
            uart_obj.uart_port[ port ].uarte.drv_inst_idx = NRFX_UARTE2_INST_IDX;
#if !defined(NRFX_CONFIG_API_VER_2_10) || NRFX_CONFIG_API_VER_2_10 == 0
            NVIC_ClearPendingIRQ( UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQn );
            NVIC_EnableIRQ( UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQn );
#else
            NVIC_ClearPendingIRQ( SPIM2_SPIS2_TWIM2_TWIS2_UARTE2_IRQn );
            NVIC_EnableIRQ( SPIM2_SPIS2_TWIM2_TWIS2_UARTE2_IRQn );
#endif
#endif
            uart_obj.is_init_uarte2 = true;
            uart_obj.mutex_handle[ port ] = os.CreateMutex();
            break;
        }

        if ( error == NO_ERROR )
        {
            uart_obj.uart_port[ port ].uarte_config.p_context = &uart_obj.uart_port[ port ];
            uart_obj.uart_port[ port ].uarte_config.baudrate = baudrate;
            uart_obj.uart_port[ port ].uarte_config.pseltxd = tx_pin;
            uart_obj.uart_port[ port ].uarte_config.pselrxd = rx_pin;
            uart_obj.uart_port[ port ].uarte_config.pselcts = cts_pin;
            uart_obj.uart_port[ port ].uarte_config.pselrts = rts_pin;
            if ( cts_pin == NO_PIN || rts_pin == NO_PIN )
            {
                uart_obj.uart_port[ port ].uarte_config.hal_cfg.hwfc = NRF_UARTE_HWFC_DISABLED;
            }
            else
            {
                uart_obj.uart_port[ port ].uarte_config.hal_cfg.hwfc = NRF_UARTE_HWFC_ENABLED;
            }
            uart_obj.uart_port[ port ].uarte_config.interrupt_priority = NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY;
            uart_obj.uart_port[ port ].uarte_config.hal_cfg.parity = NRF_UARTE_PARITY_EXCLUDED;
            uart_obj.uart_port[ port ].uarte_config.hal_cfg.stop = NRF_UARTE_STOP_ONE;

            uart_obj.uart_port[ port ].rx_handle = os.CreateStream( UARTE_RXBUF_STREAM_SIZE, UARTE_RXBUF_SIZE );
            nrfx_uarte_init( &uart_obj.uart_port[ port ].uarte, &uart_obj.uart_port[ port ].uarte_config, uart_EventHandler );
            nrfx_uarte_rx( &uart_obj.uart_port[ port ].uarte, &uart_obj.uart_port[ port ].rx_buffer, sizeof( uart_obj.uart_port[ port ].rx_buffer ) );

            uart_obj.uart_port[ port ].tx_done = true;
        }

        /* init local RAM objects */
        uart_obj.is_init = ( uart_obj.is_init_uarte1 && uart_obj.is_init_uarte2 );
    }
    else
    {
        error = ERROR_UART_ALREADY_INIT;
    }

    return error;
}

int uart_Transmit( uart_type_t port, uint8_t *data, uint8_t len )
{
    uint32_t iter;
    nrfx_err_t error = NRFX_ERROR_TIMEOUT;

    if ( os.TakeSemaphore( uart_obj.mutex_handle[ port ], QUEUE_WAIT_TIME ) )
    {
        for ( iter = 0; !uart_obj.uart_port[ port ].tx_done && iter < 0xffffffff; iter++ )
        {
        }

        uart_obj.uart_port[ port ].tx_done = false;
        if ( ( error = nrfx_uarte_tx( &uart_obj.uart_port[ port ].uarte, data, len ) ) == NRFX_SUCCESS )
        {
            for ( iter = 0; !uart_obj.uart_port[ port ].tx_done && iter < 0xffffffff; iter++ )
            {
            }
        }
        os.GiveSemaphore( uart_obj.mutex_handle[ port ] );
    }

    return error;
}

size_t uart_Receive( uart_type_t port, uint8_t *data, uint8_t len )
{
    size_t result;

    if ( os.TakeSemaphore( uart_obj.mutex_handle[ port ], QUEUE_WAIT_TIME ) )
    {
        result = os.StreamReceive( uart_obj.uart_port[ port ].rx_handle, data, len, 0 );
        os.GiveSemaphore( uart_obj.mutex_handle[ port ] );
    }

    return result;
}

void uart_EventHandler( nrfx_uarte_event_t const *p_event, void *p_context)
{
    uart_port_t *uart_port = ( uart_port_t* ) p_context;

    if ( uart_port != NULL )
    {
        switch ( p_event->type )
        {
        case NRFX_UARTE_EVT_TX_DONE:
            uart_port->tx_done = true;
            break;
        case NRFX_UARTE_EVT_RX_DONE:
            os.StreamSend( uart_port->rx_handle, p_event->data.rxtx.p_data, 1, 0 );
            nrfx_uarte_rx( &uart_port->uarte, &uart_port->rx_buffer, UARTE_RXBUF_SIZE );
            break;
        case NRFX_UARTE_EVT_ERROR:
            break;
        }
    }
}

#if NRFX_UARTE1_ENABLED
#if !defined(NRFX_CONFIG_API_VER_2_10) || NRFX_CONFIG_API_VER_2_10 == 0
void UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler( void )
#else
void SPIM1_SPIS1_TWIM1_TWIS1_UARTE1_IRQHandler( void )
#endif
{
    nrfx_uarte_1_irq_handler();
}
#endif

#if NRFX_UARTE2_ENABLED
#if NRFX_CONFIG_API_VER_2_10 == 0
void UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler( void )
#else
void SPIM2_SPIS2_TWIM2_TWIS2_UARTE2_IRQHandler( void )
#endif
{
    nrfx_uarte_2_irq_handler();
}
#endif

/**
 * @} uart
 */

/**
 * @} Driver
 */
