# Arduino UNO multifunction shield example

This example adds native px-fwlib support for the common Arduino UNO
multifunction shield built around two cascaded 74HC595 shift registers and a
four-digit seven-segment display.

## Supported shield hardware

- multiplexed 4-digit, 7-segment display;
- decimal points, ASCII text, signed/unsigned decimal values, and raw segments;
- four active-low LEDs (D1..D4);
- active-low buzzer on D3;
- three active-low buttons (S1..S3);
- A0 potentiometer/LDR ADC input;
- optional A4 LM35/analog sensor input; and
- optional active-low IR receiver input on D2.

The display is multiplexed. `px_mfs_display_refresh()` should be called about
once per millisecond. The driver blanks the display while updating the two
74HC595 devices to suppress visible ghosting.

## Arduino UNO mapping

| Shield function | Arduino UNO pin |
| --- | --- |
| 74HC595 latch / clock / data | D4 / D7 / D8 |
| Buzzer | D3 |
| LEDs D1..D4 | D13 / D12 / D11 / D10 |
| Buttons S1..S3 | A1 / A2 / A3 |
| Potentiometer / LDR | A0 |
| Optional LM35 / analog sensor | A4 |
| Optional IR receiver | D2 |

The A4 position may also be populated for a DS18B20 on some shield variants.
When using a DS18B20, use px-fwlib's existing 1-Wire/DS18B20 support instead of
the ADC helper.

## Build

From this directory:

```text
make
make build=release
make program
```

The `make program` target follows the normal px-fwlib AVR programming flow. A
generated `.hex` can also be loaded through an Arduino bootloader programmer.

## Hardware test application

At startup the example:

1. lights all display segments and decimal points;
2. beeps and lights all four LEDs;
3. performs a full-width hexadecimal display sweep with a chasing decimal point;
4. exercises each LED independently; and
5. enters the normal ADC/button/IR test loop.

In the main loop:

- S1 toggles LED D1;
- S2 toggles LED D2;
- S3 toggles LED D3 and cycles the display between A0, A4, and `TEST`;
- LED D4 follows the optional IR input; and
- holding S1 for about one second runs the ten-bit ADC bit-display exercise.

The application writes diagnostics to the UNO hardware UART at 115200 8-N-1.

## Validation

The driver and example were built in Debug and Release configurations for the
ATmega328P and tested on a physical Arduino UNO-compatible multifunction shield,
including the four-digit display, decimal points, LEDs, pushbuttons, buzzer,
A0/A4 ADC inputs, and UART diagnostics.
