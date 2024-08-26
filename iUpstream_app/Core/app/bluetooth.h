/**
******************************************************************************
* @file    		bluetooth.h
* @brief   		bluetooth 模组
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

	
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	BT_NO_CONNECT = 0,				//	无连接
	BT_DISTRIBUTION,					//	配网中
	BT_WORKING,								//	正常运行
	BT_ERROR,									//	网络故障	
} BT_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define BT_DISTRIBUTION_BLINK_TIME				2				// 2 S

#define BT_ERROR_BLINK_TIME								1				// 1 S


#define BT_DISTRIBUTION_TIME_OUT					180			// 3 min


/* Exported functions prototypes ---------------------------------------------*/
//串口发送接口
extern void SerialWrite(unsigned char *buff,int length);
//接收中断调用
extern void Usart_IRQ_CallBack(uint8_t data);
//------------------- 设置wifi状态机 ----------------------------
extern void BT_Set_Machine_State(BT_STATE_MODE_E para);
//------------------- 获取wifi状态机 ----------------------------
extern BT_STATE_MODE_E BT_Get_Machine_State(void);


//------------------- 接收处理函数 ----------------------------
extern void BT_Read_Handler(void);

//------------------- 进入配网 ----------------------------
extern void BT_Get_In_Distribution(void);
//------------------- 进入故障 ----------------------------
extern void BT_Get_In_Error(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H__ */

