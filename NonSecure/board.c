/**
 * @addtogroup Driver Driver
 * @{
 * @file      board.c
 * @brief     Board setup module
 * @details   Sets up the peripherals on the nRF9160DK and the Thingy:91 demo boards.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup Board Board setup of peripherals
 * @brief    Board specific driver (nRF9160DK, Thingy:91, Everion+).
 * @{
 */

/*************************************************************************************************************************************
* Includes
*/
#include "board_priv.h"

/***************************************************************************************************************************
 * Global variables
 */
board_interface_t board =
{
    .Init               = &board_Init,
};

static board_obj_t board_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t board_Init( void )
{
    uint16_t i;
    uint8_t pkt[ 2 ] = { 0x2b, 0x99 };
    uint32_t gpio_dir;
    uint32_t gpio_init;
    error_code_module_t error = NO_ERROR;

    if ( board_obj.is_init == false )
    {
        /* Set up GPIOs for LEDs */
        gpio_dir = 0;
        gpio_init = 0;
        for ( i = LED1_PIN; i < LED1_PIN + NUM_LEDS; i++ )
        {
            gpio_dir |= ( 1 << i );
        }
#if NRFX_SPIS_ENABLED
        gpio_dir |= ( 1 << SPI0_INT_PIN );
        gpio_init |= ( 1 << SPI0_INT_PIN );
#endif
        gpio.Init( gpio_dir, gpio_init );

        /* Set up UART */
#if TARGET_DEVICE_EVERION_PLUS
        uart.Init( uart_debug_port, 2, UART2_TX_PIN, UART2_RX_PIN, UART2_CTS_PIN, UART2_RTS_PIN, NRF_UARTE_BAUDRATE_115200 );
#else
        uart.Init( uart_debug_port, 1, UART1_TX_PIN, UART1_RX_PIN, NO_PIN, NO_PIN, NRF_UARTE_BAUDRATE_115200 );
        uart.Init( uart_util_port, 2, UART2_TX_PIN, UART2_RX_PIN, UART2_CTS_PIN, UART2_RTS_PIN, NRF_UARTE_BAUDRATE_1000000 );
#endif

        /* Set up I2C */
#if TARGET_DEVICE_THINGY91
        twim.Init( 0, TWIM0_SCL_PIN, TWIM0_SDA_PIN );
        twim.Transmit( ADP5360_ADDR, pkt, sizeof( pkt ) );
#endif

        /* Set up SPI Master */
#if TARGET_DEVICE_NRF9160DK
#if NRFX_SPIM_ENABLED
        spim.Init( 0, SPI0_CLK_PIN, SPI0_MOSI_PIN, SPI0_MISO_PIN, SPI0_SS_PIN, NRF_SPIM_FREQ_4M );
#endif
#if NRFX_SPIS_ENABLED
        spis.Init( 0, SPI0_CLK_PIN, SPI0_MOSI_PIN, SPI0_MISO_PIN, SPI0_SS_PIN, SPI0_INT_PIN );
#endif
#endif

        /* init local RAM objects */
        board_obj.is_init = true;
    }
    else
    {
        error = ERROR_BOARD_ALREADY_INIT;
    }

    return error;
}

#if defined(NRFX_CONFIG_API_VER_2_10) && NRFX_CONFIG_API_VER_2_10 == 1
void SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQHandler( void )
#else
void UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler( void )
#endif
{
#if NRFX_SPIS_ENABLED && NRFX_SPIS0_ENABLED
    nrfx_spis_0_irq_handler();
#endif
#if NRFX_SPIM0_ENABLED
    nrfx_spim_0_irq_handler();
#endif
}

/**
 * @} Board
 */

/**
 * @} Driver
 */
