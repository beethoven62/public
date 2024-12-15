/** @file fs_priv.h
 *
 * @brief       File system module (private header).
 * @author      Johnas Cukier
 * @date        April 2023
 */

/**
 * @addtogroup fs
 * @{
 * @brief       This is a driver that uses littleFS produced by ARM. Please refer to this website:
 *              https://github.com/littlefs-project/littlefs#start-of-content
 * @par         LittleFS Description:                                                                        <br>
 *              A little fail-safe filesystem designed for microcontrollers.                        <br>
 *                                                                                                  <br>
 *              Power-loss resilience - littlefs is designed to handle random power failures. All   <br>
 *              file operations have strong copy-on-write guarantees and if power is lost the       <br>
 *              filesystem will fall back to the last known good state.                             <br>
 *                                                                                                  <br>
 *              Dynamic wear leveling - littlefs is designed with flash in mind, and provides wear  <br>
 *              leveling over dynamic blocks. Additionally, littlefs can detect bad blocks and work <br>
 *              around them.                                                                        <br>
 *                                                                                                  <br>
 *              Bounded RAM/ROM - littlefs is designed to work with a small amount of memory. RAM   <br>
 *              usage is strictly bounded, which means RAM consumption does not change as the       <br>
 *              filesystem grows. The filesystem contains no unbounded recursion and dynamic memory <br>
 *              is limited to configurable buffers that can be provided statically.                 <br>
 */

#ifndef __NRF_FS_PRIV_H__
#define __NRF_FS_PRIV_H__

/***************************************************************************************************************************
 * Includes
 */

#include "fs.h"

/***************************************************************************************************************************
* Private constants and macros
*/

#define NVDATA_SIZE             ( 0x1d000 )
#define FS_BUFFER_SIZE          ( 16 )

/***************************************************************************************************************************
* Private data structures and typedefs
*/

typedef struct
{
    TaskHandle_t handle;
    char msg[ SHORT_MSG_MAX ];
} fs_msg_t;

typedef struct
{
    bool                        is_init;
} fs_obj_t;

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
static error_code_module_t fs_Init( void );

/**
 * @brief       Read a block
 */
int32_t fs_read( const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size );

/**
 * @brief       Program a block
 * @details     The block must have previously been erased.
 */
int32_t fs_prog( const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size );

/**
 * @brief       Erase a block
 * @details     A block must be erased before being programmed. The
 *              state of an erased block is undefined.
 */
int32_t fs_erase( const struct lfs_config *cfg, lfs_block_t block );

/**
 * @brief       Sync the block device
 */
int32_t fs_sync( const struct lfs_config *cfg );

/***************************************************************************************************************************
 * Private prototypes
 */

/**
 * @brief       Non-volatile memory test.
 */
static void fs_test_nvmc( const struct lfs_config *cfg, const struct lfs_file_config *file_cfg );

#endif /* __FS_PRIV_H__ */

/**
 * @}
 */
