/**
 * @addtogroup Driver
 * @{
 * @file      spis.c
 * @brief     Driver for serial peripheral interface slave module
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup spis SPI slave driver module
 * @brief     SPI slave driver
 * @{
 */

#if NRFX_SPIS_ENABLED
/*************************************************************************************************************************************
 * Includes
*/
#include "spis_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

spis_driver_t spis =
{
    .Init               = &spis_Init,
    .NRxBytes           = &spis_NRxBytes,
    .RxDone             = &spis_RxDone,
    .BufSet             = &spis_BufSet,
};

spis_obj_t spis_obj =
{
    .is_init            = false,
    .rx_done            = false,
    .rxbuf_nbytes       = 0,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t spis_Init( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, uint8_t int_pin )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( spis_obj.is_init == false )
    {
        /* Initialize SPIS id */
        switch ( id )
        {
        case 0:
#if NRFX_SPIS0_ENABLED
            spis_obj.spis.p_reg = NRF_SPIS0;
            spis_obj.spis.drv_inst_idx = NRFX_SPIS0_INST_IDX;
#endif
            break;
        case 1:
#if NRFX_SPIS1_ENABLED
            spis_obj.spis.p_reg = NRF_SPIS1;
            spis_obj.spis.drv_inst_idx = NRFX_SPIS1_INST_IDX;
#endif
            break;
        case 2:
#if NRFX_SPIS2_ENABLED
            spis_obj.spis.p_reg = NRF_SPIS2;
            spis_obj.spis.drv_inst_idx = NRFX_SPIS2_INST_IDX;
#endif
            break;
        case 3:
#if NRFX_SPIS3_ENABLED
            spis_obj.spis.p_reg = NRF_SPIS3;
            spis_obj.spis.drv_inst_idx = NRFX_SPIS3_INST_IDX;
#endif
            break;
        default:
            error = ERROR_SPIS_NOT_INIT;
            break;
        }

        if ( error == NO_ERROR )
        {
            spis_obj.config.sck_pin = sck_pin;
            spis_obj.config.mosi_pin = mosi_pin;
            spis_obj.config.miso_pin = miso_pin;
            spis_obj.config.csn_pin = ss_pin;
            spis_obj.config.irq_priority = NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY;
            spis_obj.config.orc = 0xfe;
            spis_obj.config.def = 0xff;
            spis_obj.config.mode = NRF_SPIS_MODE_0;
            spis_obj.config.bit_order = NRF_SPIS_BIT_ORDER_MSB_FIRST;
            spis_obj.config.csn_pullup = NRF_GPIO_PIN_NOPULL;
            spis_obj.config.miso_drive = NRF_GPIO_PIN_S0S1;
            spis_obj.txbuf = NULL;
            spis_obj.rxbuf = NULL;
            spis_obj.len = 0;
            spis_obj.int_pin = int_pin;

            gpio.Write( spis_obj.int_pin, 1 );

            nrfx_spis_init( &spis_obj.spis, &spis_obj.config, spis_EventHandler, &spis_obj );
            NVIC_SetPriority( SPIS_IRQn, SPIS_DEFAULT_CONFIG_IRQ_PRIORITY );
            NVIC_EnableIRQ( SPIS_IRQn );
        }

        /* init local RAM objects */
        spis_obj.is_init = true;
    }
    else
    {
        error = ERROR_SPIS_ALREADY_INIT;
    }

    return error;
}

void spis_EventHandler( nrfx_spis_evt_t const *p_event, void *p_context )
{
    spis_obj_t *context = ( spis_obj_t *)p_context;

    switch ( p_event->evt_type )
    {
    case NRFX_SPIS_XFER_DONE:
        context->rxbuf_nbytes = p_event->rx_amount;
        gpio.Write( context->int_pin, 1 );
        context->rx_done = true;
        break;
    case NRFX_SPIS_BUFFERS_SET_DONE:
        gpio.Write( context->int_pin, 0 );
        context->rx_done = false;
        break;
    default:
        break;
    }
}

uint32_t spis_NRxBytes( void )
{
    return spis_obj.rxbuf_nbytes;
}

void spis_BufSet( uint8_t *txbuf, uint8_t *rxbuf, size_t len )
{
    spis_obj.rxbuf_nbytes = 0;
    spis_obj.txbuf = txbuf;
    spis_obj.rxbuf = rxbuf;
    spis_obj.len = len;
    nrfx_spis_buffers_set( &spis_obj.spis, txbuf, len, rxbuf, len );
}

bool spis_RxDone( void )
{
    return spis_obj.rx_done;
}
#endif

/**
 * @} spis
 */

/**
 * @} Driver
 */
