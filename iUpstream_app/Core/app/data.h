/**
******************************************************************************
* @file    		display.h
* @brief   		显示模块  显示模块本身不占用线程,由其它线程任务调用
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATA_H__
#define __DATA_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <string.h>
#include "state_machine.h"
#include "tm1621.h"
#include "motor.h"
//#include "modbus.h"

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef struct Operating_Parameters
{
	uint16_t speed;
	uint16_t time;
}Operating_Parameters;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- 训练模式 最大值 -------------------

#define TRAINING_MODE_NUMBER_MAX						5
#define TRAINING_MODE_PERIOD_MAX						50
//冲浪模式 --》 P5
#define SURFING_MODE_NUMBER_ID							5
//------------------- 合法范围 ----------------------------
#define SPEED_LEGAL_MIN						20
#define SPEED_LEGAL_MAX						100

#define TIME_LEGAL_MIN						20
#define TIME_LEGAL_MAX						6000

#define	MOTOR_RPM_NUMBER_OF_POLES								5		//电机级数 默认值
#define	MOTOR_RPM_MIX_OF_POLES									1		//电机级数 合法最小值
#define	MOTOR_RPM_MAX_OF_POLES									8		//电机级数 合法最大值

/* Exported functions prototypes ---------------------------------------------*/

extern void Check_Data_Init(void);

extern void App_Data_Init(void);

extern void App_Data_ReInit(void);
// 读 flash
extern uint8_t Read_OPMode(void);
extern uint8_t Memset_OPMode(void);

extern void Update_OP_Data(void);

// 更改属性值
extern uint8_t Set_Data_OPMode(Operating_Parameters* p_op);
//------------------- 判断 模式 合法 ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode);
//------------------- 判断速度 合法 ----------------------------
extern uint8_t Is_Speed_Legal(uint16_t speed);

extern uint8_t Is_Time_Legal(uint16_t time);

//------------------- 设置 当前 速度 ----------------------------
extern void Data_Set_Current_Speed(uint8_t speed);
//------------------- 设置 当前 时间 ----------------------------
extern void Data_Set_Current_Time(uint16_t time);
//------------------- 设置 训练时段 ----------------------------
extern void Set_Pmode_Period_Now(uint16_t value);
//------------------- 是否接收外部控制 ----------------------------
extern uint8_t If_Accept_External_Control(void);
//------------------- 获取软件版本号  字符串转 uint32 ----------------------------
extern uint32_t get_uint3_version(char * buffer);
//------------------- 清除wifi标志 ----------------------------
extern void System_Wifi_State_Clean(void);


/* Private defines -----------------------------------------------------------*/

extern Operating_Parameters OP_ShowNow;

extern Operating_Parameters* p_OP_ShowLater;


// 训练模式 当前状态
extern uint8_t PMode_Now;

extern uint8_t Period_Now;

// 各模式 属性 初始值
extern Operating_Parameters OP_Init_Free;

extern Operating_Parameters OP_Init_Timing;

extern Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX];

//--------------------------- 系统属性
extern uint16_t* p_System_State_Machine;			//状态机
extern uint16_t* p_PMode_Now;									// 当前模式
extern uint16_t* p_OP_ShowNow_Speed;					// 当前速度
extern uint16_t* p_OP_ShowNow_Time;						// 当前时间

// 各模式 属性
extern Operating_Parameters* p_OP_Free_Mode;

extern Operating_Parameters* p_OP_Timing_Mode;

extern Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX];

extern uint16_t* p_Driver_Software_Version;			//驱动板软件版本

extern uint16_t* p_System_Fault_Static;			//故障状态
extern uint16_t* p_Motor_Fault_Static;			//故障状态
extern uint16_t* p_Mos_Temperature;					//mos 温度
extern uint16_t* p_Box_Temperature;				//电机 温度
extern uint32_t* p_Motor_Current;						//电机 电流
extern uint32_t* p_Motor_Reality_Speed;			//电机 实际 转速
extern uint16_t* p_Motor_Bus_Voltage;				//母线 电压

extern uint16_t* p_Modbus_Node_Addr;
extern uint16_t* p_Modbus_Baud_Rate;
extern uint16_t* p_Support_Control_Methods;
extern uint16_t* p_Motor_Pole_Number;
extern uint16_t* p_Breath_Light_Max;				//光圈亮度  

extern uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX];//电机状态

extern uint8_t System_Self_Testing_State;

//================= 临时变量  全局 ================================
extern uint16_t Temp_Data_P5_Acceleration;			//P5 加速度
extern uint16_t Temp_Data_P5_100_Time;					//P5 100% 时间	秒
extern uint16_t Temp_Data_P5_0_Time;						//P5 0% 	时间	秒

extern uint8_t WIFI_Rssi;


#ifdef __cplusplus
}
#endif

#endif /* __DATA_H__ */

