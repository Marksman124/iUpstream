SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;C:\Program Files\TortoiseGit\bin;D:\Program Files\Altium Deigner Summer 09\System;C:\mingw32\bin;D:\Qt\4.8.6\bin;D:\Qt\qtcreator-2.5.2\bin;D:\Program Files\Git\cmd;C:\Program Files\Java\jdk-22\bin;C:\Users\g__mo\esp\tool\tools\cmake\3.24.0\bin;D:\work_place\ESP_repository\tools\esptool-master;D:\work_place\ESP_repository\tools\tools\idf-exe\1.0.3;D:\Program Files\Python3.8.1\Scripts\;D:\Program Files\Python3.8.1\;C:\Users\g__mo\AppData\Local\Microsoft\WindowsApps;D:\Tool\Android\platform-tools;C:\Windows\System32\Wbem;C:\Users\g__mo\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\g__mo\esp\tool\tools\cmake\3.24.0\bin;D:\work_place\ESP_repository;D:\Tool\Android\PhoenixSuit_v1.13\
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32F103RC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=iUpstream_boot
SET CPU_CLOCK=0x007A1200
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\iupstream_boot\startup_stm32f103xe._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\iap.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stmflash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\sys.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_msp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_timebase_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_gpio_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_tim_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_rcc_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_cortex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_pwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_flash_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\stm32f1xx_hal_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\system_stm32f1xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\iupstream_boot\mbcrc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\iUpstream_boot\iUpstream_boot.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\iUpstream_boot\iUpstream_boot.axf" --i32combined --output ".\iUpstream_boot\iUpstream_boot.hex"
fromelf --bin --output=.\Output\iUpstream_boot.bin .\iUpstream_boot\iUpstream_boot.axf
fromelf --bin --output=..\..\iUpstream_app\Bin\iUpstream_boot.bin .\iUpstream_boot\iUpstream_boot.axf
