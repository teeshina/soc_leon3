@echo off

if "%1"=="ml605" goto ml605_target

:default_target
set FPGA_BOARD=PC_SIMULATION
set CC=sparc-elf-gcc.exe
set CFLAGS= -c -g -msoft-float -O2 -D%FPGA_BOARD% -mcpu=v8 -mv8
set LDFLAGS=

rem Alternative entry point 
rem LDFLAGS =  -Ttext=0xA0000000 
rem -Tdata=0xA0010000  -Tbss=0xA0030000
goto workpath


:ml605_target
set FPGA_BOARD=VIRTEX_ML605
set CC=sparc-elf-c++.exe
set CFLAGS=-c -Wall -Wno-deprecated -msoft-float -mcpu=v8 -D%FPGA_BOARD%
set LDFLAGS=
goto workpath


rem ---------------------------------------------------------------------------------------------------
:workpath

set PATH=E:\cygwin\bin;E:\fw_tools\gnutools;E:\fw_tools\gnutools\cyg;%SystemRoot%\SYSTEM32
set PATH=E:\fw_tools\sparc-elf-4.4.1-mingw\bin;%PATH%
set PATH=E:\fw_tools\sparc-elf-4.4.2-mingw\bin;%PATH%
set PATH=C:\cygwin\bin;%PATH%
set PATH=C:\opt\sparc-elf-3.4.4-mingw\bin;%PATH%
set PATH=C:\opt\sparc-elf-4.4.2-mingw\bin;%PATH%


make all

pause
exit

