/**
 * @addtogroup Application Application
 * @{
 * @file      blinky.c
 * @brief     Blinking LED demo module
 * @details   Demostrates LEDs blinking on the nRF9160DK and the Thingy:91 demo boards.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup Blinky LED blinking demo
 * @brief     LED blinking demo task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "blinky_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

blinky_interface_t blinky =
{
    .Init               = &blinky_Init,
    .Configure          = &blinky_Configure,
    .EnableMQTT         = &blinky_EnableMQTT,
};

blinky_obj_t blinky_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

void blinky_Thread( void * parameter_ptr )
{
    uint8_t gpio_pin;
    uint16_t i;
    char buffer[ SHORT_MSG_MAX ];
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    twdt.Configure( TWDT_TIMEOUT );
    shared_struct->time_led = 1000;
    shared_struct->num_led = NUM_LEDS;
    shared_struct->mqtt_enable = false;
    Log.InfoPrint( "Blinky task started" );
    bool subscribed = false;

    /* Start LEDs */
    for ( i = 0; ; i = ( i + 1 ) % shared_struct->num_led )
    {
        twdt.Update();

        /* This task will blink the LEDs */
        Log.DebugPrint( "LED #%d", i );
        if ( modem.Registered() )
        {
            if ( shared_struct->mqtt_enable )
            {
                if ( !subscribed )
                {
                    mqtt.Subscribe( "LED" );
                    subscribed = true;
                }
                else
                {
                    sprintf( buffer, "LED: %d", i );
                    mqtt.Send( "LED", buffer );
                }
            }
            else
            {
                if ( subscribed )
                {
                    mqtt.Unsubscribe();
                    subscribed = false;
                }
            }
        }

        gpio.Read( i + LED1_PIN, &gpio_pin );
        gpio.Write( i + LED1_PIN, gpio_pin ^ 0x1 );
        os.Delay( shared_struct->time_led );
        gpio.Read( i + LED1_PIN, &gpio_pin );
        gpio.Write( i + LED1_PIN, gpio_pin ^ 0x1 );
        os.Delay( shared_struct->time_led );
    }
}

error_code_module_t blinky_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( blinky_obj.is_init == false )
    {
        /* Set up blinky task */
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( blinky_Thread,
                                             "Blinky",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY | portPRIVILEGE_BIT );
        MemoryRegion_t regions[] =
        {
            { ( void *)shared_mem,  SHAREDMEM_SIZE,            tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { ( void *)NRF_P0_NS,   sizeof( NRF_GPIO_Type ),   tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
            { 0,                    0,                         0                                                      },
        };
        os.AllocateRegions( handle, regions );

        /* init local RAM objects */
        blinky_obj.is_init = true;
    }
    else
    {
        error = ERROR_BLINKY_ALREADY_INIT;
    }

    return error;
}

void blinky_Configure( uint32_t time_ms, uint32_t num_led )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    if ( time_ms > 50 )
    {
        shared_struct->time_led = time_ms;
    }
    if ( num_led <= NUM_LEDS )
    {
        shared_struct->num_led = num_led;
    }
}

void blinky_EnableMQTT( bool enable )
{
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;

    shared_struct->mqtt_enable = enable;
}

/**
 * @} Blinky
 */

/**
 * @} Application
 */
