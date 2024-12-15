/**
 * @addtogroup Application
 * @{
 *
 * @file      fs.h
 * @details   LittleFS implementation of file system.
 * @author    Johnas Cukier
 * @date      April 2023
 */

/**
 * @addtogroup fs
 * @{
 */
#ifndef __FS_H__
#define __FS_H__

/***************************************************************************************************************************
 * Includes
 */

#include <nrfx_nvmc.h>
#include <lfs.h>
#include "os.h"
#include "app.h"
#include "log.h"
#include "twdt.h"
#include "eelcodes.h"

/***************************************************************************************************************************
 * Public constants and macros
 */

/***************************************************************************************************************************
 * Public data structures and typedefs
 */
typedef struct
{
    error_code_module_t ( *Init )( void );
} const fs_interface_t;

/***************************************************************************************************************************
 * Public variables
 */

extern fs_interface_t fs;

#endif /* __FS_H__ */

/**
 * @} fs
 */

/**
 * @} Application
 */
