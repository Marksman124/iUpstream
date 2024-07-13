/**
******************************************************************************
* @file    		operation.h
* @brief   		����ģ�� �������ز��� ����������������\ɾ��
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
	OPERATION_ADDR_SET=1,					//	��ַ����
	OPERATION_BAUD_RATE,					//	������
	OPERATION_SHIELD_MENU,				//	���ο��Ʒ�ʽ
	OPERATION_MOTOR_POLES,				//	�������
	OPERATION_DISPLAY_VERSION,		//	��ʾ�� �汾��
	OPERATION_DRIVER_VERSION,			//	������ �汾��
	OPERATION_STATE_END,					//	����
} OPERATION_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- ���������Ӧ ���� -------------------
#define CALL_OUT_NUMBER_MAX						8


#define OPERATION_BAUD_MAX						4
#define OPERATION_SHIELD_MAX					7
#define OPERATION_POLES_MAX						MOTOR_RPM_MAX_OF_POLES


#define OPERATION_POLES_MIX						MOTOR_RPM_MIX_OF_POLES
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Operation_Init(void);
	
// ��������˵�
extern void To_Operation_Menu(void);

/* Private defines -----------------------------------------------------------*/

// �̰� �ۺ���
extern void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void);

// ���� �ۺ���
extern void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __OPERATION_H__ */

