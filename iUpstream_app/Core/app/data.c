/**
******************************************************************************
* @file				data.c
* @brief			数据管理模块
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "data.h"
#include "tm1621.h"
#include "motor.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

Operating_Parameters OP_ShowNow;

Operating_Parameters* p_OP_ShowLater;

// 训练模式 当前状态
uint8_t PMode_Now = 0;

uint8_t Period_Now = 0;

// 各模式 属性 初始值
Operating_Parameters OP_Init_Free = { 60 , 0};

Operating_Parameters OP_Init_Timing = { 60 , 1800};

Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX] = {
{{20,60},{40,120},{60,180},{80,240},{100,300},{80,360},{80,420},{60,480},{40,540},{20,600},{20,660},{40,720}},
{{40,60},{80,120},{20,180},{60,240},{100,300},{40,360},{60,420},{80,480},{20,540},{60,600},{100,660},{40,720}},
{{60,60},{70,120},{80,180},{90,240},{100,300},{90,360},{80,420},{70,480},{60,540},{50,600},{40,660},{30,720}}};

	
// 各模式 属性
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;
	
uint16_t* p_System_Fault_Static;			//故障状态
uint16_t* p_Motor_Fault_Static;				//故障状态
uint16_t* p_Mos_Temperature;					//mos 温度
uint16_t* p_Motor_Temperature;				//电机 温度
uint32_t* p_Motor_Current;						//电机 电流
uint32_t* p_Motor_Reality_Speed;			//电机 实际 转速
uint16_t* p_Motor_Bus_Voltage;				//母线 电压
	
	
uint16_t* p_Modbus_Node_Addr;					//地址
uint16_t* p_Modbus_Baud_Rate;					//波特率
uint16_t* p_Support_Control_Methods;	//屏蔽控制方式
uint16_t* p_Motor_Pole_Number;				//电机极数
	
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 硬件 & 驱动 ----------------------------

// 初始化 开机执行
void Check_Data_Init(void)
{
	static uint8_t x=0,y=0;
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
	}
	*p_Modbus_Node_Addr = 21;
	*p_Modbus_Baud_Rate = 4;
	*p_Support_Control_Methods = 0;
	
	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	}
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				return;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					return;
				}
			}
		}
	}
	
	
}


extern void Get_Mapping_Register(void);
// 初始化
void App_Data_Init(void)
{
	// 获取映射  flash已读
	Get_Mapping_Register();
	
	// 训练模式 当前状态
	PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	Check_Data_Init();
	
	//存储  存一个 还是 扇区存
	Memset_OPMode();
	
	// 屏幕初始化
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
}


// 恢复 初始化
void App_Data_ReInit(void)
{
	// 训练模式 当前状态
	PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;
	
	*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode));
	
	//存储  存一个 还是 扇区存
	Memset_OPMode();
}

// 存 flash
extern void MB_Flash_Buffer_Write(void);
uint8_t Memset_OPMode(void)
{
	MB_Flash_Buffer_Write();
	return 1;
}

//存储 新属性
void Update_OP_Data(void)
{
	
	if(System_State_Machine <= FREE_MODE_STOP)	// 自由
	{
		p_OP_Free_Mode->speed = OP_ShowNow.speed;
	}
	else if(System_State_Machine <= TIMING_MODE_STOP)	// 定时
	{
		p_OP_Timing_Mode->speed = OP_ShowNow.speed;
		p_OP_Timing_Mode->time = OP_ShowNow.time;
	}
	else if(System_State_Machine <= TRAINING_MODE_STOP)	// 训练
	{
		if(Is_Mode_Legal(PMode_Now))
			p_OP_PMode[PMode_Now-1][Period_Now-1].speed = OP_ShowNow.speed;
	}
	
	//存储  存一个 还是 扇区存
	Memset_OPMode();
}

//------------------- 判断 模式 合法 ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode)
{
	if((mode > 0) && (mode <= TRAINING_MODE_NUMBER_MAX) )
	{
		return 1;
	}
	else
		return 0;
}
//------------------- 判断速度 合法 ----------------------------
uint8_t Is_Speed_Legal(uint16_t speed)
{
	if((speed >= SPEED_LEGAL_MIN) && (speed <= SPEED_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- 判断时间 合法 ----------------------------
uint8_t Is_Time_Legal(uint16_t time)
{
	if((time >= TIME_LEGAL_MIN) && (time <= TIME_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- 设置 当前 速度 ----------------------------
void Data_Set_Current_Speed(uint8_t speed)
{
	//if(System_is_Starting())
		//return;
	
	OP_ShowNow.speed = speed;	
	Motor_Speed_Target_Set(speed);
	
	if(System_is_Running())
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
}

//------------------- 设置 当前 时间 ----------------------------
void Data_Set_Current_Time(uint16_t time)
{
	if(System_is_Starting())
		return;
	
	OP_ShowNow.time = time;
}
