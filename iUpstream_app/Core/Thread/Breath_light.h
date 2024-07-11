/**
******************************************************************************
* @file    		Breath_light.h
* @brief   		呼吸灯模块   独立线程
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BREATH_LIGHT_H__
#define __BREATH_LIGHT_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tim.h"
#include "display.h"
	
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define BREATH_LIGHT_THREAD_LIFECYCLE				20				// ms

#define BREATH_LIGHT_GEAR_POSITION				(1000/BREATH_LIGHT_THREAD_LIFECYCLE)				// 档位
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Breath_light_Init(void);
extern void App_Breath_light_Handler(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

