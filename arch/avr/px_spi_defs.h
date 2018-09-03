#ifndef __PX_SPI_DEFS_H__
#define __PX_SPI_DEFS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
   
    Title:          px_spi.h : SPI peripheral compatibility definitions
    Author(s):      Pieter Conradie
    Creation Date:  2013-11-23

============================================================================= */

#include <avr/io.h>

#ifndef SPCR
#define SPCR  SPCR0
#define SPCR  SPCR0
#define SPR0  SPR00
#define SPR1  SPR10
#define CPHA  CPHA0
#define CPOL  CPOL0
#define MSTR  MSTR0
#define DORD  DORD0
#define SPE   SPE0
#define SPIE  SPIE0

#define SPSR  SPSR0
#define SPI2X SPI2X0
#define WCOL  WCOL0
#define SPIF  SPIF0

#define SPDR  SPDR0
#endif

#endif // #ifndef __PX_SPI_DEFS_H__
