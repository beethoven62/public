/**
 * @file      cli_priv.h
 *
 * @brief     Command line interface module (private header).
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @addtogroup CLI
 * @{
 */

#ifndef __CLI_PRIV_H__
#define __CLI_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "log.h"
#include "twdt.h"
#include "cli.h"

/***************************************************************************************************************************
 * Private constants and macros
 */

// embedded-cli definitions
#define EMBEDDED_CLI_IMPL
#define CLI_RX_BUFFER_SIZE      ( 64 )
#define CLI_CMD_BUFFER_SIZE     ( 64 )
#define CLI_HISTORY_SIZE        ( 32 )
#define CLI_PROMPT              "nRF91 -> "

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef struct
{
    bool                        is_init;
    log_level_t                 log_level;
    bool                        cli_active;
} cli_obj_t ;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @return      Error code.
 */
static error_code_module_t cli_Init( void );

/**
 * @brief       Setup CLI bindings.
 * @details     See Embedded CLI documentation on how to bind commands to the CLI.
 * @param[in]   binding         Pointer to array of CLI command bindings
 * @param[in]   nbindings       Number of bindings
 * @param[in]   buffer          CLI buffer
 * @return      Embedded CLI binding structure.
 */
static EmbeddedCli* cli_Bindings( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer );


/**
 * @brief       Embedded CLI wrapper for putchar() function.
 * @param[in]   embedded_cli    pointer to Embedded CLI structure
 * @param[in]   c               character to print
 */
static void cli_Writechar(EmbeddedCli *embedded_cli, char c);

/**
 * @brief       Set CLI active state.
 * @param[in]   state           true = active, false = inactive
 */
static void cli_Enable( bool state );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       Command line interface thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static void cli_Thread( void * parameter_ptr );

/**
 * @brief       Get parameters from args.
 * @param[in]   args    arguments string
 * @param[out]  parms   argument array
 * @return      number of parameters retrieved or EOF
 */
static uint32_t cli_Getparms( char *args, int32_t *parms );

/**
 * @brief       Get task list.
 * @details     Retrieves task ID list from the Watchdog Task
 * @param[out]  handle          array of handlers provided by caller
 *                              Warning: this parameter must provide enough memory
 *                                       for task ID list
 */
static void cli_GetTaskList( TaskHandle_t *handle );

/**
 * @brief       Task list visibility helper function.
 * @details     Helper function for show/hide functions that print out logs
 *              based on task ID.
 * @param[in]   type            log type (show or hide)
 * @param[in]   args            list of tasks (names of tasks)
 */
static void cli_Taskvisible( log_list_type_t type, char *args );

/**
 * @brief       Command line catch all.
 * @param[in]   embedded_cli    embbedded CLI structure
 * @param[in]   command         command line string
 * @return      Error code
 */
static void cli_Oncommand( EmbeddedCli *embedded_cli, CliCommand *command );

/**
 * @brief       Set LED parameters.
 * @details     set-led t n (t = LED time (in ms) on/off, n = number of LEDs)
 * @param[in]   args    arguments string
 */
static void cli_Onled( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Set log level.
 * @details     set-mode x (x = 0, 1, 2, 3, or 4)
 *                              0 = no log
 *                              1 = error
 *                              2 = info
 *                              3 = debug
 *                              4 = insane
 * @param[in]   args    arguments string
 */
static void cli_Onsetlevel( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Get log level.
 * @details     set-mode x (x = 0, 1, 2, 3, or 4)
 *                              0 = no log
 *                              1 = error
 *                              2 = info
 *                              3 = debug
 *                              4 = insane
 */
static void cli_Ongetlevel( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Get task list.
 */
static void cli_Ongetstatus( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Show tasks in list only (limited to three tasks).
 * @details     show-tasks task1 task2 task3
 *                              taskN = name of task to show
 * @param[in]   args    arguments string
 */
static void cli_Onshowtasks( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Hide tasks in list only (limited to three tasks).
 * @details     hide-tasks task1 task2 task3
 *                              taskN = name of task to hide
 * @param[in]   args    arguments string
 */
static void cli_Onhidetasks( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Clear task list.
 * @details     clear-tasks
 */
static void cli_Oncleartasks( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Send modem command.
 * @details     send command (not AT) to modem
 *
 * @param[in]   args    argument string
 * @param[in]   args    argument string
 */
static void cli_Onmodemcommand( EmbeddedCli *embedded_cli, char *args, void *context );
#endif /* __CLI_PRIV_H__ */

/**
 * @}
 */
