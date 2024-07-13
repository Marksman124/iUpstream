/**
******************************************************************************
* @file    		fault.h
* @brief   		����ģ��s
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FAULT_H__
#define __FAULT_H__


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
	NO_FAULT_STATE = 0,			//	�����˵�
	ss
} FAULT_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- ���������Ӧ ���� -------------------
#define CALL_OUT_NUMBER_MAX						8



/* Exported functions prototypes ---------------------------------------------*/
// ��ʼ��
extern void App_Fault_Init(void);
// ������
extern uint8_t If_System_Is_Error(void);
// ���ù���ֵ
void Set_Fault_Data(uint16_t type);

//-------------------   ----------------------------

// ��ȡ����
extern uint8_t Get_Fault_Number_Sum(uint16_t para);
// ��ȡ���Ϻ�
extern uint8_t Get_Fault_Number_Now(uint16_t para, uint8_t num);
// ��������˵�
extern void To_Fault_Menu(void);
// �������״̬
extern void Clean_Fault_State(void);
// ���� ��ʾ
extern void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint16_t type);

/* Private defines -----------------------------------------------------------*/

// �̰� �ۺ���
extern void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void);

// ���� �ۺ���
extern void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __FAULT_H__ */

