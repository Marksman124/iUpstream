/**
******************************************************************************
* @file				bluetooth.c
* @brief			bluetooth 模组
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
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static BT_STATE_MODE_E BT_State_Machine = BT_NO_CONNECT;
//串口接收
uint8_t BT_Uart_Read_Buffer;
//声明一个对象
ModbusSlaveObj_t Ms_BT_Modbus;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//串口发送接口
void SerialWrite(unsigned char *buff,int length)
{
	HAL_UART_Transmit(&huart5,buff,length,0xFFFF);
}

//串口接收接口
void BT_Read_Data_Bit(unsigned char vaule)
{
	//MsSerialRead(&Ms_BT_Modbus,&vaule,1);
	if(Ms_BT_Modbus.rxWriteLock)
	{
			return;
	}

	Ms_BT_Modbus.rxTimerEnable = 1;
	Ms_BT_Modbus.rxTimerCnt = 0;

	Ms_BT_Modbus.rxBuff[Ms_BT_Modbus.rxWriteIdx] =vaule;
	if(Ms_BT_Modbus.rxWriteIdx < (MODBUS_SLAVE_TX_RX_MAX_LEN - 1))
	{
			Ms_BT_Modbus.rxWriteIdx++;
	}
	else
	{
		Ms_BT_Modbus.rxWriteIdx = 0;
	}
	
		
}

//接收中断调用
void Usart_IRQ_CallBack(uint8_t data)
{
	BT_Uart_Read_Buffer = data;
	MsSerialRead(&Ms_BT_Modbus,&BT_Uart_Read_Buffer,1);
}

// AT 指令 设 名称
void BT_Set_Name(void)
{
	char buff[32]={"AT+BLENAME=inverjet\r\n"};
	
	//sprintf(buff,"AT+BLENAME=inverjet\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}
// AT 指令 设 从机模式
void BT_Set_Mode(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMODE=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}
// AT 指令 设 MTU
void BT_Set_MTU(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMTU=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}
// AT 指令 进入透传
void BT_Set_TRANSENTER(uint8_t data)
{
	char buff[32]={0};
	
	if(data == 0)//退出
	{
		SerialWrite((uint8_t*)"+++",3);
	}
	else
	{
		sprintf(buff,"AT+TRANSENTER\r\n");
	
		SerialWrite((uint8_t*)buff,strlen(buff));
	}
}
//
void BT_Module_AT_Init(void)
{
	BT_Set_TRANSENTER(0);
	osDelay(1000);
	BT_Set_MTU(243);
	osDelay(2000);
	BT_Set_TRANSENTER(1);//进入透传
}
//
void BT_Module_AT_ReInit(void)
{
	SerialWrite((uint8_t*)"+++",3);
	osDelay(1000);
	BT_Set_Name();
	osDelay(1000);
	BT_Set_Mode(0);
	osDelay(5000);
	BT_Set_MTU(243);
	//osDelay(2000);
}
//------------------- 蓝牙 Modbus 配置初始化 ----------------------------
void BT_Modbus_Config_Init(void)
{
	//初始化对象
	MsInit(&Ms_BT_Modbus,21,1,SerialWrite);
	//设置01寄存器的参数，不设置的话会返回无效的功能错误码
	//MsConfigureRegister(&Ms_BT_Modbus,0x01,buff01,sizeof(buff01));
	//MsConfigureRegister(&Ms_BT_Modbus,0x0F,buff01,sizeof(buff01));
	MsConfigureRegister(&Ms_BT_Modbus,0x03,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x04,Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,0),REG_INPUT_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x06,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x10,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x05,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	
}
/*******************************************************************************
*功能：超时计数，放到定时器中运行，一般放到1ms中断足够满足要求
*******************************************************************************/
void BT_MsTimeout(void)
{
	MsTimeout(&Ms_BT_Modbus);
}

//------------------- 设置wifi状态机 ----------------------------
void BT_Set_Machine_State(BT_STATE_MODE_E para)
{
	if(para <= BT_ERROR)
	{
		BT_State_Machine = para;
	}
}
	
//------------------- 获取wifi状态机 ----------------------------
BT_STATE_MODE_E BT_Get_Machine_State(void)
{
	return BT_State_Machine;
}


//------------------- 接收处理函数 ----------------------------
void BT_Read_Handler(void)
{	
	MsProcess(&Ms_BT_Modbus);
}

//------------------- 进入配网 ----------------------------
void BT_Get_In_Distribution(void)
{
	BT_Set_Machine_State( BT_DISTRIBUTION );
	
	BT_Module_AT_ReInit();
}

//------------------- 进入故障 ----------------------------
void BT_Get_In_Error(void)
{
	BT_Set_Machine_State( BT_ERROR );
	
}



