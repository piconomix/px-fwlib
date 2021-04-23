#ifndef __PX_SPI_DEFS_H__
#define __PX_SPI_DEFS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
   
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

#endif
