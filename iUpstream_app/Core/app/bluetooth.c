/**
******************************************************************************
* @file				bluetooth.c
* @brief			bluetooth ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"
#include "my_modbus.h"
#include "modbus.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static BT_STATE_MODE_E BT_State_Machine=0;
//���ڽ���
uint8_t BT_Uart_Read_Buffer;
//����һ������
ModbusSlaveObj_t Ms_BT_Modbus;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//���ڷ��ͽӿ�
void SerialWrite(unsigned char *buff,int length)
{
	HAL_UART_Transmit(&huart5,buff,length,1000);
}

//���ڷ��ͽӿ�
void BT_Read_Data_Bit(unsigned char vaule)
{
	MsSerialRead(&Ms_BT_Modbus,&vaule,1);
}

//�����жϵ���
void Usart_IRQ_CallBack(uint8_t data)
{
	BT_Uart_Read_Buffer = data;
	MsSerialRead(&Ms_BT_Modbus,&BT_Uart_Read_Buffer,1);
}

//------------------- ���� Modbus ���ó�ʼ�� ----------------------------
void BT_Modbus_Config_Init(void)
{
	//��ʼ������
	MsInit(&Ms_BT_Modbus,21,1,SerialWrite);
	//����01�Ĵ����Ĳ����������õĻ��᷵����Ч�Ĺ��ܴ�����
	//MsConfigureRegister(&Ms_BT_Modbus,0x01,buff01,sizeof(buff01));
	//MsConfigureRegister(&Ms_BT_Modbus,0x0F,buff01,sizeof(buff01));
	MsConfigureRegister(&Ms_BT_Modbus,0x03,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x06,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x10,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x05,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	
}
/*******************************************************************************
*���ܣ���ʱ�������ŵ���ʱ�������У�һ��ŵ�1ms�ж��㹻����Ҫ��
*******************************************************************************/
void BT_MsTimeout(void)
{
	MsTimeout(&Ms_BT_Modbus);
}

//------------------- ����wifi״̬�� ----------------------------
void BT_Set_Machine_State(BT_STATE_MODE_E para)
{
	if(para <= BT_ERROR)
	{
		BT_State_Machine = para;
	}
}
	
//------------------- ��ȡwifi״̬�� ----------------------------
BT_STATE_MODE_E BT_Get_Machine_State(void)
{
	return BT_State_Machine;
}


//------------------- ���մ����� ----------------------------
void BT_Read_Handler(void)
{
	MsProcess(&Ms_BT_Modbus);
}

//------------------- �������� ----------------------------
void BT_Get_In_Distribution(void)
{
	BT_Set_Machine_State( BT_DISTRIBUTION );
	
}

//------------------- ������� ----------------------------
void BT_Get_In_Error(void)
{
	BT_Set_Machine_State( BT_ERROR );
	
}



