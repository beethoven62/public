/**
 * @addtogroup Application
 * @{
 * @file      fs.c
 * @brief     File system module
 * @details   LittleFS implementation of file system.
 * @author    Johnas Cukier
 * @date      Apr 2023
 */

/**
 * @defgroup fs File system
 * @brief     File system task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "fs_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

fs_interface_t fs =
{
    .Init               = &fs_Init,
};

fs_obj_t fs_obj =
{
    .is_init            = false,
};

uint8_t nvdata[ NVDATA_SIZE ] __attribute__( ( section( ".nvdata" ) ) );

/*************************************************************************************************************************************
 * Private Functions Definition
 */
void fs_Thread( void * parameter_ptr )
{
    error_code_module_t error;
    fs_msg_t msg;
    uint8_t read_buffer[ FS_BUFFER_SIZE ];
    uint8_t prog_buffer[ FS_BUFFER_SIZE ];
    uint8_t lookahead_buffer[ FS_BUFFER_SIZE ];
    uint8_t file_buffer[ FS_BUFFER_SIZE ];
    const struct lfs_config cfg =
    {
        // Board context
        .context = NULL,

        // block device operations - functions provided by host device
        .read  = fs_read,
        .prog  = fs_prog,
        .erase = fs_erase,
        .sync  = fs_sync,

        // block device configuration
        .read_size = 4,
        .prog_size = 4,
        .block_size = 4096,
        .block_count = 29,
        .cache_size = FS_BUFFER_SIZE,
        .lookahead_size = FS_BUFFER_SIZE,
        .block_cycles = 500,

        // static allocation
        .read_buffer = read_buffer,
        .prog_buffer = prog_buffer,
        .lookahead_buffer = lookahead_buffer,
    };
    const struct lfs_file_config file_cfg =
    {
        .buffer = file_buffer,
        .attrs = NULL,
        .attr_count = 0,
    };

    twdt.Configure( TWDT_TIMEOUT );
    Log.InfoPrint( "File system task started" );

    fs_test_nvmc( &cfg, &file_cfg );

    for( ; ; )
    {
        twdt.Update();
        if ( os.QueueReceive( app.GetFsQHandle(), &msg, os.Ms2Ticks( TWDT_KICK_TIME ) ) )
        {
            // To do: add processing of file system requests here
        }
    }
}

void fs_test_nvmc( const struct lfs_config *cfg, const struct lfs_file_config *file_cfg )
{
    lfs_t lfs;
    lfs_file_t file;
    // configuration of the filesystem is provided by this struct
    uint32_t boot_count = 0;
    TickType_t time = os.GetTickCount();

    if ( lfs_mount( &lfs, cfg ) != 0 )
    {
        lfs_format( &lfs, cfg );
        lfs_mount( &lfs, cfg );
    }

    // read current count
    lfs_file_opencfg( &lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT, file_cfg );
    lfs_file_read( &lfs, &file, &boot_count, sizeof( boot_count ) );
    Log.InfoPrint( "boot_count initialized: %d", boot_count );

    // update boot count
    boot_count += 1;
    lfs_file_rewind( &lfs, &file );
    lfs_file_write( &lfs, &file, &boot_count, sizeof( boot_count ) );
    Log.InfoPrint( "boot_count written: %d", boot_count );

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close( &lfs, &file );

    // read current time
    lfs_file_opencfg( &lfs, &file, "time", LFS_O_RDWR | LFS_O_CREAT, file_cfg );
    lfs_file_read( &lfs, &file, &time, sizeof( time ) );
    Log.InfoPrint( "time initialized: %d, %d (ms)", time, os.Ticks2Ms( time ) );

    // update time
    time += os.GetTickCount();
    lfs_file_rewind( &lfs, &file );
    lfs_file_write( &lfs, &file, &time, sizeof( time ) );
    Log.InfoPrint( "time written: %d, %d (ms)", time, os.Ticks2Ms( time ) );

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close( &lfs, &file );

    // Read back to see if value persisted (works for RAM disk too)
    boot_count = 0;
    Log.InfoPrint( "boot_count initialized: %d", boot_count );
    lfs_file_opencfg( &lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT, file_cfg );
    lfs_file_read( &lfs, &file, &boot_count, sizeof( boot_count ) );
    Log.InfoPrint( "boot_count read: %d", boot_count );
    lfs_file_close( &lfs, &file );

    // Read back to see if value persisted (works for RAM disk too)
    time = 0;
    Log.InfoPrint( "time initialized: %d, %d (ms)", time, os.Ticks2Ms( time ) );
    lfs_file_opencfg( &lfs, &file, "time", LFS_O_RDWR | LFS_O_CREAT, file_cfg );
    lfs_file_read( &lfs, &file, &time, sizeof( boot_count ) );
    Log.InfoPrint( "time read: %d, %d (ms)", time, os.Ticks2Ms( time ) );

    // Close files
    lfs_file_close( &lfs, &file );

    // release any resources we were using
    lfs_unmount( &lfs );
}

int32_t fs_read( const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size )
{
    // read data
    memcpy( buffer, &nvdata[ block * cfg->block_size + off ], size );

    return 0;
}

int32_t fs_prog( const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size )
{
    // program data
    nrfx_nvmc_bytes_write( ( uint32_t ) &nvdata[ block * cfg->block_size + off ], buffer, size );

    return 0;
}

int32_t fs_erase( const struct lfs_config *cfg, lfs_block_t block )
{
    // erase block (flash page = 4096 bytes)
    nrfx_nvmc_page_erase( ( uint32_t ) &nvdata[ block * cfg->block_size ] );

    return 0;
}

int32_t fs_sync( const struct lfs_config *cfg )
{
    // sync does nothing
    ( void ) cfg;

    return 0;
}

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t fs_Init( void )
{
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( fs_obj.is_init == false )
    {
        error = NO_ERROR;

        /* Set up file system task */
        shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
        shared_struct->fs_queue = os.CreateQueue( QUEUE_LEN, sizeof( fs_msg_t ) );
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( fs_Thread,
                                             "FS",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_LOW_PRIORITY );
        MemoryRegion_t regions[] =
        {
            { ( void *)shared_mem,  SHAREDMEM_SIZE,            tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { ( void *)NRF_NVMC,    sizeof( NRF_NVMC_Type ),   tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { ( void *)nvdata,      NVDATA_SIZE,               tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
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
        fs_obj.is_init = true;
    }
    else
    {
        error = ERROR_FS_ALREADY_INIT;
    }

    return error;
}

/**
 * @} fs
 */

/**
 * @} Application
 */
