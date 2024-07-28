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

#define MOTOR_THREAD_LIFECYCLE								100				// 任务生命周期 50ms

#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#define	MOTOR_RPM_CONVERSION_COEFFICIENT				(21)		//百分比 转 转速 转换系数

//-------------- MOS 温度报警值 90°C -------------------
#define MOS_TEMP_ALARM_VALUE								90
//-------------- MOS 温度 降速 80°C -------------------
#define MOS_TEMP_REDUCE_SPEED								80

//通讯故障 报警时间
#define FAULT_MOTOR_LOSS_TIME						(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 秒

//驱动板故障 标志位
#define MOTOR_FAULT_SIGN_BIT						0x6F
#define CLEAN_MOTOR_FAULT(n)						(n &= ~MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3
#define	MOTOR_PROTOCOL_ADDR_MAX									73

#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET						0
#define	MOTOR_ADDR_MOTOR_TEMP_OFFSET						2
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET					4
#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET						22
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET						26

#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET						52
#define	MOTOR_ADDR_NTC1_TEMP_OFFSET							58
#define	MOTOR_ADDR_NTC2_TEMP_OFFSET							60
#define	MOTOR_ADDR_NTC3_TEMP_OFFSET							62

/*------------------- IO define ----------------------------------------------*/
#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			256

/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
// 重启
void Motor_Usart_Restar(void);
// 清除故障
void Clean_Motor_OffLine_Timer(void);
//------------------- 主循环函数  ----------------------------
void App_Motor_Handler(void);
	
//------------------- 电机转速更新 ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- 电机转速是否达到目标值 ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- 电机转速 目标值 设置 ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- 电机转速 目标值 设置 ----------------------------
extern uint8_t Motor_Speed_Target_Get(void);
//------------------- 百分比转 转速 ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);


//================================================== 内部调用接口
//-------------------- 获取电机故障状态 ----------------------------
uint8_t Get_Motor_Fault_State(void);


//------------------- 发送 ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- 心跳 ----------------------------
extern void Motor_Heartbeat_Send(void);
//------------------- 读故障状态 ----------------------------
extern void Motor_ReadFault_Send(void);
//-------------------- 读寄存器 ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- 收发处理 ----------------------------------------------*/
void Motor_State_Analysis(void);
//-------------------- 发送 ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- 接收 ----------------------------
extern void Motor_RxData(uint8_t len);



/* Private defines -----------------------------------------------------------*/

//**************** 收发缓冲区
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//定义一个接收缓存区
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//定义一个发送缓存区


extern DMA_HandleTypeDef hdma_usart3_rx;
#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

