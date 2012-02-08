@echo off

set USE_ARM_MODE=0
set USE_ARM_COMPILER=GCC_LEON3
set FPGA_BOARD=VIRTEX_ML605

rem ---------------------------------------------------------------------------------------------------
:workpath

set PATH=E:\cygwin\bin;E:\fw_tools\gnutools;E:\fw_tools\gnutools\cyg;%SystemRoot%\SYSTEM32
set PATH=E:\fw_tools\sparc-elf-4.4.1-mingw\bin;%PATH%
set PATH=E:\fw_tools\sparc-elf-4.4.2-mingw\bin;%PATH%
set TARGET=fw_hello_world
set USE_FPU=0

rem IF EXIST make_gcc_leon3.bat call make_gcc_leon3.bat
rem m4.exe -Dfilename=gcc_leon3_settings.mak cpu_settings.m4 > cpu_settings.mak

rem make -f makefile %TARGET% noerrfile=%2 USE_ARM_MODE=%USE_ARM_MODE% USE_ARM_COMPILER=%USE_ARM_COMPILER% FPGA_BOARD=%FPGA_BOARD%
make all

pause
exit

