/**
******************************************************************************
* @file    		motor.h
* @brief   		��� ���Э��  ����ת������ 200ms
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
//#include "display.h"			// ��ʾģ��
//#include "port.h"
//#include "mbcrc.h"				// crc

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define MOTOR_THREAD_LIFECYCLE								5				// ������������ 50ms

#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#define	MOTOR_RPM_CONVERSION_COEFFICIENT				(21)		//�ٷֱ� ת ת�� ת��ϵ��

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3		// Э���ͷ ƫ�Ƶ�ַ
/*------------------- IO define ----------------------------------------------*/
#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			128

/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
//------------------- ��ѭ������  ----------------------------
void App_Motor_Handler(void);
	
//------------------- ���ת�ٸ��� ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- ���ת���Ƿ�ﵽĿ��ֵ ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- �ٷֱ�ת ת�� ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);

//------------------- ���� ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- ���� ----------------------------
extern void Motor_Heartbeat_Send(void);
//------------------- ������״̬ ----------------------------
extern void Motor_ReadFault_Send(void);
//-------------------- ���Ĵ��� ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- �շ����� ----------------------------------------------*/
//-------------------- ���� ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- ���� ----------------------------
extern void Motor_RxData(uint8_t len);



/* Private defines -----------------------------------------------------------*/

//**************** �շ�������
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//����һ�����ջ�����
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//����һ�����ͻ�����



#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

