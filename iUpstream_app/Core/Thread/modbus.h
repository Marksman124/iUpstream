/**
******************************************************************************
* @file    		modbus.h
* @brief   		modbus协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODBUS_H__
#define __MODBUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "usart.h"
#include "iap.h"
#include "string.h"
#include "gpio.h"

#include "metering.h" // 消息队列

#include "control_interface.h"	 // 控制接口
/* Private defines -----------------------------------------------------------*/


#define MODBUS_THREAD_LIFECYCLE							100				// ms

extern UART_HandleTypeDef* p_huart_mb;		 //UART句柄

// 03 Holding

#define MB_SLAVE_NODE_ADDRESS								( 0x00 )	//	从机节点地址
#define MB_SLAVE_BAUD_RATE             			( 0x01 )	//	波特率
#define MB_SUPPORT_CONTROL_METHODS          ( 0x02 )	//	0:可控;1：屏蔽  // 1bit：蓝牙; 2bit：Modbus-RS485; 3bit：wifi;
#define MB_DISTRIBUTION_NETWORK_CONTROL  		( 0x03 )	//	配网控制 遥控 & wifi
#define MB_MOTOR_POLE_NUMBER  							( 0x04 )	//	电机磁极数
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_WORKING_MODE     					( 0x61 )	//	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
#define MB_SYSTEM_WORKING_STATUS      			( 0x62 )	//	系统工作状态  0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
#define MB_SYSTEM_POWER_ON				        	( 0x63 )	//	系统开机 0：关机  1：开机
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_CURRENT_SPEED		        	( 0x70 )	//	当前转速 (临时有效)
#define MB_MOTOR_CURRENT_TIME        				( 0x71 )	//	当前时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_FREE_MODE_SPEED		        	( 0x80 )	//	用户 自由模式 	转速
#define MB_USER_FREE_MODE_TIME        			( 0x81 )	//								时间
#define MB_USER_TIME_MODE_SPEED		        	( 0x82 )	//	用户 定时模式 	转速
#define MB_USER_TIME_MODE_TIME        			( 0x83 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P1_1		        ( 0x100 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_1        		( 0x101 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P1_2		        ( 0x102 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_2        		( 0x103 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P1_50		      ( 0x162 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_50        		( 0x163 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P2_1		        ( 0x180 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_1        		( 0x181 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P2_2		        ( 0x182 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_2        		( 0x183 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P2_50		      ( 0x1E2 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_50        		( 0x1E3 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P3_1		        ( 0x200 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_1        		( 0x201 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P3_2		        ( 0x202 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_2        		( 0x203 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P3_50		      ( 0x262 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_50        		( 0x263 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P4_1		        ( 0x280 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_1        		( 0x281 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P4_2		        ( 0x282 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_2        		( 0x283 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P4_50		      ( 0x2E2 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_50        		( 0x2E3 )	//								时间
// ----------------------------------------------------------------------------------------------


// 04 Input
#define MB_DISPLAY_SOFTWARE_VERSION								( 0x00 )	//	显示板 软件版本
#define MB_DISPLAY_HARDWARE_VERSION             	( 0x02 )	//	显示板 硬件版本
#define MB_DRIVER_SOFTWARE_VERSION								( 0x04 )	//	驱动板 软件版本
#define MB_DRIVER_HARDWARE_VERSION             		( 0x06 )	//	驱动板 硬件版本
#define MB_SYSTEM_FAULT_STATUS					          ( 0x08 )	//	系统故障状态

#define MB_MOTOR_FAULT_STATUS					          	( 0x09 )	// 	电机 	故障状态
#define MB_MOS_TEMPERATURE					          		( 0x0A )	//	mos 	温度
#define MB_MOTOR_TEMPERATURE					          	( 0x0B )	//	电机	温度
#define MB_MOTOR_CURRENT					          			( 0x0C )	// 	电机 	电流
#define MB_MOTOR_REALITY_SPEED					          ( 0x0E )	//	电机 	实际 转速
#define MB_MOTOR_BUS_VOLTAGE					          	( 0x10 )	//	母线 	电压

/*
#define MB_DEFAULT_FREE_MODE_SPEED		        		( 0x11 )	//	默认 自由模式 	转速
#define MB_DEFAULT_FREE_MODE_TIME        					( 0x12 )	//								时间
#define MB_DEFAULT_TIME_MODE_SPEED		        		( 0x13 )	//	默认 定时式 	转速
#define MB_DEFAULT_TIME_MODE_TIME        					( 0x14 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_1		      ( 0x15 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_1        		( 0x16 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_2		      ( 0x17 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_2        		( 0x18 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_12		      ( 0x2B )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_12        	( 0x2C )	//								时间

#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_1		      ( 0x2D )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_1        		( 0x2E )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_2		      ( 0x2F )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_2        		( 0x30 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_12		      ( 0x43 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_12        	( 0x44 )	//								时间

#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_1		      ( 0x45 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_1        		( 0x46 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_2		      ( 0x47 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_2        		( 0x48 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_12		      ( 0x5B )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_12        	( 0x5C )	//								时间

*/

/* Exported functions prototypes ---------------------------------------------*/

extern void Modbus_Init(void);
extern void Modbus_Handle_Task(void);

// *********  修改内部数据接口  *************************************
extern void Modbus_Buffer_Init(void);
extern void MB_Flash_Buffer_Write(void);
extern uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr);
extern uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr);
extern void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value);
extern void Set_DataValue_U32(UCHAR ucFunctionCode, USHORT addr, uint32_t value);

extern void Get_Mapping_Register(void);
#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_H__ */

