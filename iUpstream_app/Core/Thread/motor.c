/**
******************************************************************************
* @file				motor.c
* @brief			电机 相关协议  控制转速命令 200ms
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "motor.h"
#include "debug_protocol.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/

#if (MOTOR_MODULE_HUART == 3)//驱动板串口 UART句柄
UART_HandleTypeDef* p_huart_motor = &huart3;
#elif (MOTOR_MODULE_HUART == 1)
UART_HandleTypeDef* p_huart_motor = &huart1;
#elif (MOTOR_MODULE_HUART == 4)
UART_HandleTypeDef* p_huart_motor = &huart4;
#endif

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

//**************** 收发缓冲区
uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE]={0};//定义一个接收缓存区
uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE]={0};//定义一个发送缓存区

uint8_t Motor_Speed_Now = 0;			// 电机转速 

uint16_t Motor_Timer_Cnt=0;

uint8_t Motor_Heartbeat_Cnt=0;

/* Private user code ---------------------------------------------------------*/

// 初始化
void Metering_Receive_Init(void)
{
	//__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_RXNE); //使能IDLE中断
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_IDLE);//使能idle中断
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_ERR);//
	
  HAL_UART_Receive_DMA(p_huart_motor,Motor_DMABuff,MOTOR_RS485_RX_BUFF_SIZE);//打开串口DMA接收
}

//------------------- 主循环函数  ----------------------------
void App_Motor_Handler(void)
{
	//读状态  2s
	if((Motor_Timer_Cnt % 40)==0)
	{
		Motor_Read_Register();
	}
	else
	{
		if(Motor_Speed_Update() == 0)
		{
			//心跳 50ms
			Motor_Heartbeat_Send();
		}
	}
	if(Motor_Timer_Cnt < 10000)
		Motor_Timer_Cnt ++;
	else
		Motor_Timer_Cnt = 0;
}


//------------------- 同步电机转速 ----------------------------
uint8_t Motor_Speed_Update(void)
{
	if( Motor_Speed_Now == Motor_Speed_Target )
	{
		return 0;
	}
	
	if( Motor_Speed_Now > Motor_Speed_Target )
		Motor_Speed_Now --;
	else if( Motor_Speed_Now < Motor_Speed_Target )
		Motor_Speed_Now ++;
	
	//发送
	Motor_Speed_Send(Motor_Speed_To_Rpm(Motor_Speed_Now));
	return 1;
}

//------------------- 电机转速是否达到目标值 ----------------------------
uint8_t Motor_Speed_Is_Reach(void)
{
	if(Motor_Speed_Now == Motor_Speed_Target)
		return 1;
	else
		return 0;
}

//------------------- 百分比功率 转 电机转速 ----------------------------
// 100% -->  2100 rpm
uint32_t Motor_Speed_To_Rpm(uint8_t speed)
{
	uint32_t speed_rpm = speed*MOTOR_RPM_CONVERSION_COEFFICIENT;
	
	return speed_rpm;
}


/*-------------------- 收发处理 ----------------------------------------------*/
uint16_t CRC16_XMODEM_T(uint8_t *ptr, uint16_t len)
{
    const uint16_t crc_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };

    uint16_t crc = 0x0000;
    
    while(len--) 
    {
        crc = (crc << 8) ^ crc_table[(crc >> 8 ^ *ptr++) & 0xff];
    }
    
    return(crc);
}

//-------------------- 发送 ----------------------------
void Motor_UART_Send(uint8_t* p_buff, uint8_t len)
{
#ifdef MOTOR_MODULE_HUART
	HAL_UART_Transmit(p_huart_motor, p_buff, len, 0xFFFF);
	//HAL_UART_Transmit_IT(p_huart_motor, p_buff, len);//DMA发送
#endif
}
//-------------------- 接收 ----------------------------
void Motor_RxData(uint8_t len)
{
	//
	uint8_t debug_send_buffer[DEBUG_PROTOCOL_TX_MAX]={0};
	// 滤波后的mosfet温度
	uint16_t mosfet_tmp = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+0]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+1];
	// 滤波后的电机温度
	uint16_t motor_tmp = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+2]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+3];
	// 电机平均电流
	uint32_t mosfet_current = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+4]<<24 |Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+5]<<16 |Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+6]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+7];
	// 当前电气转速erpm
	uint32_t motor_speed = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+22]<<24 |Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+23]<<16 |Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+24]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+25];
	// 母线电压
	uint16_t bus_voltage = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+26]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+27];
	// 获取电机故障
	uint8_t motor_fault = Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+52];
	// 10KNTC温度1 2 3
	uint16_t ntc_tmp[3] = {Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+58]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+59],Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+60]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+61],Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+62]<<8 | Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET+63]};
	
	if(motor_fault)
		*p_System_Fault_Static |= 0x08;
	
	sprintf((char*)debug_send_buffer,"\n\n\nmosfet温度:\t%d.%d\n电机温度:\t%d.%d\n电机电流:\t%d.%d\n转速:\t\t%d\n母线电压:\t%d.%d\n电机故障:\t%d\n10KNTC温度1 2 3:\t%d.%d\t%d.%d\t%d.%d\n\n",
	
	mosfet_tmp/10,mosfet_tmp%10,motor_tmp/10,motor_tmp%10,mosfet_current/100,mosfet_current%100,
	motor_speed,bus_voltage/10,bus_voltage%10,motor_fault,
	ntc_tmp[0]/10,ntc_tmp[0]%10,ntc_tmp[1]/10,ntc_tmp[1]%10,ntc_tmp[2]/10,ntc_tmp[2]%10);
	
	
	UART_Send_Debug(debug_send_buffer,strlen((char*)debug_send_buffer));
	
	
	
	memset(Motor_DMABuff,0,MOTOR_RS485_RX_BUFF_SIZE);    				//清空缓存区
	__HAL_UART_CLEAR_IDLEFLAG(p_huart_motor);               //清除标志位
	HAL_UART_Receive_DMA(p_huart_motor,Motor_DMABuff,MOTOR_RS485_RX_BUFF_SIZE);  //开DMA接收，数据存入rx_buffer数组中。
}



/*-------------------- 快速发送命令 ----------------------------------------------*/
//------------------- 设置电机转速 ----------------------------
void Motor_Speed_Send(uint32_t speed_rpm)
{
	uint8_t buffer[10]={0x02,0x05,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x03};
	uint16_t crc_value=0;
	
	buffer[3] = speed_rpm>>24;
	buffer[4] = speed_rpm>>16;
	buffer[5] = speed_rpm>>8;
	buffer[6] = speed_rpm & 0xFF;
	
	//crc
	
	crc_value = CRC16_XMODEM_T(&buffer[2], 5 );
	
	buffer[7] = crc_value>>8;
	buffer[8] = crc_value & 0xFF;
	
	Motor_UART_Send(buffer, 10);
}


//------------------- 心跳 ----------------------------
void Motor_Heartbeat_Send(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//------------------- 读故障状态 ----------------------------
void Motor_ReadFault_Send(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读寄存器 ( 全读 )----------------------------
void Motor_Read_Register(void)
{
	uint8_t buffer[10]={0x02,0x01,0x04,0x40,0x84,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读 软硬件版本 ----------------------------
void Motor_Read_Version(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读 故障 ----------------------------
void Motor_Read_Fault(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}


