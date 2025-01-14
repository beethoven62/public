/**
 * @addtogroup Application Application
 * @{
 * @file      spimtest.c
 * @brief     SPI master test module
 * @details   Demostrates SPI master on the nRF9160DK demo boards.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup spimtest SPI master demo
 * @brief     SPI master test demo task
 * @{
 */

#if NRFX_SPIM_ENABLED
/*************************************************************************************************************************************
 * Includes
*/
#include "spimtest_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

spimtest_interface_t spimtest =
{
    .Init               = &spimtest_Init,
};

spimtest_obj_t spimtest_obj =
{
    .is_init            = false,
};

/*************************************************************************************************************************************
 * Public Functions Definition
 */

void spimtest_Thread( void * parameter_ptr )
{
    uint32_t i, error, error_rate, old_rate;
    shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
    const uint8_t txbuf[] =
    {
        0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
        0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0,
        0xdf, 0xde, 0xdd, 0xdc, 0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xd6, 0xd5, 0xd4, 0xd3, 0xd2, 0xd1, 0xd0,
        0xcf, 0xce, 0xcd, 0xcc, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2, 0xc1, 0xc0,
        0xbf, 0xbe, 0xbd, 0xbc, 0xbb, 0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0xb2, 0xb1, 0xb0,
        0xaf, 0xae, 0xad, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0,
        0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90,
        0x8f, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80,
        0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70,
        0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60,
        0x5f, 0x5e, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50,
        0x4f, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40,
        0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
        0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
        0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
        0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
    };
    uint8_t rxbuf[BUF_LEN];

    twdt.Configure( TWDT_TIMEOUT );
    Log.InfoPrint( "SPI master test task started" );

    /* Start SPI transfer */
    Log.DebugPrint( "SPI0 xfer started" );
    error_rate = 0;
    for ( ; ; )
    {
        twdt.Update();
        memset( rxbuf, 0, BUF_LEN );
        spim.Xfer( ( uint8_t *) txbuf, rxbuf, sizeof( txbuf ) );
        while( spim.NRxBytes() == 0 )
        {
            os.Delay( SPIM_WAIT_TIME );
        }

        error = 0;
        for ( i = 0; i < sizeof( rxbuf ); i++ )
        {
            if ( rxbuf[i] != i )
            {
                error++;
            }
        }
        old_rate = error_rate;
        error_rate = ( error * 1000 ) / sizeof( rxbuf );

        /* Print status or allow other tasks to run */
        if ( error_rate != old_rate )
        {
            Log.DebugPrint( "SPI0 error rate: %d.%d%%", error_rate / 10, error_rate % 10 );
        }
    }
}

error_code_module_t spimtest_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( spimtest_obj.is_init == false )
    {
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( spimtest_Thread,
                                             "SPIMTest",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY | portPRIVILEGE_BIT );
        MemoryRegion_t regions[] =
        {
            { ( void *)shared_mem,  SHAREDMEM_SIZE,            tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { 0,                    0,                         0                                                      },
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
        spimtest_obj.is_init = true;
    }
    else
    {
        error = ERROR_SPIMTEST_ALREADY_INIT;
    }

    return error;
}
#endif

/**
 * @} SPIMTest
 */

/**
 * @} Application
 */
