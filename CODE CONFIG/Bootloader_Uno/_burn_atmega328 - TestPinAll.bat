
@set COMX=COM4

@echo.
@echo ======= Burn Bootloader for atmega328_8mhz =======
@echo.

@echo.
@echo ======= Burn 3 fuses =======
@echo.
avrdude.exe -v   -e -c avrisp -p atmega328p -P "//./%COMX%" -b 19200 -U lfuse:w:0xE2:m  -U hfuse:w:0xDF:m  -U efuse:w:0x05:m
@rem  Feature configuration: Int. RC Osc. 8MHz; ...; [CKSEL=0010 SUT=10]; default value 
@rem  Use Internal RC Oscillator @8MHz
@rem  Extended fuse: (efuse) 0x05

@echo.
@echo ======= Burn HEX file =======
@echo.
avrdude.exe -v   -e -c avrisp -p atmega328p -P "//./%COMX%" -b 19200 -U flash:w:TestPinAll.cpp.hex   -U lock:w:0x0F:m


@echo.
@echo ======= Complete! =======
@echo.

@pause
