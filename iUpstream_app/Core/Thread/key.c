/**
******************************************************************************
* @file				key.c
* @brief			Key function implementation
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "tm1621.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

#define KEY_IO_NUMBER_MAX			4

//------------------- 按键 & 引脚 ----------------------------
#define KEY_SPEED_IO_PORT			GPIOC
#define KEY_SPEED_IO_PIN			GPIO_PIN_7

#define KEY_TIME_IO_PORT			GPIOC
#define KEY_TIME_IO_PIN				GPIO_PIN_8

#define KEY_TRAIN_IO_PORT			GPIOC
#define KEY_TRAIN_IO_PIN			GPIO_PIN_9

#define KEY_POWER_IO_PORT			GPIOA
#define KEY_POWER_IO_PIN			GPIO_PIN_8


//------------------- led & 引脚 ----------------------------
#define LED_SPEED_IO_PORT			GPIOC
#define LED_SPEED_IO_PIN			GPIO_PIN_1

#define LED_TIME_IO_PORT			GPIOC
#define LED_TIME_IO_PIN				GPIO_PIN_2

#define LED_TRAIN_IO_PORT			GPIOC
#define LED_TRAIN_IO_PIN			GPIO_PIN_3

#define LED_POWER_IO_PORT			GPIOC//GPIOA
#define LED_POWER_IO_PIN			GPIO_PIN_0//GPIO_PIN_1

/* Private variables ---------------------------------------------------------*/

// 按键对应 引脚
IO_Hardware_Pin Key_Gpio_Pin_Array[KEY_IO_NUMBER_MAX] = {
	{ KEY_SPEED_IO_PORT, KEY_SPEED_IO_PIN }, 
	{ KEY_TIME_IO_PORT, 	KEY_TIME_IO_PIN }, 
	{ KEY_TRAIN_IO_PORT, KEY_TRAIN_IO_PIN }, 
	{ KEY_POWER_IO_PORT, KEY_POWER_IO_PIN }
};

// 按键灯对应 引脚
uint8_t Key_IO_Ordering_Value[KEY_CALL_OUT_NUMBER_MAX]={
	KEY_VALUE_BIT_BUTTON_1,
	KEY_VALUE_BIT_BUTTON_2,
	KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_4,
	KEY_VALUE_BIT_BUTTON_1|KEY_VALUE_BIT_BUTTON_2,
	KEY_VALUE_BIT_BUTTON_1|KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_2|KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_2|KEY_VALUE_BIT_BUTTON_4,
};

// 短按 槽函数
void (*p_Funtion_Button[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_clicked,  on_pushButton_2_clicked,  on_pushButton_3_clicked,  on_pushButton_4_Short_Press,
	on_pushButton_1_2_Short_Press, on_pushButton_1_3_Short_Press, 
	on_pushButton_2_3_Short_Press, on_pushButton_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Funtion_Long_Press[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_1_Long_Press,  on_pushButton_2_Long_Press,  on_pushButton_3_Long_Press,  on_pushButton_4_Long_Press,
	on_pushButton_1_2_Long_Press, on_pushButton_1_3_Long_Press,
	on_pushButton_2_3_Long_Press, on_pushButton_2_4_Long_Press,
};

// 各槽 长按判断 时长
uint32_t Key_Long_Press_Confirm_Value[KEY_CALL_OUT_NUMBER_MAX]={
	KEY_LONG_PRESS_TIME_SHORT, KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME,
	KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME,KEY_LONG_PRESS_TIME,KEY_LONG_PRESS_TIME,
};



//------------------- 变量 声明 ----------------------------
uint8_t Key_IO_Hardware = 0;		// 读取key 值
uint8_t Key_IO_Old = 0;					// 上一次 key 值

uint32_t Key_Long_Press_cnt[KEY_CALL_OUT_NUMBER_MAX]={0};	// 长按 计数器



/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 按键回调 ----------------------------

// ① 档位键
void on_pushButton_clicked(void)
{
	if((System_State_Machine == POWER_OFF_STATUS) || System_is_Pause() || System_is_Stop())
			return;

	Clean_Timing_Timer_Cnt();
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(OP_ShowNow.speed++ > 100)
			OP_ShowNow.speed = 20;
		
		//if(Motor_is_Start())
		if(System_is_Running())
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		else if(System_is_Starting())
			OP_ShowNow.time = p_OP_ShowLater->time;
		Arbitrarily_To_Initial();
		Lcd_Show();
	}
	else
	{
		if((OP_ShowNow.speed % 20) != 0)
			OP_ShowNow.speed += (20-(OP_ShowNow.speed % 20));
		else
		OP_ShowNow.speed += 20;
		
		if(OP_ShowNow.speed > 100)
			OP_ShowNow.speed = 20;
		
		//if(Motor_is_Start())
		if(System_is_Running())
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		else if(System_is_Starting())
			OP_ShowNow.time = p_OP_ShowLater->time;
		Arbitrarily_To_Initial();
		Lcd_Show();
	}
}

// ② 时间键
void on_pushButton_2_clicked(void)
{
	Clean_Timing_Timer_Cnt();
	if(System_State_Machine == TIMING_MODE_INITIAL)
	{
			if(OP_ShowNow.time >= 5400)
					OP_ShowNow.time = 900;
			else
			{
					if((OP_ShowNow.time % 900) != 0)
							OP_ShowNow.time += (900-(OP_ShowNow.time % 900));
					else
							OP_ShowNow.time += 900;
			}
			Lcd_Show();
	}
	else
	{
			To_Timing_Mode();
			Fun_Change_Mode();
	}
	Arbitrarily_To_Initial();
}

// ③ 模式键
void on_pushButton_3_clicked(void)
{
	Clean_Timing_Timer_Cnt();
	if(System_State_Machine == TRAINING_MODE_INITIAL)
	{
			if(PMode_Now >= 3)
			{
					To_Free_Mode(0);
			}
			else
			{
					To_Train_Mode((PMode_Now+1));
			}
	}
	else
	{
			To_Train_Mode(1);
			Fun_Change_Mode();
	}

	Period_Now = 0;
	Arbitrarily_To_Initial();
}

// ④ 开机键  短按
void on_pushButton_4_Short_Press(void)
{
	Clean_Timing_Timer_Cnt();
	if(System_State_Machine == POWER_OFF_STATUS)//关机中 执行开机
	{
			System_Power_On();
			return;
	}

	if(System_is_Pause())	// 暂停 --> 启动
	{
		Arbitrarily_To_Starting();
		OP_ShowNow.speed = p_OP_ShowLater->speed;
	}
	else if(System_is_Initial())	// 初始 --> 启动
	{
		Arbitrarily_To_Starting();
		*p_OP_ShowLater = OP_ShowNow;
	}
	else	// 其它 --> 暂停
	{
		Arbitrarily_To_Pause();
		*p_OP_ShowLater = OP_ShowNow;
		OP_ShowNow.speed = 0;
	}
	
	OP_ShowNow.time = 20;
	Lcd_Show();
}

// ① + ②  组合键  测试 设置故障
void on_pushButton_1_2_Short_Press(void)
{
	Set_Fault_Data(0xAC);
}

// ① + ③  组合键  短按   切换档位 100级 or 5级
void on_pushButton_1_3_Short_Press(void)
{
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
		Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
	else
		Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
}

// ② + ③  组合键  恢复出厂设置
void on_pushButton_2_3_Short_Press(void)
{
	Restore_Factory_Settings();
}

// ② + ④  组合键  进入操作菜单
void on_pushButton_2_4_Short_Press(void)
{
	To_Operation_Menu();
}


//------------------- 按键回调   长按 ----------------------------

void on_pushButton_1_Long_Press(void)
{
	if((System_State_Machine == POWER_OFF_STATUS) || System_is_Pause() || System_is_Stop())
			return;
	
	if(Key_Long_Press_cnt[0] == KEY_LONG_PRESS_TIME)
	{
		//长按可设 高精度转速
		if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR) == 0)
			Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
	}
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(OP_ShowNow.speed++ >= 100)
					OP_ShowNow.speed = 20;
		if(System_is_Running())
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		else if(System_is_Starting())
			OP_ShowNow.time = p_OP_ShowLater->time;
		Arbitrarily_To_Initial();
		Lcd_Show();
	}

}

void on_pushButton_2_Long_Press(void)
{
	Set_Fault_Data(0);
}

void on_pushButton_3_Long_Press(void)
{
}

//关机
void on_pushButton_4_Long_Press(void)
{
    System_Power_Off();
}

// 关机下  参数设置
// 停机下  wifi配对
void on_pushButton_1_2_Long_Press(void)
{
	if(Motor_is_Start() ==0)
	{
		BT_Get_In_Distribution();
	}
}

//
void on_pushButton_1_3_Long_Press(void)
{
}

// 关机下  查看版本号
// 停机下  蓝牙配对
void on_pushButton_2_3_Long_Press(void)
{
	if(System_is_Power_Off())
	{
		To_Operation_Menu();
	}
	else if(Motor_is_Start() ==0)
	{
		WIFI_Get_In_Distribution();
	}
}

//  关机下   恢复出厂
void on_pushButton_2_4_Long_Press(void)
{
	if(System_is_Power_Off())
	{
		Restore_Factory_Settings();
	}
}

//------------------- 硬件 & 驱动 ----------------------------
// 初始化
void App_Key_Init(void)
{
}

//------------------- 按键灯 ----------------------------
// 按键灯
void Led_Button_On(uint8_t para)
{
	if(para & 1<<0)
		HAL_GPIO_WritePin(LED_SPEED_IO_PORT, LED_SPEED_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_SPEED_IO_PORT, LED_SPEED_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<1)
		HAL_GPIO_WritePin(LED_TIME_IO_PORT, LED_TIME_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_TIME_IO_PORT, LED_TIME_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<2)
		HAL_GPIO_WritePin(LED_TRAIN_IO_PORT, LED_TRAIN_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_TRAIN_IO_PORT, LED_TRAIN_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<3)
		HAL_GPIO_WritePin(LED_POWER_IO_PORT, LED_POWER_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_POWER_IO_PORT, LED_POWER_IO_PIN, GPIO_PIN_RESET);	// 0		
	
}


// 按键主循环任务
//  200 ms
void App_Key_Handler(void)
{
	uint8_t i;
	
	Key_IO_Hardware = Key_Get_IO_Input();
	
	for(i=0; i<KEY_CALL_OUT_NUMBER_MAX; i++)
	{
		if(Key_IO_Hardware == Key_IO_Ordering_Value[i])
		{
			if(Key_IO_Old & 1<<i)
			{
				if(++Key_Long_Press_cnt[i] >= KEY_LONG_PRESS_TIME)//长按
				{
					if(OPERATION_MENU_STATUS == System_State_Machine)
						p_Operation_Long_Press[i]();
					else if(ERROR_DISPLAY_STATUS == System_State_Machine)
						p_Fault_Long_Press[i]();
					else
						p_Funtion_Long_Press[i]();
				}
			}
			else
			{
				if(Key_Long_Press_cnt[i] == 0)
					TM1621_Buzzer_Click();
				
				if(OPERATION_MENU_STATUS == System_State_Machine)
					p_Operation_Button[i]();
				else if(ERROR_DISPLAY_STATUS == System_State_Machine)
					p_Fault_Button[i]();
				else
					p_Funtion_Button[i]();
				
				Key_Long_Press_cnt[i] = 0;
			}
		}
		else
		{
			Key_Long_Press_cnt[i] = 0;
		}
	}
	
	Key_IO_Old = Key_IO_Hardware;
	
}


// 获取按键
uint8_t Key_Get_IO_Input(void)
{
	uint8_t result=0;
	GPIO_PinState read_io;
	uint8_t i;
	
	result = Key_Form_Usart;
	Key_Form_Usart = 0;
	for(i=0; i<KEY_IO_NUMBER_MAX; i++) // KEY_IO_NUMBER_MAX
	{
		read_io = HAL_GPIO_ReadPin(Key_Gpio_Pin_Array[i].port, Key_Gpio_Pin_Array[i].pin);
		if(read_io == 0)//低有效
			result |= (1<<i);
	}
	
	if(System_State_Machine == POWER_OFF_STATUS)
	{
		result &= KEY_VALUE_BIT_BUTTON_4;
	}
	
	return result;
}


//------------------- 功能接口 ----------------------------

//	开机 进入自由模式
void System_Power_On(void)
{
	// 检查 属性
	Check_Data_Init();
	// 寄存器
	Set_DataAddr_Value( MB_FUNC_READ_HOLDING_REGISTER, MB_SYSTEM_POWER_ON, 1);
	
	//	状态
	To_Free_Mode(0);				// ui
	
	Led_Button_On(0x0F);	// 按键
	
	//
	
	// 后台定时器
	//BlackGround_Task_On();
}

//	关机
void System_Power_Off(void)
{
	To_Power_Off();
	// 寄存器
	Set_DataAddr_Value( MB_FUNC_READ_HOLDING_REGISTER, MB_SYSTEM_POWER_ON, 0);
	Led_Button_On(0x08);	// 按键
	
	// 后台定时器
	//BlackGround_Task_Off();
}

//	恢复出厂设置
void Restore_Factory_Settings(void)
{
	// data 恢复
	App_Data_ReInit();
	
	// wifi 恢复
	
	// 返回 自由模式 初始状态
	To_Free_Mode(1);
	
}





