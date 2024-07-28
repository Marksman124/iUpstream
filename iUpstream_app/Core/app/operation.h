/**
******************************************************************************
* @file    		operation.h
* @brief   		操作模块 用于隐藏操作 单独出来便于屏蔽\删除
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPERATION_H__
#define __OPERATION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
#include "state_machine.h"
#include "data.h"
#include "dev.h"
#include "modbus.h"
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

//typedef enum 
//{
#define OPERATION_ADDR_SET							(1)													//	地址设置
#define	OPERATION_BAUD_RATE							(OPERATION_ADDR_SET+1)			//	波特率
//#define	OPERATION_SPEED_MODE					(OPERATION_BAUD_RATE+1)			//	转速方式： 0：转速  1：功率
#define	OPERATION_MOTOR_POLES						(OPERATION_BAUD_RATE+1)			//	电机极数
#define	OPERATION_DISPLAY_VERSION				(OPERATION_MOTOR_POLES+1)			//	显示板 版本号
#define	OPERATION_SHIELD_MENU						(OPERATION_DISPLAY_VERSION+1)			//	屏蔽控制方式
#define	OPERATION_STATE_END							(OPERATION_SHIELD_MENU+1)			//	结束
//} OPERATION_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- 按键组合响应 总数 -------------------
#define CALL_OUT_NUMBER_MAX						8


#define OPERATION_BAUD_MAX						4
#define OPERATION_SHIELD_MAX					7
#define OPERATION_POLES_MAX						MOTOR_RPM_MAX_OF_POLES


#define OPERATION_POLES_MIX						MOTOR_RPM_MIX_OF_POLES
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Operation_Init(void);
	
// 进入操作菜单
extern void To_Operation_Menu(void);

/* Private defines -----------------------------------------------------------*/

// 短按 槽函数
extern void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void);

// 长按 槽函数
extern void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __OPERATION_H__ */

