/**
******************************************************************************
* @file    		key.h
* @brief   		Key function implementation
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H__
#define __KEY_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "display.h"			// 显示模块
#include "gpio.h"
#include "usart.h"				// 串口
#include "timing.h"				// 后台定时器

#include "operation.h"		// 操作 菜单
#include "fault.h"				// 故障 菜单

#include "wifi.h"				// wifi 模组
#include "bluetooth.h"				// bluetooth 模组
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef struct IO_Hardware_Pin
{
	GPIO_TypeDef * port;
	uint16_t pin;
	
}IO_Hardware_Pin;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define KEY_THREAD_LIFECYCLE								200				// 任务生命周期 200ms

#define KEY_LONG_PRESS_TIME									15				//长按等待时间  15 * 200 = 3s
#define KEY_LONG_PRESS_TIME_SHORT						5					//短一点的 长按等待时间  5 * 200 = 1s


//-------------- 按键组合响应 总数 -------------------
#define KEY_CALL_OUT_NUMBER_MAX						8


#define KEY_VALUE_BIT_BUTTON_1						0x01
#define KEY_VALUE_BIT_BUTTON_2						0x02
#define KEY_VALUE_BIT_BUTTON_3						0x04
#define KEY_VALUE_BIT_BUTTON_4						0x08


/* Exported functions prototypes ---------------------------------------------*/

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 档位键
extern void on_pushButton_clicked(void);
// ② 时间键
extern void on_pushButton_2_clicked(void);
// ③ 模式键
extern void on_pushButton_3_clicked(void);
// ④ 开机键  短按
extern void on_pushButton_4_Short_Press(void);
// ① + ②  组合键  测试 设置故障
extern void on_pushButton_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
extern void on_pushButton_1_3_Short_Press(void);
// ② + ③  组合键  短按
extern void on_pushButton_2_3_Short_Press(void);
// ② + ④  组合键  短按
extern void on_pushButton_2_4_Short_Press(void);
//--------------- 长按 -----------------------
// 长按
extern void on_pushButton_1_Long_Press(void);
extern void on_pushButton_2_Long_Press(void);
extern void on_pushButton_3_Long_Press(void);
extern void on_pushButton_4_Long_Press(void);
extern void on_pushButton_1_2_Long_Press(void);
extern void on_pushButton_1_3_Long_Press(void);
extern void on_pushButton_2_3_Long_Press(void);
extern void on_pushButton_2_4_Long_Press(void);

//------------------- 硬件 & 驱动 ----------------------------
// 初始化
extern void App_Key_Init(void);
// 按键灯
extern void Led_Button_On(uint8_t para);
// 按键主循环任务
extern void App_Key_Handler(void);
// 获取按键
extern uint8_t Key_Get_IO_Input(void);
//------------------- 功能接口 ----------------------------

//	开机 进入自由模式
extern void System_Power_On(void);
//	关机
extern void System_Power_Off(void);
//	恢复出厂设置
extern void Restore_Factory_Settings(void);

/* Private defines -----------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

