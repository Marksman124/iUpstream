/**
******************************************************************************
* @file    		timing.h
* @brief   		timing function implementation
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMING_H__
#define __TIMING_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "display.h"			// 显示模块
#include "data.h"

#include "fault.h"				// 故障 菜单
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	TIMING_STATE_SUSPEND,				//	暂停
	TIMING_STATE_STARTING,			//	软启动
	TIMING_STATE_RUNNING,				//	运行
	
} SYSTEM_STATE_TIMING_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define TIMING_THREAD_LIFECYCLE				500				// ms  1000

/* Exported functions prototypes ---------------------------------------------*/

extern void App_Timing_Init(void);

extern void Clean_Timing_Timer_Cnt(void);
	
// 定时任务主线程
extern void App_Timing_Handler(void);

/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

