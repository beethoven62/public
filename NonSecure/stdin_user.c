/*-----------------------------------------------------------------------------
 * Name:    stdin_user.c
 * Purpose: STDIN User Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2015 ARM LIMITED

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
#include "uart.h"

/**
 *  Get a character from stdin
 *  @return     The next character from the input, or -1 on read error.
 */
int stdin_getchar( void )
{
    int8_t ch;

    if ( uart.Receive( uart_debug_port, ( uint8_t * ) &ch, 1 ) == 0 )
    {
        ch = -1;
    }

    return ch;
}

#if BUILD_TOOL_SES
#include "__SEGGER_RTL_Int.h"

/*********************************************************************
*
*       __SEGGER_RTL_X_file_read()
*
*  Function description
*    Read data from file.
*
*  Parameters
*    stream - Pointer to file to read from.
*    s      - Pointer to object that receives the input.
*    len    - Number of characters to read from file.
*
*  Return value
*    >= 0 - Success, amount of data read.
*    <  0 - Failure.
*
*  Additional information
*    Reading from any stream other than stdin results in an error.
*
*  Note: This works for Segger Embedded Studio v5. For Segger Embedded Studio v6 and
*  later, follow instructions on this page:
*      https://wiki.segger.com/Embedded_Studio_Library_IO
*/
int __SEGGER_RTL_X_file_read_std(FILE *stream, char *s, unsigned len)
{
    int c;

    if (stream == stdin)
    {
        c = 0;
        while ( len > 0 )
        {
            *s = ( char )stdin_getchar();
            ++s;
            ++c;
            --len;
        }
    }
    else
    {
        c = EOF;
    }
    return c;
}

/*********************************************************************
*
*       __SEGGER_RTL_X_file_unget()
*
*  Function description
*    Push character back to stream.
*
*  Parameters
*    stream - Pointer to file to push back to.
*    c      - Character to push back.
*
*  Return value
*    >= 0 - Success.
*    <  0 - Failure.
*
*  Additional information
*    Push-back is only supported for standard input, and
*    only a single-character pushback buffer is implemented.
*/
int __SEGGER_RTL_X_file_unget_std(FILE *stream, int c)
{
    return EOF;
}
#endif