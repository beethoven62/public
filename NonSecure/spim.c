/**
 * @addtogroup Driver
 * @{
 * @file      spim.c
 * @brief     Driver for serial peripheral interface master module
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup spim SPI master driver module
 * @brief     SPI master driver
 * @{
 */

#if NRFX_SPIM_ENABLED
/*************************************************************************************************************************************
 * Includes
*/
#include "spim_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

spim_driver_t spim =
{
    .Init               = &spim_Init,
    .Xfer               = &spim_Xfer,
    .NRxBytes           = &spim_NRxBytes,
};

spim_obj_t spim_obj =
{
    .is_init            = false,
    .rxbuf_nbytes       = 0,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t spim_Init( uint8_t id, uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t ss_pin, nrf_spim_frequency_t freq )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( spim_obj.is_init == false )
    {
        /* Initialize SPIM id */
        switch ( id )
        {
        case 0:
#if NRFX_SPIM0_ENABLED
            spim_obj.spim.p_reg = NRF_SPIM0;
            spim_obj.spim.drv_inst_idx = NRFX_SPIM0_INST_IDX;
#endif
            break;
        case 1:
#if NRFX_SPIM1_ENABLED
            spim_obj.spim.p_reg = NRF_SPIM1;
            spim_obj.spim.drv_inst_idx = NRFX_SPIM1_INST_IDX;
#endif
            break;
        case 2:
#if NRFX_SPIM2_ENABLED
            spim_obj.spim.p_reg = NRF_SPIM2;
            spim_obj.spim.drv_inst_idx = NRFX_SPIM2_INST_IDX;
#endif
            break;
        case 3:
#if NRFX_SPIM3_ENABLED
            spim_obj.spim.p_reg = NRF_SPIM3;
            spim_obj.spim.drv_inst_idx = NRFX_SPIM3_INST_IDX;
#endif
            break;
        default:
            error = ERROR_SPIM_NOT_INIT;
            break;
        }

        if ( error == NO_ERROR )
        {
            spim_obj.config.sck_pin = sck_pin;
            spim_obj.config.mosi_pin = mosi_pin;
            spim_obj.config.miso_pin = miso_pin;
            spim_obj.config.ss_pin = ss_pin;
            spim_obj.config.ss_active_high = false;
            spim_obj.config.irq_priority = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY;
            spim_obj.config.orc = 0x0f;
            spim_obj.config.frequency = freq;
            spim_obj.config.mode = NRF_SPIM_MODE_0;
            spim_obj.config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
            spim_obj.config.miso_pull = NRF_GPIO_PIN_NOPULL;

            nrfx_spim_init( &spim_obj.spim, &spim_obj.config, spim_EventHandler, &spim_obj );
            NVIC_SetPriority( SPIM_IRQn, SPIM_DEFAULT_CONFIG_IRQ_PRIORITY );
            NVIC_EnableIRQ( SPIM_IRQn );
        }

        /* init local RAM objects */
        spim_obj.is_init = true;
    }
    else
    {
        error = ERROR_SPIM_ALREADY_INIT;
    }

    return error;
}

void spim_Xfer( uint8_t *tx_data, uint8_t *rx_data, size_t len )
{
    spim_obj.rxbuf_nbytes = 0;
    spim_obj.desc.p_tx_buffer = tx_data;
    spim_obj.desc.tx_length = len;
    spim_obj.desc.p_rx_buffer = rx_data;
    spim_obj.desc.rx_length = len;
    nrfx_spim_xfer( &spim_obj.spim, &spim_obj.desc, 0 );
}

void spim_EventHandler( nrfx_spim_evt_t const *p_event, void *p_context )
{
    spim_obj_t *context = ( spim_obj_t *)p_context;

    switch ( p_event->type )
    {
    case NRFX_SPIM_EVENT_DONE:
        context->rxbuf_nbytes = p_event->xfer_desc.rx_length;
        break;
    default:
        break;
    }
}

uint32_t spim_NRxBytes( void )
{
    return spim_obj.rxbuf_nbytes;
}
#endif

/**
 * @} spim
 */

/**
 * @} Driver
 */
