/**
******************************************************************************
* @file    		motor.h
* @brief   		电机 相关协议  控制转速命令 200ms
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
//#include "display.h"			// 显示模块
//#include "port.h"
//#include "mbcrc.h"				// crc

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define MOTOR_THREAD_LIFECYCLE								5				// 任务生命周期 50ms

#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#define	MOTOR_RPM_CONVERSION_COEFFICIENT				(21)		//百分比 转 转速 转换系数

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3		// 协议表头 偏移地址
/*------------------- IO define ----------------------------------------------*/
#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			128

/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
//------------------- 主循环函数  ----------------------------
void App_Motor_Handler(void);
	
//------------------- 电机转速更新 ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- 电机转速是否达到目标值 ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- 电机转速 目标值 设置 ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- 百分比转 转速 ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);

//------------------- 发送 ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- 心跳 ----------------------------
extern void Motor_Heartbeat_Send(void);
//------------------- 读故障状态 ----------------------------
extern void Motor_ReadFault_Send(void);
//-------------------- 读寄存器 ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- 收发处理 ----------------------------------------------*/
//-------------------- 发送 ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- 接收 ----------------------------
extern void Motor_RxData(uint8_t len);



/* Private defines -----------------------------------------------------------*/

//**************** 收发缓冲区
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//定义一个接收缓存区
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//定义一个发送缓存区



#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

