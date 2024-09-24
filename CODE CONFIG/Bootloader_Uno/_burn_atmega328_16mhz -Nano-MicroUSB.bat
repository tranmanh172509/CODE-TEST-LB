
@set COMX=COM3

@echo.
@echo ======= Burn Bootloader for atmega8_8mhz =======
@echo.

@echo.
@echo ======= Burn 3 fuses =======
@echo.
avrdude.exe -v   -F   -e -c avrisp -p atmega328p -P "//./%COMX%" -b 19200 -U lfuse:w:0xFF:m  -U hfuse:w:0xDE:m  -U efuse:w:0x05:m

@echo.
@echo ======= Burn HEX file =======
@echo.
avrdude.exe -v   -F   -e -c avrisp -p atmega328p -P "//./%COMX%" -b 19200 -U flash:w:optiboot_atmega328.hex   -U lock:w:0x0F:m


@echo.
@echo ======= Complete! =======
@echo.

@pause
