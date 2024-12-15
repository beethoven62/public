/*-----------------------------------------------------------------------------
 * Name:    stdout_user.c
 * Purpose: STDOUT User Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/

/*  Copyright (c) 2013 - 2015 ARM LIMITED

    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
    - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
    *
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
    ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "uart.h"

/**
 * Put a character to the stdout
 *
 * @param[in]   ch  Character to output
 * @return          The character written, or -1 on write error.
 */
int stdout_putchar( int ch )
{
    return ( uart.Transmit( uart_debug_port, ( uint8_t *)&ch, 1 ) );
}

#if BUILD_TOOL_SES
#include "__SEGGER_RTL_Int.h"

/*********************************************************************
*
*       __SEGGER_RTL_X_file_write()
*
*  Function description
*    Write data to file.
*
*  Parameters
*    stream - Pointer to file to write to.
*    s      - Pointer to object to write to file.
*    len    - Number of characters to write to the file.
*
*  Return value
*    >= 0 - Success.
*    <  0 - Failure.
*
*  Additional information
*    this version is NOT reentrant!
*    a reentrant version needs to lock use of the output ring buffer and _UART_Busy.
*    stdout and stderr are directed to UART;
*    writing to any stream other than stdout or stderr results in an error
*
*  Note: This works for Segger Embedded Studio v5. For Segger Embedded Studio v6 and
*  later, follow instructions on this page:
*      https://wiki.segger.com/Embedded_Studio_Library_IO
*/
int __SEGGER_RTL_X_file_write_std(FILE *stream, const char *s, unsigned len)
{
    unsigned p;

    if ((stream == stdout) || (stream == stderr))
    {
        for (p = 0; p < len; ++p)
        {
            stdout_putchar( s[ p ] );
        }
        return len;
    }
    else
    {
        return EOF;
    }
}
#endif
