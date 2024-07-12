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

#define TRAINING_MODE_NUMBER_MAX						3
#define TRAINING_MODE_PERIOD_MAX						12


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


/* Private defines -----------------------------------------------------------*/

extern Operating_Parameters OP_ShowNow;

extern Operating_Parameters* p_OP_ShowLater;

extern uint8_t Motor_Speed_Target;

// 训练模式 当前状态
extern uint8_t PMode_Now;

extern uint8_t Period_Now;

// 各模式 属性 初始值
extern Operating_Parameters OP_Init_Free;

extern Operating_Parameters OP_Init_Timing;

extern Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX];

	
// 各模式 属性
extern Operating_Parameters* p_OP_Free_Mode;

extern Operating_Parameters* p_OP_Timing_Mode;

extern Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX];

extern uint16_t* p_System_Fault_Static;			//故障状态
extern uint16_t* p_Motor_Fault_Static;				//故障状态
extern uint16_t* p_Mos_Temperature;					//mos 温度
extern uint16_t* p_Motor_Temperature;				//电机 温度
extern uint32_t* p_Motor_Current;						//电机 电流
extern uint32_t* p_Motor_Reality_Speed;			//电机 实际 转速
extern uint16_t* p_Motor_Bus_Voltage;				//母线 电压

extern uint16_t* p_Modbus_Node_Addr;
extern uint16_t* p_Modbus_Baud_Rate;
extern uint16_t* p_Support_Control_Methods;
extern uint16_t* p_Motor_Pole_Number;

#ifdef __cplusplus
}
#endif

#endif /* __DATA_H__ */

