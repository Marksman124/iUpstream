@echo on
setlocal EnableDelayedExpansion

::SET COMPILER_PATH="C:\Keil_v5\ARM\ARMCC\include\..\bin" 
::SET AXF_FILE="E:\My_Project\temp_project\iUpstream_app\MDK-ARM\iUpstream_app\iUpstream_app.axf" 
::SET BIN_FILE="E:\My_Project\temp_project\iUpstream_app\MDK-ARM\Output\iUpstream_app.bin"
SET COMPILER_PATH=%1
SET AXF_FILE=%2
SET BIN_FILE=%3

:: 生成bin文件
%COMPILER_PATH%\fromelf.exe --bin -o %BIN_FILE% %AXF_FILE%

SET SCRPIT_PATH=%cd%
CD ..\
SET ROOT_PATH=%cd%
SET LIB_PATH=%ROOT_PATH%\lib
SET BIN_PATH=%ROOT_PATH%\Bin
SET NEW_BIN_FILE=%SCRPIT_PATH%\iUpstream_mcu\iUpstream_app.bin
SET BIN_TOOL=%ROOT_PATH%\Tools\BinTool\BinTool.exe
SET CRC16_TOOL=%ROOT_PATH%\Tools\AddCRC16Tool\AddCRC16.exe
SET BOOTLOADER_FILE=%ROOT_PATH%\Bin\iUpstream_boot.bin
SET APP_BURN_FILE=%ROOT_PATH%\Bin\iUpstream_app.bin
SET APP_OTA_FILE=%ROOT_PATH%\Bin\iUpstream_ota.bin
SET APP_OFT_ADDR=0x8000

%BIN_TOOL% %BOOTLOADER_FILE% %BIN_FILE% %APP_BURN_FILE% %APP_OFT_ADDR%
%CRC16_TOOL% %BIN_FILE% %APP_OTA_FILE%

::DEL "%NEW_BIN_FILE%" /f /q