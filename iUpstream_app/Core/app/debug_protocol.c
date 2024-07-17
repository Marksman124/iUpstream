/**
******************************************************************************
* @file    		debug_protocol.c
* @brief			调试协议
*
*
* @author			WQG
* @versions   v2.0.2
* @date   		2024-4-10
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "debug_protocol.h"	///////////////////////	串口调试
#include "motor.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#if DEBUG_USART == 4
UART_HandleTypeDef* p_huart_debug = &huart4;		 //调试串口 UART句柄
#elif DEBUG_USART == 5
UART_HandleTypeDef* p_huart_debug = &huart5;
#endif


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Debug_Protocol_Mode	=	0;
uint8_t Debug_Send_Buffer[DEBUG_PROTOCOL_TX_MAX];
uint8_t Debug_Read_Buffer[DEBUG_PROTOCOL_RX_MAX];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/



void Set_Debug_Protocol_Mode(uint8_t mode)
{
	if( mode == 2)
		Debug_Protocol_Mode = 2;
	else if( mode == 1)
		Debug_Protocol_Mode = 1;
	else
		Debug_Protocol_Mode = 0;
	
	Debug_Send_Buffer[0] = 0xAA;
	Debug_Send_Buffer[1] = 0xAA;
	Debug_Send_Buffer[2] = 0x60;
	Debug_Send_Buffer[3] = 0xDE;
	Debug_Send_Buffer[4] = Debug_Protocol_Mode;
	
#ifdef DEBUG_USART
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 5,0xFFFF); //将收到的信息发送出去
	//HAL_UART_Transmit_IT(p_huart_debug, Debug_Send_Buffer, 5); //将收到的信息发送出去
#endif
}


void UART_Send_Debug(uint8_t * p_buff, uint8_t len)
{
#ifdef DEBUG_USART
//	if(Debug_Protocol_Mode != 1)
//		return;
	
	memset(Debug_Send_Buffer, 0, DEBUG_PROTOCOL_TX_MAX);
	//Debug_Send_Buffer[0] = cmd;
	memcpy(Debug_Send_Buffer, p_buff, len);
	// 转发至串口  用于调试
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, len,0xFFFF); //将收到的信息发送出去
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//使用DMA发送数据

#else
	return;
#endif
}



//
void To_Debug_Protocol_Analysis(uint8_t len)
{
#ifdef DEBUG_USART

	//UART_Send_Debug(Debug_Read_Buffer, len);
	
	if(Debug_Read_Buffer[0] <= MOTOR_PROTOCOL_ADDR_MAX)
	{
		switch(Debug_Read_Buffer[0])
		{
			case MOTOR_ADDR_MOTOR_FAULT_OFFSET:
				Motor_State_Storage[MOTOR_ADDR_MOTOR_FAULT_OFFSET] = Debug_Read_Buffer[1];
				break;
		}
		
		Motor_State_Analysis();
	}
//	#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET						0
//#define	MOTOR_ADDR_MOTOR_TEMP_OFFSET						2
//#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET					4
//#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET						22
//#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET						26

//#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET						52
//#define	MOTOR_ADDR_NTC1_TEMP_OFFSET							58
//#define	MOTOR_ADDR_NTC2_TEMP_OFFSET							60
//#define	MOTOR_ADDR_NTC3_TEMP_OFFSET							62
	
	
	memset(Debug_Read_Buffer,0,DEBUG_PROTOCOL_RX_MAX);    				//清空缓存区
	__HAL_UART_CLEAR_IDLEFLAG(p_huart_debug);               //清除标志位
	HAL_UART_Receive_DMA(p_huart_debug,Debug_Read_Buffer,DEBUG_PROTOCOL_RX_MAX);  //开DMA接收，数据存入rx_buffer数组中。

	return;
#endif
}


// 初始化
void Debug_Protocol_Init(void)
{
	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_IDLE);//使能idle中断
	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_ERR);//
	
  HAL_UART_Receive_DMA(p_huart_debug,Debug_Read_Buffer,DEBUG_PROTOCOL_RX_MAX);//打开串口DMA接收
}

