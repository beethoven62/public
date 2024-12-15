/**
 * @addtogroup Application Application
 * @{
 * @file      cli.c
 * @brief     Command line interface module
 * @details   Acts as a user interface and takes in user input via serial terminal.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup CLI Command line interface module
 * @brief     Command line interface task
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include "cli_priv.h"

/***************************************************************************************************************************
 * Global variables
 */

cli_interface_t cli =
{
    .Init               = &cli_Init,
    .Enable             = &cli_Enable,
    .Bindings           = &cli_Bindings,
    .Writechar          = &cli_Writechar,
};

cli_obj_t cli_obj =
{
    .is_init            = false,
    .cli_active         = true,
};

/*************************************************************************************************************************************
 * Private Functions Definition
 */

void cli_Thread( void *parameter_ptr )
{
    char byte;
    EmbeddedCli *embedded_cli;
    CLI_UINT cli_buffer[ BYTES_TO_CLI_UINTS( CLI_BUFFER_SIZE ) ];
    CliCommandBinding binding[] =
    {
        {
            "set-led",
            "Set LED timing in ms/number of LEDs: set-led 1000 4",
            true,
            NULL,
            cli_Onled
        },
        {
            "set-level",
            "Set log level (0 = no log, 1 = error, 2 = info, 3 = debug, 4 = insane): set-level 3",
            true,
            NULL,
            cli_Onsetlevel
        },
        {
            "get-level",
            "Get log level (0 = no log, 1 = error, 2 = info, 3 = debug, 4 = insane): get-level",
            false,
            NULL,
            cli_Ongetlevel
        },
        {
            "get-status",
            "Get the system status",
            false,
            NULL,
            cli_Ongetstatus
        },
        {
            "show-tasks",
            "Show logs from task list (up to 3)",
            true,
            NULL,
            cli_Onshowtasks
        },
        {
            "hide-tasks",
            "Hide logs from task list (up to 3)",
            true,
            NULL,
            cli_Onhidetasks
        },
        {
            "clear-tasks",
            "Clear task filter lists",
            false,
            NULL,
            cli_Oncleartasks
        },
        {
            "modem",
            "Send modem commands",
            true,
            NULL,
            cli_Onmodemcommand
        },
    };

    embedded_cli = cli_Bindings( binding, sizeof( binding ) / sizeof( CliCommandBinding ), cli_buffer );
    embedded_cli->onCommand = cli_Oncommand;
    embedded_cli->writeChar = cli_Writechar;
    cli_obj.log_level = Log.GetLevel();
    twdt.Configure( 0 );        // Disable CLI task watchdog

    Log.InfoPrint( "CLI task started" );

    /**
     * @details
     * Logical flow:
     *      1. CLI active?
     *      1a. CLI is active:
     *          2. Character received?
     *          2a. Character received:
     *                  Process character
     *                  Yield CPU only
     *          2b. No character received:
     *                  Delay one tick time
     *      1b. CLI is not active:
     *              Delay watchdog kick time
     */
    for( ; ; )
    {
        twdt.Update();

        if ( cli_obj.cli_active )
        {
            if ( ( byte = getchar() ) != 0xff )
            {
                if ( Log.GetLevel() != loglevel_none )
                {
                    cli_obj.log_level = Log.GetLevel();
                    Log.SetLevel( loglevel_none );
                }
                embeddedCliReceiveChar( embedded_cli, byte );

                if ( byte == '\r' )
                {
                    Log.SetLevel( cli_obj.log_level );
                }

                embeddedCliProcess( embedded_cli );
            }
            else
            {
                // No byte received, so wait at least one tick time
                os.Delay( CLI_WAIT_TIME );
            }
        }
        else
        {
            // CLI not active, kick watchdog at normal watchdog kick rate
            os.Delay( CLI_WAIT_TIME );
        }
    }
}

/**
 * Embedded CLI functions
 */

void cli_Oncommand( EmbeddedCli *embedded_cli, CliCommand *command )
{
    Log.DebugPrint( "Received: %s", command->name );
    embeddedCliTokenizeArgs( command->args );
    char cmd[3];
    char resp[ SHORT_MSG_MAX ];

    cmd[0] = tolower( command->name[0] );
    cmd[1] = tolower( command->name[1] );
    cmd[2] = 0;

    if ( strcmp( "at", cmd ) == 0 )
    {
        modem.ATCmd( ( char *)command->name, resp );
        Log.InfoPrint( "Modem command: %s", command->name );
        Log.InfoPrint( "Modem response: %s", resp );
    }
    else
    {
        Log.ErrorPrint( "Error: Not Supported!" );
    }
}

void cli_Onled( EmbeddedCli *embedded_cli, char *args, void *context )
{
    uint32_t result;
    int32_t parms[ 2 ];

    result = cli_Getparms( args, parms );

    if ( result < embeddedCliGetTokenCount( args ) )
    {
        Log.ErrorPrint( "No valid arguments" );
    }
    else
    {
        blinky.Configure( parms[ 0 ], parms[ 1 ] );
    }
}

void cli_Onsetlevel( EmbeddedCli *embedded_cli, char *args, void *context )
{
    uint32_t result;
    int32_t parms[ 1 ];

    result = cli_Getparms( args, parms );

    if ( result < embeddedCliGetTokenCount( args ) )
    {
        Log.ErrorPrint( "No valid arguments" );
    }
    else
    {
        cli_obj.log_level = ( log_level_t ) parms[ 0 ];
        Log.SetLevel( cli_obj.log_level );
    }
}

void cli_Ongetlevel( EmbeddedCli *embedded_cli, char *args, void *context )
{
    Log.Print( "Log level: %d\r\n", cli_obj.log_level );
}

void cli_Ongetstatus( EmbeddedCli *embedded_cli, char *args, void *context )
{
    twdt.Report();
    dmm.Report( dmm_handle_0 );
    dmm.Report( dmm_handle_1 );
}

void cli_Onshowtasks( EmbeddedCli *embedded_cli, char *args, void *context )
{
    cli_Taskvisible( logtask_show, args );
}

void cli_Onhidetasks( EmbeddedCli *embedded_cli, char *args, void *context )
{
    cli_Taskvisible( logtask_hide, args );
}

void cli_Oncleartasks( EmbeddedCli *embedded_cli, char *args, void *context )
{
    log_tasklist_t task_list;

    task_list.type = logtask_all;
    memset( task_list.handle, NULL, sizeof( task_list.handle ) );
    Log.SetListType( task_list );
}

void cli_Onmodemcommand( EmbeddedCli *embedded_cli, char *args, void *context )
{
    uint32_t parm_count = embeddedCliGetTokenCount( args );
    cli_obj.cli_active = false;
    slm.Command( args );
}

/**
 * Helper functions
 */

uint32_t cli_Getparms( char *args, int32_t *parms )
{
    uint32_t i;
    int32_t result = 0;

    for ( i = 0; i < embeddedCliGetTokenCount( args ) && result != EOF; i++ )
    {
        result = sscanf( embeddedCliGetToken( args, i + 1 ), "%d", &parms[ i ] );
    }

    return i;
}

void cli_Taskvisible( log_list_type_t type, char *args )
{
    bool flag = true;
    uint32_t i, j;
    TaskHandle_t handle[ MAX_TASKS ];

    log_tasklist_t task_list;

    cli_GetTaskList( handle );

    task_list.type = type;
    memset( task_list.handle, NULL, sizeof( task_list.handle ) );
    for ( i = 0; flag && i < embeddedCliGetTokenCount( args ); i++ )
    {
        flag = false;
        for ( j = 0; handle[ j ] != NULL && j < MAX_TASKS; j++ )
        {
            if ( strcmp( embeddedCliGetToken( args, i + 1 ), os.GetTaskName( handle[ j ] ) ) == 0 )
            {
                task_list.handle[ i ] = handle[ j ];
                flag = true;
            }
        }
        if ( flag == false )
        {
            memset( task_list.handle, NULL, sizeof( task_list.handle ) );
            Log.Print( "Invalid task: %s\r\n", embeddedCliGetToken( args, i + 1 ) );
        }
    }

    if ( flag )
    {
        Log.SetListType( task_list );
    }
}

void cli_GetTaskList( TaskHandle_t *handle )
{
    if ( handle != NULL )
    {
        twdt.List();
        if( os.QueueReceive( app.GetCliQHandle(), handle, os.Ms2Ticks( TWDT_KICK_TIME ) ) )
        {
            /* handle receives the task ID list which is passed back to caller */
        }
    }
}

void cli_Writechar( EmbeddedCli *embedded_cli, char c )
{
    Log.Putchar( c );
}

/*************************************************************************************************************************************
 * Public Functions Definition
 */

error_code_module_t cli_Init( void )
{
    uint16_t i;
    error_code_module_t error = NO_ERROR;

    /* Singleton pattern */
    if ( cli_obj.is_init == false )
    {
        /* Set up CLI task */
        shared_struct_t *shared_struct = ( shared_struct_t *)shared_mem;
        shared_struct->cli_queue = os.CreateQueue( QUEUE_LEN, MAX_TASKS * sizeof( TaskHandle_t ) );
        static StackType_t task_stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
        TaskHandle_t handle = os.CreateTask( cli_Thread,
                                             "CLI",
                                             task_stack,
                                             sizeof( task_stack ) / sizeof( StackType_t ),
                                             NULL,
                                             TASK_HIGH_PRIORITY | portPRIVILEGE_BIT );
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
        cli_obj.is_init = true;
    }
    else
    {
        error = ERROR_CLI_ALREADY_INIT;
    }

    return error;
}

void cli_Enable( bool state )
{
    cli_obj.cli_active = state;
}

EmbeddedCli* cli_Bindings( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer )
{
    uint32_t i;
    EmbeddedCli *embedded_cli;
    EmbeddedCliConfig *config = embeddedCliDefaultConfig();

    config->cliBuffer = buffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = nbindings;
    config->invitation = CLI_PROMPT;
    embedded_cli = embeddedCliNew( config );

    if ( embedded_cli != NULL )
    {
        for ( i = 0; i < nbindings; i++ )
        {
            embeddedCliAddBinding( embedded_cli, binding[i] );
        }
    }

    return embedded_cli;
}

/**
 * @} CLI
 */

/**
 * @} Application
 */
