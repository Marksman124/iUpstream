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

typedef enum 
{
	OPERATION_ADDR_SET=1,					//	地址设置
	OPERATION_BAUD_RATE,					//	波特率
	OPERATION_SHIELD_MENU,				//	屏蔽控制方式
	OPERATION_MOTOR_POLES,				//	电机极数
	OPERATION_DISPLAY_VERSION,		//	显示板 版本号
	OPERATION_DRIVER_VERSION,			//	驱动板 版本号
	OPERATION_STATE_END,					//	结束
} OPERATION_STATE_MODE_E;

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

