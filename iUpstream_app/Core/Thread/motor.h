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

#define MOTOR_THREAD_LIFECYCLE								100				// ������������ 50ms

#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#define	MOTOR_RPM_CONVERSION_COEFFICIENT				(21)		//�ٷֱ� ת ת�� ת��ϵ��

//-------------- MOS �¶ȱ���ֵ 90��C -------------------
#define MOS_TEMP_ALARM_VALUE								90
//-------------- MOS �¶� ���� 80��C -------------------
#define MOS_TEMP_REDUCE_SPEED								80

//ͨѶ���� ����ʱ��
#define FAULT_MOTOR_LOSS_TIME						(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 ��

//��������� ��־λ
#define MOTOR_FAULT_SIGN_BIT						0x6F
#define CLEAN_MOTOR_FAULT(n)						(n &= ~MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3
#define	MOTOR_PROTOCOL_ADDR_MAX									73

#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET						0
#define	MOTOR_ADDR_MOTOR_TEMP_OFFSET						2
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET					4
#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET						22
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET						26

#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET						52
#define	MOTOR_ADDR_NTC1_TEMP_OFFSET							58
#define	MOTOR_ADDR_NTC2_TEMP_OFFSET							60
#define	MOTOR_ADDR_NTC3_TEMP_OFFSET							62

/*------------------- IO define ----------------------------------------------*/
#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			256

/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
// ����
void Motor_Usart_Restar(void);
// �������
void Clean_Motor_OffLine_Timer(void);
//------------------- ��ѭ������  ----------------------------
void App_Motor_Handler(void);
	
//------------------- ���ת�ٸ��� ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- ���ת���Ƿ�ﵽĿ��ֵ ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
extern uint8_t Motor_Speed_Target_Get(void);
//------------------- �ٷֱ�ת ת�� ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);


//================================================== �ڲ����ýӿ�
//-------------------- ��ȡ�������״̬ ----------------------------
uint8_t Get_Motor_Fault_State(void);


//------------------- ���� ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- ���� ----------------------------
extern void Motor_Heartbeat_Send(void);
//------------------- ������״̬ ----------------------------
extern void Motor_ReadFault_Send(void);
//-------------------- ���Ĵ��� ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- �շ����� ----------------------------------------------*/
void Motor_State_Analysis(void);
//-------------------- ���� ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- ���� ----------------------------
extern void Motor_RxData(uint8_t len);



/* Private defines -----------------------------------------------------------*/

//**************** �շ�������
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//����һ�����ջ�����
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//����һ�����ͻ�����


extern DMA_HandleTypeDef hdma_usart3_rx;
#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

