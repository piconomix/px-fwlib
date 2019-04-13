echo off
path c:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin;c:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin

set stage=1
echo connecting to STM32...
STM32_Programmer_CLI -c port=SWD freq=4000 reset=HWrst -w TrueSTUDIO\cubemx_hal\Debug\cubemx_hal.hex -v -g 0x00000000
if errorlevel 1 goto error

exit /B 0

:error
pause
exit /B %stage%
