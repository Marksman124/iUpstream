/**
******************************************************************************
* @file    		modbus.c
* @brief   		Modbus 接口
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "modbus.h"
#include "dev.h"
/* ----------------------- Defines ------------------------------------------*/
// 03
#define REG_HOLDING_START               ( MB_SLAVE_NODE_ADDRESS )
#define REG_HOLDING_NREGS               ( MB_USER_TRAIN_MODE_TIME_P4_50 )

// 04
#define REG_INPUT_START 								( MB_DISPLAY_SOFTWARE_VERSION )
#define REG_INPUT_NREGS 								( MB_SYSTEM_FAULT_STATUS )

// 21
#define REG_FILE_NUMBER_MAX 								( 0x270F )
#define REG_FILE_NUMBER_STAR 								( 0 )
#define REG_FILE_NUMBER_END 								( 0xFFFF )
#define REG_FILE_LENTH_MAX 									( 0x77 )

#define MODBUS_RESTART_TIMEOUT 							( 10 )					// 100MS * N

/* Private variables ---------------------------------------------------------*/
ULONG OTA_Pack_Len=0;					// 总长度

uint32_t ModbusTimerCnt=0;



/* ----------------------- Static variables ---------------------------------*/
//USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS+1];

//USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS+1];

USHORT		MB_Data_Addr_Need_CallOut[] = {
	MB_SLAVE_NODE_ADDRESS,MB_SLAVE_BAUD_RATE,MB_DISTRIBUTION_NETWORK_CONTROL,
	MB_SYSTEM_WORKING_MODE,MB_SYSTEM_WORKING_STATUS,MB_SYSTEM_POWER_ON,
	MB_MOTOR_CURRENT_SPEED,MB_MOTOR_CURRENT_TIME,
};

BOOL Dmx512_Data_Change_Sign =0;
/* ----------------------- Start implementation -----------------------------*/
extern TIM_HandleTypeDef htim1;

void Clean_ModbusTimerCnt()
{
	ModbusTimerCnt = 0;
}

BOOL 
Check_Need_CallOut(USHORT addr)
{
	USHORT sum,i;
	
	sum = sizeof(MB_Data_Addr_Need_CallOut)/sizeof(MB_Data_Addr_Need_CallOut[0]);
	
	for(i=0; i<sum; i++)
	{
		if(addr == MB_Data_Addr_Need_CallOut[i])
			return TRUE;
	}
	
	return FALSE;
}

// Call Out
void
HoldingCallOut( USHORT usAddress )
{
	//uint16_t iRegIndex;
	//uint32_t message_info=0;
	
	//iRegIndex = ( usAddress );
	
	if(usAddress == MB_SLAVE_NODE_ADDRESS) // 从机地址
	{
		Modbus_Init();
	}
	else if(usAddress == MB_SLAVE_BAUD_RATE) // 设置波特率
	{
		__HAL_UART_DISABLE(p_huart_mb);
#if MODBUS_USART == 1
		MX_USART1_UART_Init();
#elif MODBUS_USART == 3
		MX_USART3_UART_Init();
#elif MODBUS_USART == 4
		MX_UART4_Init();
#elif MODBUS_USART == 5
		MX_UART5_Init();
#endif
		__HAL_UART_ENABLE(p_huart_mb);
	}
	else if(usAddress == MB_DISTRIBUTION_NETWORK_CONTROL) //	配网控制 遥控 & wifi
	{
		Ctrl_GetIn_Distribution(usRegHoldingBuf[MB_DISTRIBUTION_NETWORK_CONTROL]);
	}
	else if(usAddress == MB_SYSTEM_WORKING_MODE) //	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
	{
		Ctrl_Set_System_Mode(usRegHoldingBuf[MB_SYSTEM_WORKING_MODE]);
	}
	else if(usAddress == MB_SYSTEM_WORKING_STATUS) //	系统工作状态  0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
	{
		Ctrl_Set_System_Status(usRegHoldingBuf[MB_SYSTEM_WORKING_STATUS]);
	}
	else if(usAddress == MB_SYSTEM_POWER_ON) //	系统开机 0：关机  1：开机
	{
		Ctrl_Set_System_PowerOn(usRegHoldingBuf[MB_SYSTEM_POWER_ON]);
	}
	else if(usAddress == MB_MOTOR_CURRENT_SPEED) //	当前转速 (临时有效)
	{
		Ctrl_Set_Motor_Current_Speed(usRegHoldingBuf[MB_MOTOR_CURRENT_SPEED]);
	}
	else if(usAddress == MB_MOTOR_CURRENT_TIME) ///	当前时间 (临时有效)
	{
		Ctrl_Set_Motor_Current_Time(usRegHoldingBuf[MB_MOTOR_CURRENT_TIME]);
	}
	
	//扇区是2048， 整个 usRegHoldingBuf 一起写
	//STMFLASH_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

	Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS )
    {
			taskENTER_CRITICAL();
        //iRegIndex = ( int )( usAddress - usRegInputStart );
				iRegIndex = ( int )( usAddress );
        while( usNRegs > 0 )
        {
					*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
					*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
					iRegIndex++;
					usNRegs--;
        }
				taskEXIT_CRITICAL();
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
	
		Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) 
    {
				//iRegIndex = ( int )( usAddress - usRegHoldingStart );
        iRegIndex = ( int )( usAddress );
        switch ( eMode )
        {
        case MB_REG_READ:
					taskENTER_CRITICAL();
            while( usNRegs > 0 )
            {
							*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
							*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
              iRegIndex++;
              usNRegs--;
            }
						taskEXIT_CRITICAL();
            break;

        case MB_REG_WRITE:
					taskENTER_CRITICAL();
            while( usNRegs > 0 )
            {
							usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
							usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
							iRegIndex++;
              usNRegs--;
            }
						taskEXIT_CRITICAL();
						if(Check_Need_CallOut(usAddress))
						{
							HoldingCallOut(usAddress);
						}
						break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegFileCB( UCHAR * pucRegBuffer, USHORT fileNumber, USHORT fileLength,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
	ULONG write_addr=0;						// flash 写入地址
	ULONG sign=0;
	
	Clean_ModbusTimerCnt();
	
	if( (( fileNumber <= REG_FILE_NUMBER_MAX ) || ( fileNumber == REG_FILE_NUMBER_END )) && ( fileLength <= REG_FILE_LENTH_MAX ) )
	{
		switch ( eMode )
		{
		case MB_REG_READ:
			break;
		case MB_REG_WRITE:
			if(fileNumber == REG_FILE_NUMBER_STAR) // 起始包
			{OTA_Pack_Len = 0;}
			
			taskENTER_CRITICAL();
			write_addr = (FLASH_APP_PATCH_ADDR + OTA_Pack_Len);
			iap_write_appbin(write_addr,pucRegBuffer,fileLength);
			OTA_Pack_Len += (fileLength*2);
			taskEXIT_CRITICAL();
			
			if(fileNumber == REG_FILE_NUMBER_END)//最后一帧
			{
				//STMFLASH_Write(USER_FLASH_ADDR_SOFTWARE_VERSION,&fileNumber,1); // 写版本信息 
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PACK_LEN,(uint16_t*)&OTA_Pack_Len,2); // 写包长度 (含crc)
				sign = PRODUCT_BOOT_PASSWORD;
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2); // 进入OTA 

				SysSoftReset();// 软件复位
			}
			break;
		}
	}
	else
	{
			eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	Clean_ModbusTimerCnt();
	//错误状态
	eMBErrorCode eStatus = MB_ENOERR;
//	//寄存器个数
//	int16_t iNCoils = ( int16_t )usNCoils;
//	//寄存器偏移量
//	int16_t usBitOffset;

//	//检查寄存器是否在指定范围内
//	if( ( (int16_t)usAddress >= REG_COILS_START ) &&
//	( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
//	{
//	//计算寄存器偏移量
//	usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
//	switch ( eMode )
//	{
//	//读操作
//	case MB_REG_READ:
//	while( iNCoils > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
//	iNCoils -= 8;
//	usBitOffset += 8;
//	}
//	break;

//	//写操作
//	case MB_REG_WRITE:
//	while( iNCoils > 0 )
//	{
//	xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
//	*pucRegBuffer++ );
//	iNCoils -= 8;
//	}
//	break;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	Clean_ModbusTimerCnt();
	//错误状态
	eMBErrorCode eStatus = MB_ENOERR;
//	//操作寄存器个数
//	int16_t iNDiscrete = ( int16_t )usNDiscrete;
//	//偏移量
//	uint16_t usBitOffset;

//	//判断寄存器时候再制定范围内
//	if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
//	( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
//	{
//	//获得偏移量
//	usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

//	while( iNDiscrete > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
//	( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
//	iNDiscrete -= 8;
//	usBitOffset += 8;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}




void Modbus_Init(void)
{
	// 先写死 0xAA  wuqingguang
	//if( (*p_Local_Address >= 0xFF) || (*p_Local_Address == 0) )
	{
		*p_Local_Address = MODBUS_LOCAL_ADDRESS;	// 默认 0xAA
	}
	eMBInit( MB_RTU, *p_Local_Address, 0, 115200, MB_PAR_ODD);//初始化modbus，走modbusRTU，从站地址为0xAA，串口为2。
	eMBEnable(  );//使能modbus
	
}

void Modbus_Handle_Task(void)
{
	( void )eMBPoll(  );//启动modbus侦听
	
	if(ModbusTimerCnt++ > MODBUS_RESTART_TIMEOUT)
	{
		ModbusTimerCnt = 0;
		vMBPortSerialEnable( TRUE, FALSE );
	}
}



// *********  Buf 相关数据接口  *************************************

void Modbus_Buffer_Init(void)
{
	STMFLASH_Read(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
	
	// 默认波特率  测试用
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_SLAVE_BAUD_RATE,3);
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_SLAVE_NODE_ADDRESS,3);
	
}

void MB_Flash_Buffer_Write(void)
{
//扇区是2048， 整个 usRegHoldingBuf 一起写
	//STMFLASH_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}



uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return &usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return &usRegInputBuf[addr];
		}
	}
	
	return NULL;
}


uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return usRegInputBuf[addr];
		}
	}
	
	return NULL;
}

void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			usRegHoldingBuf[addr] = value;
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			usRegInputBuf[addr] = value;
		}
	}
}

void Set_DataValue_U32(UCHAR ucFunctionCode, USHORT addr, uint32_t value)
{
	//taskENTER_CRITICAL();
	Set_DataAddr_Value( ucFunctionCode, addr, value>>16 );
	Set_DataAddr_Value( ucFunctionCode, addr+1, value&0xFFFF );
	//taskEXIT_CRITICAL();
}



void Get_Mapping_Register(void)
{
	p_OP_ShowLater = 		(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_CURRENT_SPEED);
	p_OP_Free_Mode = 		(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_FREE_MODE_SPEED);
	p_OP_Timing_Mode = 	(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TIME_MODE_SPEED);
	p_OP_PMode =				(Operating_Parameters(*)[TRAINING_MODE_PERIOD_MAX])Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P1_1);
	p_System_Fault_Static = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER , MB_SYSTEM_FAULT_STATUS);
	
}



