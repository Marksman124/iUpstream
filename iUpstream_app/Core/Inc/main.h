/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//#define SYSTEM_HARDWARE_DEBUG
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// 串口1 --> 中控Modbus 	(485)
// 串口2 --> wifi 			(ttl)
// 串口3 --> 驱动板 			(ttl)
// 串口4 --> 调试 debug 	(ttl)
// 串口5 --> 蓝牙				(ttl)

#define MODBUS_USART								1
#define	WIFI_USART									2
#define	DRIVER_USART								3
#define	DEBUG_USART									4
#define	BLUETOOTH_USART							5


#define	SYSTEM_USER_USART_MAX				5

#define THREAD_PERIOD_RS485_MODBUS_TASK					MODBUS_THREAD_LIFECYCLE
#define THREAD_PERIOD_MAIN_TASK									TIMING_THREAD_LIFECYCLE
#define	THREAD_PERIOD_BREATH_LIGHT_TASK					BREATH_LIGHT_THREAD_LIFECYCLE
#define	THREAD_PERIOD_KEY_BUTTON_TASK						KEY_THREAD_LIFECYCLE
#define	THREAD_PERIOD_MOTOR_TASK								MOTOR_THREAD_LIFECYCLE


#define POWER_ON_WAITE_TIME_TASK								(6000)///30000

// 产品机型码
#define	SYSTEM_PRODUCT_MODEL_CODE								0x0001		//

// 软件版本
#define	SOFTWARE_VERSION_UINT32								0x00010000		//1.0.0
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define led_debug_Pin GPIO_PIN_13
#define led_debug_GPIO_Port GPIOC
#define Led_Power_Pin GPIO_PIN_0
#define Led_Power_GPIO_Port GPIOC
#define Led_Speed_Pin GPIO_PIN_1
#define Led_Speed_GPIO_Port GPIOC
#define Led_Time_Pin GPIO_PIN_2
#define Led_Time_GPIO_Port GPIOC
#define Led_Mode_Pin GPIO_PIN_3
#define Led_Mode_GPIO_Port GPIOC
#define PWM_Led_Ring_Pin GPIO_PIN_0
#define PWM_Led_Ring_GPIO_Port GPIOA
#define PWM_Led_Power_Pin GPIO_PIN_1
#define PWM_Led_Power_GPIO_Port GPIOA
#define RS485_A_Pin GPIO_PIN_2
#define RS485_A_GPIO_Port GPIOA
#define RS485_B_Pin GPIO_PIN_3
#define RS485_B_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOB
#define WR_Pin GPIO_PIN_14
#define WR_GPIO_Port GPIOB
#define DATA_Pin GPIO_PIN_15
#define DATA_GPIO_Port GPIOB
#define Key_Speed_Pin GPIO_PIN_7
#define Key_Speed_GPIO_Port GPIOC
#define Key_Time_Pin GPIO_PIN_8
#define Key_Time_GPIO_Port GPIOC
#define Key_Mode_Pin GPIO_PIN_9
#define Key_Mode_GPIO_Port GPIOC
#define Key_Power_Pin GPIO_PIN_8
#define Key_Power_GPIO_Port GPIOA
#define BackLight_Pin GPIO_PIN_15
#define BackLight_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
