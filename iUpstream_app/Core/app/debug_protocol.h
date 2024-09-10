/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    debug_protocol.h
  * @brief   This file contains all the function prototypes for
  *          the debug_protocol.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_PROTOCOL_H__
#define __DEBUG_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "modbus.h"
#include <stdio.h>
/* Exported macro ------------------------------------------------------------*/
#define DEBUG_PROTOCOL_RX_MAX			64
#define DEBUG_PROTOCOL_TX_MAX			256

extern uint8_t debug_buffer[128];

// ����DEBUG_PRINT�꣬�����������������Ϣ
#ifdef UART_PRINTF_LOG
  #define DEBUG_PRINT(fmt, ...) 	printf((char*)debug_buffer,"DEBUG: " fmt "\n", ##__VA_ARGS__);UART_Send_Debug(debug_buffer,strlen((char*)debug_buffer));
#else
  #define DEBUG_PRINT(fmt, ...)
#endif

extern uint8_t Debug_Read_Buffer[DEBUG_PROTOCOL_RX_MAX];

#ifdef UART_DEBUG_SEND_CTRL
extern uint8_t Chassis_Temperature_Debug;
#endif

/* Exported functions prototypes ---------------------------------------------*/

void Add_Ctrl_Log(void);

void UART_Send_Debug(uint8_t * p_buff, uint8_t len);

void To_Debug_Protocol_Analysis(uint8_t len);

void Debug_Protocol_Init(void);
// ����
void Debug_Usart_Restar(void);
	
#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_PROTOCOL_H__ */

