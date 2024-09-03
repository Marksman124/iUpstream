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
#include "modbus.h"
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
Operating_Parameters OP_Init_Free = { 40 , 0};

Operating_Parameters OP_Init_Timing = { 40 , 1800};

Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX] = {
{{20,120},{30,300}, {20,360},{35,540},{20,600},{30,780}, {20,900} },
{{45,180},{55,360}, {45,480},{70,720},{45,780},{55,1020},{45,1200}},
{{70,300},{80,540}, {70,600},{85,840},{70,900},{80,1200},{70,1500}},
{{45,420},{65,1440},{45,1800}},
{{30,00}},
};

//--------------------------- 系统属性
uint16_t* p_System_State_Machine;			// 状态机
uint16_t* p_PMode_Now;								// 当前模式
uint16_t* p_OP_ShowNow_Speed;					// 当前速度
uint16_t* p_OP_ShowNow_Time;					// 当前时间


// 各模式 属性
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;

uint16_t* p_Driver_Software_Version;			//驱动板软件版本

uint16_t* p_System_Fault_Static;			//故障状态
uint16_t* p_Motor_Fault_Static;				//故障状态
uint16_t* p_Mos_Temperature;					//mos 温度
uint16_t* p_Box_Temperature;					//电箱 温度
uint32_t* p_Motor_Current;						//电机 电流
uint32_t* p_Motor_Reality_Speed;			//电机 实际 转速
uint16_t* p_Motor_Bus_Voltage;				//母线 电压
	
uint16_t* p_Modbus_Node_Addr;					//地址
uint16_t* p_Modbus_Baud_Rate;					//波特率
uint16_t* p_Support_Control_Methods;	//屏蔽控制方式
uint16_t* p_Motor_Pole_Number;				//电机极数
uint16_t* p_Breath_Light_Max;					//光圈亮度  
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//电机状态

//================= 临时变量  全局 ================================
uint16_t Temp_Data_P5_Acceleration = 2;				//P5 加速度
uint16_t Temp_Data_P5_100_Time = 15;					//P5 100% 时间	秒
uint16_t Temp_Data_P5_0_Time = 15;						//P5 0% 	时间	秒

uint8_t WIFI_Rssi = 0xFF;

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
	//*p_Modbus_Node_Addr = 21;
	//*p_Modbus_Baud_Rate = 4;
	//*p_Support_Control_Methods = 0;
	
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
	Read_OPMode();
	// 获取映射  flash已读
	Get_Mapping_Register();
	
	// 训练模式 当前状态
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	Check_Data_Init();
	
	//存储  存一个 还是 扇区存
	//Memset_OPMode();
	
	// 屏幕初始化
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
	
	//测试代码 记得删
	TM1621_light_Off();
}


// 恢复 初始化
void App_Data_ReInit(void)
{
	// 训练模式 当前状态
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;
	
	*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode));
	
	//存储  存一个 还是 扇区存
	Memset_OPMode();
}

// 读 flash
uint8_t Read_OPMode(void)
{
	MB_Flash_Buffer_Read();
	return 1;
}

// 存 flash
uint8_t Memset_OPMode(void)
{
	MB_Flash_Buffer_Write();
	return 1;
}

//存储 新属性
void Update_OP_Data(void)
{
	
	if(*p_System_State_Machine <= FREE_MODE_STOP)	// 自由
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
	}
	else if(*p_System_State_Machine <= TIMING_MODE_STOP)	// 定时
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
	}
//	else if(*p_System_State_Machine <= TRAINING_MODE_STOP)	// 训练  训练不保存
//	{
//		if(Is_Mode_Legal(*p_PMode_Now))
//			p_OP_PMode[*p_PMode_Now-1][Period_Now-1].speed = *p_OP_ShowNow_Speed;
//	}

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
	
	*p_OP_ShowNow_Speed = speed;	
	Motor_Speed_Target_Set(speed);
	
	//if(System_is_Running())
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
}

//------------------- 设置 当前 时间 ----------------------------
void Data_Set_Current_Time(uint16_t time)
{
	//if(System_is_Starting())
		//return;
	
	*p_OP_ShowNow_Time = time;
}

//------------------- 设置 训练时段 ----------------------------
void Set_Pmode_Period_Now(uint16_t value)
{
	Period_Now = value;
}


//------------------- 是否接收外部控制 ----------------------------
uint8_t If_Accept_External_Control(void)
{
	if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
		return 1;
	else
		return 0;
}
