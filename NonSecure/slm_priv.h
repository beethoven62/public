/** @file slm_priv.h
 *
 * @brief       Modem module (private header).
 * @author      Johnas Cukier
 * @date        January 2023
 *
 */

/**
 * @addtogroup modem
 * @{
 */

#ifndef __SLM_PRIV_H__
#define __SLM_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "slm.h"

/***************************************************************************************************************************
 * Private constants and macros
 */
// embedded-cli definitions
#define EMBEDDED_CLI_IMPL
#define SLM_RX_BUFFER_SIZE      ( 64 )
#define SLM_CMD_BUFFER_SIZE     ( 64 )
#define SLM_HISTORY_SIZE        ( 32 )
#define SLM_PROMPT              "Modem -> "

/***************************************************************************************************************************
 * Private data structures and typedefs
 */

typedef int32_t ( *slm_handler_t ) ( char *parms );

typedef struct
{
    TaskHandle_t handle;
    char msg[ SHORT_MSG_MAX ];
} slm_msg_t;

typedef struct
{
    char *command;
    slm_handler_t handler;
} atx_cmd_t;

typedef struct
{
    bool                        is_init;
    bool                        slm_active;
    log_level_t                 log_level;
    int32_t                     socket;
} slm_obj_t;

/***************************************************************************************************************************
 * Private variables
 */

/***************************************************************************************************************************
 * Private prototypes (interface functions)
 */

/**
 * @brief       Initialize the module.
 * @param[out]  did_reset_system    (Optional) Indicates whether the system was reset by Watchdog.
 * @return      Error code.
 */
static error_code_module_t slm_Init( void );

/**
 * @brief       Send modem command.
 * @param[out]  did_reset_system    (Optional) Indicates whether the system was reset by Watchdog.
 */
static void slm_Command( char *args );

/**
 * @brief       Setup SLM bindings.
 * @details     See Embedded CLI documentation on how to bind commands to the CLI.
 * @param[in]   binding         Pointer to array of CLI command bindings
 * @param[in]   nbindings       Number of bindings
 * @param[in]   buffer          CLI buffer
 * @return      Embedded CLI binding structure.
 */
static EmbeddedCli* slm_Bindings( CliCommandBinding *binding, size_t nbindings, CLI_UINT *buffer );

/**
 * @brief       Embedded CLI wrapper for putchar() function.
 * @param[in]   embedded_cli    pointer to Embedded CLI structure
 * @param[in]   c               character to print
 */
static void slm_Writechar(EmbeddedCli *embedded_cli, char c);

/***************************************************************************************************************************
 * Private prototypes
 */
/**
 * @brief       SLM thread function. Executes and responds to requests.
 * @param[in]   parameter_ptr    Initial parameters passed into thread at start of thread.
 */
static void slm_Thread( void * parameter_ptr );

/**
 * @brief Implements the connection to server TCP/UDP over PDN transport connection.
 *
 * @details API to create as socket and ultimately a TCP/UDP connection using the DNS/
 *          IP address and port of the server.
 *
 * @param pParameters[in]: input string to set up socket as below:
 *          parm[0]: context ID used for the PDN connection - not used
 *          parm[1]: connect ID socket handle
 *          parm[2]: service type (TCP, UDP)
 *          parm[3]: DNS/IP address of server
 *          parm[4]: remote port
 *          parm[5]: local port
 *          parm[6]: cellular data access mode (buffer, direct push, or transparent) - not used
 *
 * @return a zero on success
 *
 */
static void slm_Connect( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Implements the disconnection from remote server and closing of socket.
 *
 * @details API to create as socket and ultimately a TCP/UDP connection using the DNS/
 *          IP address and port of the server.
 *
 * @param pParameters[in]: input string to set up socket as below:
 *          parm[0]: connect ID socket handle
 *
 * @return a zero on success
 *
 */
static void slm_Disconnect( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Reports the state of the modem.
 * @param[in]   embedded_cli    CLI data structure (not used)
 * @param[in]   args            CLI argument list (not used)
 * @param[in]   contest         User specific data (not used)
 */
static void slm_Status( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Dump contents of all credentials in the modem.
 * @param[in]   embedded_cli    CLI data structure (not used)
 * @param[in]   args            CLI argument list (not used)
 * @param[in]   contest         User specific data (not used)
 */
static void slm_Credentials( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Starts the modem.
 * @param[in]   embedded_cli    CLI data structure (not used)
 * @param[in]   args            CLI argument list (not used)
 * @param[in]   contest         User specific data (not used)
 */
static void slm_Start( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Stops the modem.
 * @param[in]   embedded_cli    CLI data structure (not used)
 * @param[in]   args            CLI argument list (not used)
 * @param[in]   contest         User specific data (not used)
 */
static void slm_Stop( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Initiate an NTP request
 *
 * @param[in]   ntp_server     Server name
 */
static void slm_Ntpreq( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Initiate an HTTP request
 *
 * @param[in]   http_server     Server name
 */
static void slm_Httpreq( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Initiate an HTTPS (TLS) request
 *
 * @param pParameters[in]: input string to set up HTTPS request as below:
 *          parm[0]: HTTPS Request type
 *          parm[1]: Path and Query
 *          parm[2]: Hostname
 *          parm[3]: HTTPS port
 *
 * @return a zero on success
 *
 *
 * @sample usage:
 *          AT#XHTTPSREQ=GET,/,www.google.com,443
 *          AT#XHTTPSREQ=GET,/index.html,www.example.com,443
 */
static void slm_Httpsreq( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief Initiate an MQTT (mTLS) request
 *
 * @param pParameters[in]: input string to set up HTTPS request as below:
 *          parm[0]: MQTT Request type
 *          parm[1]: Path and Query
 *          parm[2]: Hostname
 *          parm[3]: MQTT port
 *
 * @return a zero on success
 *
 *
 */
static void slm_Mqttreq( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Command line catch all.
 * @param[in]   embedded_cli    embbedded CLI structure
 * @param[in]   command         command line string
 */
static void slm_Dummy( EmbeddedCli *embedded_cli, CliCommand *command );

/**
 * @brief       Exit SLM subcommand mode.
 * @param[in]   embedded_cli    embbedded CLI structure
 * @param[in]   command         command line string
 */
static void slm_Exit( EmbeddedCli *embedded_cli, char *args, void *context );

/**
 * @brief       Process data received from cloud.
 * @param[in]   data            pointer to data block
 * @param[in]   length          data block length
 */
static void slm_Process( uint8_t *data, uint32_t length );

/**
 * @brief       HTTP/HTTPS get request.
 * @param[in]   transport_protocol  protocol type (tcp, tls)
 * @param[in]   host_name           server name
 * @param[in]   port                server port number
 */
static void slm_GetRequest( const char *transport_protocol, const char *host_name, uint16_t port );

#endif /* __SLM_PRIV_H__ */

/**
 * @}
 */
