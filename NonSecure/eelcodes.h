/**
 * @addtogroup Error Error codes
 * @{
 * @file      eelCodes.h
 * @brief     Error code definitions
 * @details   File describes all event and error codes.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

#ifndef  EEL_CODES_H__
#define  EEL_CODES_H__

typedef enum
{
    ERROR_EEL                       = (0x0100),    /*!< Module Error/event log. */
    ERROR_BOARD                     = (0x0200),    /*!< Module board. */
    ERROR_OS                        = (0x0300),    /*!< Module operating system. */
    // 0x0400
    ERROR_TMMGR                     = (0x0500),    /*!< Module timer. */
    // 0x0600
    // 0x0700
    // 0x0800
    // 0x0900
    // 0x0a00
    ERROR_MQTT                      = (0x0B00),    /*!< Module MQTT interface. */
    ERROR_FS                        = (0x0C00),    /*!< Module FS interface. */
    ERROR_SLM                       = (0x0D00),    /*!< Module SLM interface. */
    ERROR_PMR                       = (0x0E00),    /*!< Module power manager. */
    ERROR_SPIMTEST                  = (0x0F00),    /*!< Module serial periperal interface master test. */
    ERROR_SPISTEST                  = (0x1000),    /*!< Module serial periperal interface slave test. */
    ERROR_GPIO                      = (0x1100),    /*!< Module GPIO interface. */
    ERROR_CLI                       = (0x1200),    /*!< Module command line interface. */
    ERROR_LOG                       = (0x1300),    /*!< Module log interface. */
    ERROR_MODEM                     = (0x1400),    /*!< Module modem interface. */
    ERROR_APP                       = (0x1500),    /*!< Module application. */
    ERROR_TLS                       = (0x1600),    /*!< Module TLS interface. */
    // 0x1700
    ERROR_DMM                       = (0x1800),    /*!< Module dynamic memory manager. */
    ERROR_SPIM                      = (0x1900),    /*!< Module serial periperal interface master. */
    ERROR_SPIS                      = (0x1A00),    /*!< Module serial periperal interface slave. */
    ERROR_RTCD                      = (0x1B00),    /*!< Module internal RTC driver. */
    ERROR_CST                       = (0x1C00),    /*!< Module configuration storage. */
    ERROR_WDOG                      = (0x1D00),    /*!< Module watchdog monitor. */
    ERROR_UART                      = (0x1E00),    /*!< Module universal asynchronous receiver/transmitter. */
    ERROR_TWIM                      = (0x1F00),    /*!< Module two-wire interface driver. */
    ERROR_BLINKY                    = (0x2000),    /*!< Module Blinky. */
    ERROR_BATT                      = (0x2100),    /*!< Module BATT. */
    ERROR_FST                       = (0x2200),    /*!< Module firmware update storage. */
    ERROR_TEMP                      = (0x2300),    /*!< Module TEMP. */
    ERROR_UC                        = (0x2400),    /*!< Module UC User configuration. */
} error_code_modules_number_t ;

typedef enum
{
    NO_ERROR = 0,

    //--- ERROR_EEL -------------------------------------------------------------------------------
    ERROR_EEL_GENERAL               = (ERROR_EEL + 0x0000),
    ERROR_EEL_XY                    = (ERROR_EEL + 0x0001),

    //--- ERROR_BOARD -----------------------------------------------------------------------------
    ERROR_BOARD_ALREADY_INIT        = (ERROR_BOARD + 0x0002),

    //--- OS --------------------------------------------------------------------------------------
    ERROR_OS_GENERAL                = (ERROR_OS + 0x0000),
    ERROR_OS_ALREADY_INIT           = (ERROR_OS + 0x0001),
    ERROR_OS_INIT                   = (ERROR_OS + 0x0002),
    ERROR_OS_NOT_INIT               = (ERROR_OS + 0x0003),
    ERROR_OS_BAD_PARAM              = (ERROR_OS + 0x0004),
    ERROR_OS_EVENT_PROCESSING       = (ERROR_OS + 0x0005),

    //--- ERROR_TMMGR -----------------------------------------------------------------------------
    ERROR_TMMGR_GENERAL             = (ERROR_TMMGR + 0x0000),
    ERROR_TMMGR_NOT_INIT            = (ERROR_TMMGR + 0x0001),
    ERROR_TMMGR_ALREADY_INIT        = (ERROR_TMMGR + 0x0002),
    ERROR_TMMGR_MUTEX_NOT_CREATED   = (ERROR_TMMGR + 0x0003),
    ERROR_TMMGR_BAD_PARAM           = (ERROR_TMMGR + 0x0004),
    ERROR_TMMGR_TIMER_RUNNNING      = (ERROR_TMMGR + 0x0005),
    ERROR_TMMGR_TIMER_STOPPED       = (ERROR_TMMGR + 0x0006),
    ERROR_TMMGR_TOO_MANY_SIGNALS    = (ERROR_TMMGR + 0x0007),
    ERROR_TMMGR_NO_SIGNALS          = (ERROR_TMMGR + 0x0008),

    //--- MQTT ------------------------------------------------------------------------------------
    ERROR_MQTT_GENERAL              = (ERROR_MQTT + 0x0000),
    ERROR_MQTT_ALREADY_INIT         = (ERROR_MQTT + 0x0001),
    ERROR_MQTT_INIT                 = (ERROR_MQTT + 0x0002),
    ERROR_MQTT_NOT_INIT             = (ERROR_MQTT + 0x0003),
    ERROR_MQTT_BAD_PARAM            = (ERROR_MQTT + 0x0004),
    ERROR_MQTT_EVENT_PROCESSING     = (ERROR_MQTT + 0x0005),

    //--- FS --------------------------------------------------------------------------------------
    ERROR_FS_GENERAL                = (ERROR_FS + 0x0000),
    ERROR_FS_ALREADY_INIT           = (ERROR_FS + 0x0001),
    ERROR_FS_INIT                   = (ERROR_FS + 0x0002),
    ERROR_FS_NOT_INIT               = (ERROR_FS + 0x0003),
    ERROR_FS_BAD_PARAM              = (ERROR_FS + 0x0004),
    ERROR_FS_EVENT_PROCESSING       = (ERROR_FS + 0x0005),

    //--- SLM -------------------------------------------------------------------------------------
    ERROR_SLM_GENERAL               = (ERROR_SLM + 0x0000),
    ERROR_SLM_ALREADY_INIT          = (ERROR_SLM + 0x0001),
    ERROR_SLM_INIT                  = (ERROR_SLM + 0x0002),
    ERROR_SLM_NOT_INIT              = (ERROR_SLM + 0x0003),
    ERROR_SLM_BAD_PARAM             = (ERROR_SLM + 0x0004),
    ERROR_SLM_EVENT_PROCESSING      = (ERROR_SLM + 0x0005),

    //--- ERROR_PMR -------------------------------------------------------------------------------
    ERROR_PMR_GENERAL               = (ERROR_PMR + 0x0000),
    ERROR_PMR_ALREADY_INITIALIZED   = (ERROR_PMR + 0x0001),
    ERROR_PMR_NULL_POINTER          = (ERROR_PMR + 0x0002),
    ERROR_PMR_NOT_INITIALIZED       = (ERROR_PMR + 0x0003),
    ERROR_PMR_REGISTRATION_FINISHED = (ERROR_PMR + 0x0004),

    //--- ERROR_SPIMTEST --------------------------------------------------------------------------
    ERROR_SPIMTEST_GENERAL          = (ERROR_SPIM + 0x0000),
    ERROR_SPIMTEST_INIT             = (ERROR_SPIM + 0x0001),
    ERROR_SPIMTEST_NOT_INIT         = (ERROR_SPIM + 0x0002),
    ERROR_SPIMTEST_ALREADY_INIT     = (ERROR_SPIM + 0x0003),
    ERROR_SPIMTEST_NULL_POINTER     = (ERROR_SPIM + 0x0004),
    ERROR_SPIMTEST_WRITE            = (ERROR_SPIM + 0x0005),
    ERROR_SPIMTEST_READ             = (ERROR_SPIM + 0x0006),

    //--- ERROR_SPISTEST --------------------------------------------------------------------------
    ERROR_SPISTEST_GENERAL          = (ERROR_SPIS + 0x0000),
    ERROR_SPISTEST_INIT             = (ERROR_SPIS + 0x0001),
    ERROR_SPISTEST_NOT_INIT         = (ERROR_SPIS + 0x0002),
    ERROR_SPISTEST_ALREADY_INIT     = (ERROR_SPIS + 0x0003),
    ERROR_SPISTEST_NULL_POINTER     = (ERROR_SPIS + 0x0004),
    ERROR_SPISTEST_WRITE            = (ERROR_SPIS + 0x0005),
    ERROR_SPISTEST_READ             = (ERROR_SPIS + 0x0006),

    //--- UI --------------------------------------------------------------------------------------
    ERROR_GPIO_GENERAL              = (ERROR_GPIO + 0x0000),
    ERROR_GPIO_ALREADY_INIT         = (ERROR_GPIO + 0x0001),
    ERROR_GPIO_INIT                 = (ERROR_GPIO + 0x0002),
    ERROR_GPIO_NOT_INIT             = (ERROR_GPIO + 0x0003),
    ERROR_GPIO_BAD_PARAM            = (ERROR_GPIO + 0x0004),
    ERROR_GPIO_EVENT_PROCESSING     = (ERROR_GPIO + 0x0005),

    //--- CLI -------------------------------------------------------------------------------------
    ERROR_CLI_GENERAL               = (ERROR_CLI + 0x0000),
    ERROR_CLI_ALREADY_INIT          = (ERROR_CLI + 0x0001),
    ERROR_CLI_INIT                  = (ERROR_CLI + 0x0002),
    ERROR_CLI_NOT_INIT              = (ERROR_CLI + 0x0003),
    ERROR_CLI_BAD_PARAM             = (ERROR_CLI + 0x0004),
    ERROR_CLI_EVENT_PROCESSING      = (ERROR_CLI + 0x0005),

    //--- LOG -------------------------------------------------------------------------------------
    ERROR_LOG_GENERAL               = (ERROR_LOG + 0x0000),
    ERROR_LOG_ALREADY_INIT          = (ERROR_LOG + 0x0001),
    ERROR_LOG_INIT                  = (ERROR_LOG + 0x0002),
    ERROR_LOG_NOT_INIT              = (ERROR_LOG + 0x0003),
    ERROR_LOG_BAD_PARAM             = (ERROR_LOG + 0x0004),
    ERROR_LOG_EVENT_PROCESSING      = (ERROR_LOG + 0x0005),

    //--- MODEM -----------------------------------------------------------------------------------
    ERROR_MODEM_GENERAL             = (ERROR_MODEM + 0x0000),
    ERROR_MODEM_ALREADY_INIT        = (ERROR_MODEM + 0x0001),
    ERROR_MODEM_INIT                = (ERROR_MODEM + 0x0002),
    ERROR_MODEM_NOT_INIT            = (ERROR_MODEM + 0x0003),
    ERROR_MODEM_BAD_PARAM           = (ERROR_MODEM + 0x0004),
    ERROR_MODEM_EVENT_PROCESSING    = (ERROR_MODEM + 0x0005),

    //--- APPLICATION -----------------------------------------------------------------------------
    ERROR_APP_INIT                  = (ERROR_APP + 0x0000),
    ERROR_APP_ALREADY_INIT          = (ERROR_APP + 0x0001),
    ERROR_APP_NOT_INIT              = (ERROR_APP + 0x0002),
    ERROR_APP_NULL_POINTER          = (ERROR_APP + 0x0003),
    ERROR_APP_NOT_SUPPORTED         = (ERROR_APP + 0x0004),
    ERROR_APP_SHUTDOWN_FAILED       = (ERROR_APP + 0x0005),
    ERROR_APP_SELF_TEST_FAILED      = (ERROR_APP + 0x0006),
    ERROR_APP_NO_RESOURCES          = (ERROR_APP + 0x0007),

    //--- TLS-- -----------------------------------------------------------------------------------
    ERROR_TLS_GENERAL               = (ERROR_TLS + 0x0000),
    ERROR_TLS_ALREADY_INIT          = (ERROR_TLS + 0x0001),
    ERROR_TLS_INIT                  = (ERROR_TLS + 0x0002),
    ERROR_TLS_NOT_INIT              = (ERROR_TLS + 0x0003),
    ERROR_TLS_BAD_PARAM             = (ERROR_TLS + 0x0004),
    ERROR_TLS_EVENT_PROCESSING      = (ERROR_TLS + 0x0005),

    //--- ERROR_DMM -------------------------------------------------------------------------------
    ERROR_DMM_GENERAL               = (ERROR_DMM + 0x0000),
    ERROR_DMM_INIT                  = (ERROR_DMM + 0x0001),
    ERROR_DMM_NOT_INIT              = (ERROR_DMM + 0x0002),
    ERROR_DMM_ALREADY_INIT          = (ERROR_DMM + 0x0003),
    ERROR_DMM_NULL_POINTER          = (ERROR_DMM + 0x0004),
    ERROR_DMM_WRITE                 = (ERROR_DMM + 0x0005),
    ERROR_DMM_READ                  = (ERROR_DMM + 0x0006),

    //--- ERROR_SPIM ------------------------------------------------------------------------------
    ERROR_SPIM_GENERAL              = (ERROR_SPIM + 0x0000),
    ERROR_SPIM_INIT                 = (ERROR_SPIM + 0x0001),
    ERROR_SPIM_NOT_INIT             = (ERROR_SPIM + 0x0002),
    ERROR_SPIM_ALREADY_INIT         = (ERROR_SPIM + 0x0003),
    ERROR_SPIM_NULL_POINTER         = (ERROR_SPIM + 0x0004),
    ERROR_SPIM_WRITE                = (ERROR_SPIM + 0x0005),
    ERROR_SPIM_READ                 = (ERROR_SPIM + 0x0006),

    //--- ERROR_SPIM ------------------------------------------------------------------------------
    ERROR_SPIS_GENERAL              = (ERROR_SPIS + 0x0000),
    ERROR_SPIS_INIT                 = (ERROR_SPIS + 0x0001),
    ERROR_SPIS_NOT_INIT             = (ERROR_SPIS + 0x0002),
    ERROR_SPIS_ALREADY_INIT         = (ERROR_SPIS + 0x0003),
    ERROR_SPIS_NULL_POINTER         = (ERROR_SPIS + 0x0004),
    ERROR_SPIS_WRITE                = (ERROR_SPIS + 0x0005),
    ERROR_SPIS_READ                 = (ERROR_SPIS + 0x0006),

    //--- ERROR_RTCD ------------------------------------------------------------------------------
    ERROR_RTCD_GENERAL              = (ERROR_RTCD + 0x0000),
    ERROR_RTCD_ALREADY_INIT         = (ERROR_RTCD + 0x0001),
    ERROR_RTCD_INIT                 = (ERROR_RTCD + 0x0002),
    ERROR_RTCD_TIMEOUT              = (ERROR_RTCD + 0x0003),
    ERROR_RTCD_BAD_PARAM            = (ERROR_RTCD + 0x0004),
    ERROR_RTCD_NOT_INIT             = (ERROR_RTCD + 0x0005),

    //--- ERROR_CST -------------------------------------------------------------------------------
    ERROR_CST_GENERAL               = (ERROR_CST + 0x0000),
    ERROR_CST_BAD_PARAM             = (ERROR_CST + 0x0001),
    ERROR_CST_NOT_INIT              = (ERROR_CST + 0x0002),
    ERROR_CST_ALREADY_INIT          = (ERROR_CST + 0x0003),
    ERROR_CST_ALLOC_FAILED          = (ERROR_CST + 0x0004),
    // Leave these as individual bits
    ERROR_CST_NO_STORAGE_HEADER     = (ERROR_CST + 0x0010),
    ERROR_CST_NO_ATTR_AND_SETTINGS  = (ERROR_CST + 0x0020),
    ERROR_CST_NO_FIRST_BEAT         = (ERROR_CST + 0x0040),
    ERROR_CST_NO_SECTOR_MAP         = (ERROR_CST + 0x0080),

    //--- ERROR_WDOG ------------------------------------------------------------------------------
    ERROR_WDOG_GENERAL              = (ERROR_WDOG + 0x0000),
    ERROR_WDOG_ALREADY_INIT         = (ERROR_WDOG + 0x0001),
    ERROR_WDOG_NOT_INIT             = (ERROR_WDOG + 0x0002),
    ERROR_WDOG_INIT                 = (ERROR_WDOG + 0x0003),
    ERROR_WDOG_TOO_MANY_CLIENTS     = (ERROR_WDOG + 0x0004),
    ERROR_WDOG_BAD_PARAM            = (ERROR_WDOG + 0x0005),

    //--- ERROR_UART ------------------------------------------------------------------------------
    ERROR_UART_GENERAL              = (ERROR_UART + 0x0000),
    ERROR_UART_INIT                 = (ERROR_UART + 0x0001),
    ERROR_UART_NOT_INIT             = (ERROR_UART + 0x0002),
    ERROR_UART_ALREADY_INIT         = (ERROR_UART + 0x0003),
    ERROR_UART_NULL_POINTER         = (ERROR_UART + 0x0004),
    ERROR_UART_WRITE                = (ERROR_UART + 0x0005),
    ERROR_UART_READ                 = (ERROR_UART + 0x0006),

    //--- ERROR_I2CD ------------------------------------------------------------------------------
    ERROR_TWIM_GENERAL              = (ERROR_TWIM + 0x0000),
    ERROR_TWIM_INIT                 = (ERROR_TWIM + 0x0001),
    ERROR_TWIM_NOT_INIT             = (ERROR_TWIM + 0x0002),
    ERROR_TWIM_ALREADY_INIT         = (ERROR_TWIM + 0x0003),
    ERROR_TWIM_NULL_POINTER         = (ERROR_TWIM + 0x0004),
    ERROR_TWIM_WRITE                = (ERROR_TWIM + 0x0005),
    ERROR_TWIM_READ                 = (ERROR_TWIM + 0x0006),
    ERROR_TWIM_MUTEX                = (ERROR_TWIM + 0x0007),
    ERROR_TWIM_WRITE_AND_MUTEX      = (ERROR_TWIM + 0x0008),
    ERROR_TWIM_READ_AND_MUTEX       = (ERROR_TWIM + 0x0009),

    //--- ERROR_BATT ------------------------------------------------------------------------------
    ERROR_BATT_GENERAL              = (ERROR_BATT + 0x0000),
    ERROR_BATT_INIT                 = (ERROR_BATT + 0x0001),
    ERROR_BATT_NOT_INIT             = (ERROR_BATT + 0x0002),
    ERROR_BATT_ALREADY_INIT         = (ERROR_BATT + 0x0003),
    ERROR_BATT_NULL_POINTER         = (ERROR_BATT + 0x0004),
    ERROR_BATT_INVALID_PARAM        = (ERROR_BATT + 0x0005),
    ERROR_BATT_SELF_TEST            = (ERROR_BATT + 0x0006),

    //--- ERROR_BLINKY ----------------------------------------------------------------------------
    ERROR_BLINKY_GENERAL            = (ERROR_BLINKY + 0x0000),
    ERROR_BLINKY_INIT               = (ERROR_BLINKY + 0x0001),
    ERROR_BLINKY_NOT_INIT           = (ERROR_BLINKY + 0x0002),
    ERROR_BLINKY_ALREADY_INIT       = (ERROR_BLINKY + 0x0003),

    //--- ERROR_FST -------------------------------------------------------------------------------
    ERROR_FST_GENERAL               = (ERROR_FST + 0x0000),
    ERROR_FST_BAD_PARAM             = (ERROR_FST + 0x0001),
    ERROR_FST_NOT_INIT              = (ERROR_FST + 0x0002),
    ERROR_FST_ALREADY_INIT          = (ERROR_FST + 0x0003),
    ERROR_FST_BAD_STATE             = (ERROR_FST + 0x0004),
    ERROR_FST_TRANSFER              = (ERROR_FST + 0x0005),
    ERROR_FST_BAD_CRC               = (ERROR_FST + 0x0006),
    ERROR_FST_BAD_HEADER            = (ERROR_FST + 0x0007),
    ERROR_FST_BAD_VERSION_MAIN      = (ERROR_FST + 0x0008),
    ERROR_FST_BAD_VERSION_BLE       = (ERROR_FST + 0x0009),
    ERROR_FST_BAD_KEY_ID            = (ERROR_FST + 0x000A),
    ERROR_FST_NOT_SUPPORTED         = (ERROR_FST + 0x000B),

    //--- ERROR_TEMP ------------------------------------------------------------------------------
    ERROR_TEMP_GENERAL              = (ERROR_TEMP + 0x0000),
    ERROR_TEMP_INIT                 = (ERROR_TEMP + 0x0001),
    ERROR_TEMP_NOT_INIT             = (ERROR_TEMP + 0x0002),
    ERROR_TEMP_ALREADY_INIT         = (ERROR_TEMP + 0x0003),
    ERROR_TEMP_NULL_POINTER         = (ERROR_TEMP + 0x0004),
    ERROR_TEMP_SELF_TEST_FAILED     = (ERROR_TEMP + 0x0005),

    //--- ERROR_UC --------------------------------------------------------------------------------
    EEROR_UC_GENERAL                = (ERROR_UC + 0x0000),
    ERROR_UC_INIT                   = (ERROR_UC + 0x0001),
    EEROR_UC_NOT_INIT               = (ERROR_UC + 0x0002),
    EEROR_UC_ALREADY_INIT           = (ERROR_UC + 0x0003),
    EEROR_UC_NULL_POINTER           = (ERROR_UC + 0x0004),
    EEROR_UC_FLASH_WRITE            = (ERROR_UC + 0x0005),

} error_code_module_t ;

#endif //eelCodes_H__
/**
 @} group
*/
