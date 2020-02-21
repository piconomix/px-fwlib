[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

![](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/hero_board/hero_board_isometric_top_bot.jpg)

STM32L072 PX-HER0 Board Crowd Supply campaign is now live! Click [HERE](https://www.crowdsupply.com/piconomix/px-her0-board)

Introduction
============

![https://piconomix.com](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/piconomix_banner_300x70.png)

**px-lib** is a collection of open source C firmware and documentation
for microcontrollers to develop portable bare-metal code that is vendor and 
architecture neutral (or easier to reuse).

It is tough to find the best compromise between lean 8-bit targets, 
middle-of-the-road 16-bit targets, and resource rich 32-bit targets, but this 
cross-platform library aims to provide a good foundation before you are forced 
to add target specific code and getting locked in.

Click [HERE](https://piconomix.com/fwlib/index.html) to view the complete online 
documentation.

Click [HERE](https://sourceforge.net/projects/piconomic-fwlib/files) to download
releases of the open source library (source code and offline documentation).

Goals
=====

- Teach **good firmware development practices** to guide the next 
  generation of professional embedded engineers.
- Share **source code, knowledge and expertise** with our global community of 
  engineers, scientists and enthusiasts.
- Publish **concise quick start guides and tutorials** to reduce the 
  learning curve of new microcontrollers.
- Provide a **standard framework and drivers** for rapid code development.
- **Minimize porting** by providing unified peripheral driver APIs 
  (to maximize device driver reuse).

*My sincere hope is that seasoned veterans will pitch in and share their 
years of experience to mentor the future generation and raise the bar in terms 
of quality and quantity.*

Example
=======

Included with the library is a CLI (Command Line Interpreter) executing on an 
Arduino Uno R3 that creates a "Un*x Shell"-like environment so that you can 
experiment with GPIO, ADC, I2C and SPI using only an ANSI/VT100 terminal 
emulator (for example [Tera Term](http://en.sourceforge.jp/projects/ttssh2)).

![Electrodragon BMP280 breakout board connected to I2C](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/arduino_uno_board/arduino_uno_i2c_slave_bmp280.jpg)

Connect your board, fire up the CLI and verify that it works:

![CLI Exlorer executing on an Arduino Uno R3](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/arduino_uno_board/arduino_uno_cli_animated.gif)

The BMP280 is found at 7-bit slave address 0x76. The chip identification 
register value for the BMP280 is 0x58. This confirms that you are able to write 
to and read from the I2C slave.

After verification, it is easy to accomplish the same in C using the library:

```c
#include "px_i2c.h"
#include "px_board.h"

// Bosch BMP280 I2C Slave Address
#define I2C_SLA_ADR   0x76

// Create I2C Slave handle object
px_i2c_handle_t px_i2c_handle;

int main(void)
{
    uint8_t data[1];

    // Initialise board
    px_board_init();
    // Initialise I2C driver
    px_i2c_init();
    // Open handle to I2C slave device
    px_i2c_open(&px_i2c_handle, PX_I2C_PER_0, I2C_SLA_ADR);
    // START I2C write transaction and write register address
    data[0] = 0xd0;
    px_i2c_wr(&px_i2c_handle, 
              data,
              1,
              PX_I2C_FLAG_START_AND_END);
    // REPEATED START I2C read transaction and read register value
    px_i2c_rd(&px_i2c_handle, 
              data,
              1,
              PX_I2C_FLAG_REP_START_AND_STOP);
    // Close I2C Handle
    px_i2c_close(&px_i2c_handle);
}
```

License
=======

[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

The [MIT license](https://en.wikipedia.org/wiki/MIT_License)
has been selected to grant freedom for both open source and commercial 
(closed source) projects and to nurture future growth of the library. Please see 
the [LICENSE](https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md).

Important Links
===============

- [Piconomix FW Library home page](https://piconomix.com/fwlib/index.html)
- [GitHub repository](https://github.com/piconomix/piconomix-fwlib)
- [Download page](https://sourceforge.net/projects/piconomic-fwlib/files) for file releases of the library (source code and offline documentation)

Questions or Feedback?
======================

Questions or feedback (positive and negative) is great and will help to improve 
the library:

![Feedback](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/feedback_animated.gif)

Methods of communication:
- [Forum](https://piconomix.com/forum)
- [Contact page](https://piconomix.com/contact)

Support
=======

A significant amount of time has been invested to craft and refine this free 
open source library. If you saved development time, acquired a new skill, or 
advanced your career, you are welcome to support this project with a visit to 
the shop.

[![Shop](https://github.com/piconomix/piconomix-fwlib/blob/master/doc/images/shop.png)](http://piconomix.com/shop/)
Click [HERE](http://piconomix.com/shop/) to visit the shop


