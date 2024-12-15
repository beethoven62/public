/**
 * @mainpage FreeRTOS Connectivity
 * @section intro_sec Introduction
 *      This is an introduction to the code architecture and ideology
 * @section task_arch Task Architecture
 *     1. All tasks must not exit unless a "kill" event is received. There may be an
 *        exception under special circumstances (one shot task).
 *     2. All tasks must register with the task watchdog for proper operation of the
 *        system.
 *     3. Drivers provide a wrapper for a peripheral and will own the peripheral.
 *        All tasks accessing that peripheral should work cooperatively for the resource.
 *        This can be done through a single task or through a semaphore.
 *     4. The Memory Protection Unit (MPU) is enabled. Therefore, all memory access
 *        outside of a task's local memory is restricted. Permission is required to
 *        access memory outside of the task via the task start up code. A shared
 *        memory block has been set up to aggregate all memory across tasks in
 *        compliance with the MPU restrictions.
 *     5. Events should be used for a one-to-many type of communication as a signal.
 *        No data should be transferred in an event.
 *
 * @section build_instructions Instructions to Build and Run:
 *     - The Keil multi-project workspace FreeRTOSDemo.uvmpw contains projects for
 *       both the secure project, and non secure project.
 *     - Set the FreeRTOSDemo_s project as Active - Right click on
 *       "Project: FreeRTOSDemo_s" and select "Set as Active Project".
 *     - Build the FreeRTOSDemo_s project using "Project --> Build" or by pressing
 *       "F7".
 *     - Set the FreeRTOSDemo_ns project as Active - Right click on
 *       "Project: FreeRTOSDemo_ns" and select "Set as Active Project".
 *     - Build the FreeRTOSDemo_ns project using "Project --> Build" or by
 *       pressing "F7".
 *     - Start Debug Session using "Debug -> Start/Stop Debug Session" or by
 *       pressing "Ctrl+F5".
 *
 *    Compiler defines:
 *     - ARM Cortex M33 secure domain:                      NRF_TRUSTZONE_NONSECURE
 *     - ???:                                               __SUPPORT_RESET_HALT_AFTER_BTL=0
 *     - Use ARM floating-point coprocessor:                ARM_MATH_ARMV8MML
 *     - Device is Nordic nRF9160:                          NRF9160_XXAA
 *     - Initialize user RAM:                               __NRF_FAMILY INITIALIZE_USER_SECTIONS
 *     - Share peripheral resources:                        NRFX_PRS_ENABLED
 *     - Enable UARTs:                                      NRFX_UARTE_ENABLED
 *     - Enable UARTx (x = 0, 1, 2):                        NRFX_UARTE1_ENABLED
 *                                                          NRFX_UARTE2_ENABLED
 *     - Enable SPI masters:                                NRFX_SPIM_ENABLED
 *     - Enable SPI master x (x = 0, 1, 2, 3):              NRFX_SPIM0_ENABLED
 *     - Redirect SPIx (x = 0, 1, 2, 3) interrupt handler:  NRF_SPIM0=NRF_NSPIM0
 *     - Enable SPI slaves:                                 NRFX_SPIS_ENABLED
 *     - Enable SPI slave x (x = 0, 1, 2, 3):               NRFX_SPIS0_ENABLED
 *     - Redirect SPIx (x = 0, 1, 2, 3) interrupt handler:  NRF_SPIS0=NRF_NSPIS0
 *     - Enable two-wire master interfaces (I2C):           NRFX_TWIM_ENABLED
 *     - Enable I2Cx master (x = 0, 1, 2, 3):               NRFX_TWIM0_ENABLED
 *     - Enable two-wire slave interfaces (I2C):            NRFX_TWIS_ENABLED
 *     - Enable I2Cx slave (x = 0, 1, 2, 3):                NRFX_TWIS0_ENABLED
 *     - Enable inter-processor communications:             NRFX_IPC_ENABLED
 *     - Enable modem trace logs:                           CONFIG_NRF_MODEM_LIB_TRACE_ENABLED=0
 *     - Enable non-volatile memory controller (flash)      NRFX_NVMC_ENABLED
 *     - LittleFS (file system) uses static memory          LFS_NO_MALLOC
 *     - Target board:                                      TARGET_DEVICE_NRF9160DK
 *          - options:
 *              - nRF9160DK dev board:                        TARGET_DEVICE_NRF9160DK
 *              - Thingy:91 dev board:                        TARGET_DEVICE_THINGY91
 *              - Everion+ board:                             TARGET_DEVICE_EVERION_PLUS
 *     - Boot up using log level                            INIT_LOG_LEVEL=loglevel_info
 */

/**
 * @addtogroup Application Application
 * @{
 * @file      main_ns.c
 * @brief     Non-secure main module.
 * @author    Johnas Cukier
 * @date      Jan 2023
 */

/**
 * @defgroup main Main function (non-secure)
 * @brief    Main (starting point)
 * @{
 */

/*************************************************************************************************************************************
 * Includes
*/
#include <stdio.h>
#include <string.h>
#include "os.h"
#include "app.h"

/***************************************************************************************************************************
 * Global variables
 */

#if BUILD_TOOL_SES           // Segger Embedded Studio configuration
/* Initialize the MPU symbols needed by the port code. */
extern uint32_t __PRIVILEGED_FLASH_NS_segment_start__[];
extern uint32_t __PRIVILEGED_FLASH_NS_segment_end__[];
extern uint32_t __SYSCALLS_FLASH_NS_segment_start__[];
extern uint32_t __SYSCALLS_FLASH_NS_segment_end__[];
extern uint32_t __UNPRIVILEGED_FLASH_NS_segment_start__[];
extern uint32_t __UNPRIVILEGED_FLASH_NS_segment_end__[];
extern uint32_t __PRIVILEGED_RAM_NS_segment_start__[];
extern uint32_t __PRIVILEGED_RAM_NS_segment_end__[];

uint32_t * __privileged_functions_start__   = __PRIVILEGED_FLASH_NS_segment_start__;
uint32_t * __privileged_functions_end__     = ( uint32_t * )( ( uint32_t )__PRIVILEGED_FLASH_NS_segment_end__ - ( uint32_t ) 1 );
uint32_t * __syscalls_flash_start__         = __SYSCALLS_FLASH_NS_segment_start__;
uint32_t * __syscalls_flash_end__           = ( uint32_t * )( ( uint32_t )__SYSCALLS_FLASH_NS_segment_end__ - ( uint32_t ) 1 );
uint32_t * __unprivileged_flash_start__     = __UNPRIVILEGED_FLASH_NS_segment_start__;
uint32_t * __unprivileged_flash_end__       = ( uint32_t * )( ( uint32_t )__UNPRIVILEGED_FLASH_NS_segment_end__ - ( uint32_t ) 1 );
uint32_t * __privileged_sram_start__        = __PRIVILEGED_RAM_NS_segment_start__;
uint32_t * __privileged_sram_end__          = ( uint32_t * )( ( uint32_t )__PRIVILEGED_RAM_NS_segment_end__ - ( uint32_t ) 1 );

#else           // Keil configuration
/* Externs needed by the MPU setup code. These are defined in Scatter-Loading
 * description file (FreeRTOSDemo_ns.sct).
 */
extern uint32_t Image$$ER_IROM_NS_PRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_NS_PRIVILEGED_ALIGN$$Limit;
extern uint32_t Image$$ER_IROM_NS_FREERTOS_SYSTEM_CALLS$$Base;
extern uint32_t Image$$ER_IROM_NS_FREERTOS_SYSTEM_CALLS_ALIGN$$Limit;
extern uint32_t Image$$ER_IROM_NS_UNPRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_NS_UNPRIVILEGED_ALIGN$$Limit;

extern uint32_t Image$$ER_IRAM_NS_PRIVILEGED$$Base;
extern uint32_t Image$$ER_IRAM_NS_PRIVILEGED_ALIGN$$Limit;
extern uint32_t Image$$ER_IRAM_NS_UNPRIVILEGED$$Base;
extern uint32_t Image$$ER_IRAM_NS_UNPRIVILEGED_ALIGN$$Limit;

/* Privileged flash. */
const uint32_t * __privileged_functions_start__    = ( uint32_t * ) &( Image$$ER_IROM_NS_PRIVILEGED$$Base );
const uint32_t * __privileged_functions_end__    = ( uint32_t * ) ( ( uint32_t ) &( Image$$ER_IROM_NS_PRIVILEGED_ALIGN$$Limit ) - 0x1 ); /* Last address in privileged Flash region. */

/* Flash containing system calls. */
const uint32_t * __syscalls_flash_start__        = ( uint32_t * ) &( Image$$ER_IROM_NS_FREERTOS_SYSTEM_CALLS$$Base );
const uint32_t * __syscalls_flash_end__        = ( uint32_t * ) ( ( uint32_t ) &( Image$$ER_IROM_NS_FREERTOS_SYSTEM_CALLS_ALIGN$$Limit ) - 0x1 ); /* Last address in Flash region containing system calls. */

/* Unprivileged flash. Note that the section containing system calls is
 * unprivileged so that unprivileged tasks can make system calls. */
const uint32_t * __unprivileged_flash_start__    = ( uint32_t * ) &( Image$$ER_IROM_NS_UNPRIVILEGED$$Base );
const uint32_t * __unprivileged_flash_end__        = ( uint32_t * ) ( ( uint32_t ) &( Image$$ER_IROM_NS_UNPRIVILEGED_ALIGN$$Limit ) - 0x1 ); /* Last address in un-privileged Flash region. */

/* RAM with priviledged access only. This contains kernel data. */
const uint32_t * __privileged_sram_start__        = ( uint32_t * ) &( Image$$ER_IRAM_NS_PRIVILEGED$$Base );
const uint32_t * __privileged_sram_end__        = ( uint32_t * ) ( ( uint32_t ) &( Image$$ER_IRAM_NS_PRIVILEGED_ALIGN$$Limit ) - 0x1 ); /* Last address in privileged RAM. */

/* Unprivileged RAM. */
const uint32_t * __unprivileged_sram_start__    = ( uint32_t * ) &( Image$$ER_IRAM_NS_UNPRIVILEGED$$Base );
const uint32_t * __unprivileged_sram_end__        = ( uint32_t * ) ( ( uint32_t ) &( Image$$ER_IRAM_NS_UNPRIVILEGED_ALIGN$$Limit ) - 0x1 ); /* Last address in un-privileged RAM. */
#endif

/*************************************************************************************************************************************
 * Public Functions Definition
 */

/**
 * @brief The memory manager fault handler.
 *
 * It calls a function called vHandleMemoryFault.
 */
void MemManage_Handler( void ) __attribute__ ( ( naked ) );

/**
 * @brief Non-Secure main.
 */
int main( void )
{
#if SYSVIEW_ENABLED
    /* Start Segger System View Analyzer */
    traceSTART();
#endif

    /* Start application task */
    app.Init();

    /* Start scheduler. */
    os.Init();

    /* Should not reach here as the scheduler is already started. */
    for( ; ; )
    {
    }
}

/**
 * FreeRTOS functions
 *
 */

#if BUILD_TOOL_SES
/**
 * @brief Initializes the privileged_data section.
 *
 * Called from the startup code.
 */
void InitializeUserMemorySections( void )
{
    extern uint8_t __privileged_data_load_start__[];
    extern uint8_t __privileged_data_start__[];
    extern uint8_t __privileged_data_end__[];

    memcpy( __privileged_data_start__,
            __privileged_data_load_start__,
            __privileged_data_end__ - __privileged_data_start__ );
}
#endif

/**
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    /* Force an assert. */
    configASSERT( pcTaskName == 0 );
}

/**
 * @brief If configUSE_STATIC_ALLOCATION is set to 1, so the application must
 * provide an implementation of vApplicationGetIdleTaskMemory() to provide the
 * memory that is used by the Idle task.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be
     * allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__((aligned(32)));

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be
     * allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ] __attribute__((aligned(32)));

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/
void MemManage_Handler( void )
{
    __asm volatile
    (
        " tst lr, #4                                        \n"
        " ite eq                                            \n"
        " mrseq r0, msp                                     \n"
        " mrsne r0, psp                                     \n"
        " ldr r1, handler_address_const                     \n"
        " bx r1                                             \n"
        "                                                   \n"
        " handler_address_const: .word vHandleMemoryFault   \n"
    );
}
/*-----------------------------------------------------------*/
portDONT_DISCARD void vHandleMemoryFault( uint32_t * pulFaultStackAddress )
{
    uint32_t ulPC;
    uint16_t usOffendingInstruction;

    /* Read program counter. */
    ulPC = pulFaultStackAddress[ 6 ];

    /* Read the offending instruction. */
    usOffendingInstruction = *( uint16_t * ) ulPC;

    /* From ARM docs:
     * If the value of bits[15:11] of the halfword being decoded is one of
     * the following, the halfword is the first halfword of a 32-bit
     * instruction:
     * - 0b11101.
     * - 0b11110.
     * - 0b11111.
     * Otherwise, the halfword is a 16-bit instruction.
     */

    /* Extract bits[15:11] of the offending instruction. */
    usOffendingInstruction = usOffendingInstruction & 0xF800;
    usOffendingInstruction = ( usOffendingInstruction >> 11 );

    /* Determine if the offending instruction is a 32-bit instruction or
     * a 16-bit instruction. */
    if( ( usOffendingInstruction == 0x001F ) ||
            ( usOffendingInstruction == 0x001E ) ||
            ( usOffendingInstruction == 0x001D ) )
    {
        /* Since the offending instruction is a 32-bit instruction,
         * increment the program counter by 4 to move to the next
         * instruction. */
        ulPC += 4;
    }
    else
    {
        /* Since the offending instruction is a 16-bit instruction,
         * increment the program counter by 2 to move to the next
         * instruction. */
        ulPC += 2;
    }

    /* Save the new program counter on the stack. */
    pulFaultStackAddress[ 6 ] = ulPC;
}

/**
 * @} main
 */

/**
 * @} Application
 */
