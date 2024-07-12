/**
******************************************************************************
* @file				operation.c
* @brief			操作模块 用于隐藏操作 单独出来便于屏蔽\删除
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-13
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "operation.h"
#include "tm1621.h"

/* Private includes ----------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
extern void To_Free_Mode(uint8_t mode);

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 上
void on_Button_1_clicked(void);
// ② 下
void on_Button_2_clicked(void);
// ③ 确认
void on_Button_3_clicked(void);
// ④ 取消
void on_Button_4_Short_Press(void);
void on_Button_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
void on_Button_1_3_Short_Press(void);
// ② + ③  组合键  短按
void on_Button_2_3_Short_Press(void);
// ② + ④  组合键  短按
void on_Button_2_4_Short_Press(void);
//--------------- 长按 -----------------------
// 长按
void on_Button_1_Long_Press(void);
void on_Button_2_Long_Press(void);
void on_Button_3_Long_Press(void);
void on_Button_4_Long_Press(void);
void on_Button_1_2_Long_Press(void);
void on_Button_1_3_Long_Press(void);
void on_Button_2_3_Long_Press(void);
void on_Button_2_4_Long_Press(void);

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

UART_HandleTypeDef* p_huart_operation = &huart3;		 //调试串口 UART句柄

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static uint8_t Operation_State_Machine = 0;		//	状态机	

static uint8_t Operation_Menu_Value = 0;		//	菜单值	


static uint16_t Operation_Addr_Value = 0;		//	地址	
static uint16_t Operation_Baud_Rate = 0;			//	波特率	
static uint16_t Operation_Shield_Value = 0;		//	控制方式	
static uint16_t Operation_Motor_Poles = 0;		//	控制方式	

// 发送缓冲区
uint8_t operation_send_buffer[24] = {0};


// 短按 槽函数
void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_clicked,  on_Button_2_clicked,  on_Button_3_clicked,  on_Button_4_Short_Press,
	on_Button_1_2_Short_Press, on_Button_1_3_Short_Press,
	on_Button_2_3_Short_Press, on_Button_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_Long_Press,  on_Button_2_Long_Press,  on_Button_3_Long_Press,  on_Button_4_Long_Press,
	on_Button_1_2_Long_Press, on_Button_1_3_Long_Press,
	on_Button_2_3_Long_Press, on_Button_2_4_Long_Press,
};


static uint32_t button_cnt=0;
/* Private user code ---------------------------------------------------------*/


//测试用
extern TIM_HandleTypeDef htim1;


// 初始化
void App_Operation_Init(void)
{
	Operation_Addr_Value = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS );
	
	Operation_Baud_Rate = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE );

	Operation_Shield_Value = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS );
		
	Operation_Motor_Poles = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_POLE_NUMBER );
}

//static void _Delay(uint32_t mdelay)
//{
//	__HAL_TIM_SET_COUNTER(&htim1,0);
//	while(__HAL_TIM_GET_COUNTER(&htim1) < mdelay);
//}

//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Number	

显示当前故障编号， 0-100
******************************************************************************
*/  
void Display_Oper_Number(uint8_t no)
{
	if(no > 100)
		no = 100;
	
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(no));
	TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, GET_NUMBER_TEN_DIGIT(no));
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, GET_NUMBER_ONE_DIGIT(no));
	
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_FaultCode

显示参数值
******************************************************************************
*/  
void Display_Oper_value(uint16_t value)
{
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH,  (value / 1000)%10);
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  	(value / 100)%10);
	
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  	(value / 10)&10);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  	(value % 10));
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Sum

显示故障总数
******************************************************************************
*/  
void Display_Mode_Hide(void)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  0xFF);
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  0xFF);
	
	//TM1621_LCD_Redraw();
}

/***********************************************************************
*		显示 函数总入口
*
*
***********************************************************************/
void Lcd_Show_Operation(uint8_t type, uint16_t num)
{
	if(System_is_Operation() == 0)
	{
			return ;
	}
	taskENTER_CRITICAL();
	// sum
	Display_Oper_Number(type);
	Display_Oper_value(num);
	Display_Mode_Hide();
	
	
	//版本号显示小数点
	if((type == OPERATION_DISPLAY_VERSION) ||( type == OPERATION_DRIVER_VERSION))
		Lcd_Display_Symbol(STATUS_BIT_POINT);
	else
		Lcd_Display_Symbol(0);
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
	return;
}




//------------------- 切换模式  ----------------------------

// 进入操作菜单
void To_Operation_Menu(void)
{
	// 操作 菜单
	App_Operation_Init();
	
	//功能暂停, 电机关闭
	Set_System_State_Machine(OPERATION_MENU_STATUS);

	Operation_State_Machine = OPERATION_ADDR_SET;		//	状态机	

  Operation_Menu_Value = Operation_Addr_Value;		//	菜单值	
	
	Lcd_Show_Operation( Operation_State_Machine, Operation_Menu_Value);
	
}

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 按键回调 ----------------------------

// ① 档位键
static void on_Button_1_clicked(void)
{
	button_cnt = 0;
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value < 255)
			(Operation_Addr_Value)++;
		else
			Operation_Addr_Value = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate < OPERATION_BAUD_MAX)
			(Operation_Baud_Rate)++;
		else
			Operation_Baud_Rate = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value < OPERATION_SHIELD_MAX)
			(Operation_Shield_Value)++;
		else
			Operation_Shield_Value = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles < OPERATION_POLES_MAX)
			(Operation_Motor_Poles)++;
		else
			Operation_Motor_Poles = OPERATION_POLES_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
}

// ② 时间键
static void on_Button_2_clicked(void)
{
	button_cnt = 0;
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value > 0)
			(Operation_Addr_Value)--;
		else
			Operation_Addr_Value = 255;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate > 0)
			(Operation_Baud_Rate)--;
		else
			Operation_Baud_Rate = OPERATION_BAUD_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value > 0)
			(Operation_Shield_Value)--;
		else
			Operation_Shield_Value = OPERATION_SHIELD_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles > OPERATION_POLES_MIX)
			(Operation_Motor_Poles)--;
		else
			Operation_Motor_Poles = OPERATION_POLES_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
}

// ③ 模式键
static void on_Button_3_clicked(void)
{
	button_cnt = 0;
	switch(Operation_State_Machine)
	{
		case OPERATION_ADDR_SET:
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS, Operation_Addr_Value );
			Operation_State_Machine = OPERATION_BAUD_RATE;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
		break;
		case OPERATION_BAUD_RATE:
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE, Operation_Baud_Rate );
			Operation_State_Machine = OPERATION_SHIELD_MENU;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		break;
		case OPERATION_SHIELD_MENU:
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_DISTRIBUTION_NETWORK_CONTROL, Operation_Shield_Value );
			Operation_State_Machine = OPERATION_MOTOR_POLES;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
		break;
		case OPERATION_MOTOR_POLES:
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_POLE_NUMBER, Operation_Motor_Poles );
			Operation_State_Machine = OPERATION_DISPLAY_VERSION;
			Lcd_Show_Operation( Operation_State_Machine, (SOFTWARE_VERSION_HIGH*100 + SOFTWARE_VERSION_LOW));
		break;
		case OPERATION_DISPLAY_VERSION:
			//Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
			Operation_State_Machine = OPERATION_DRIVER_VERSION;
			Lcd_Show_Operation( Operation_State_Machine, (SOFTWARE_VERSION_HIGH*100 + SOFTWARE_VERSION_LOW));
		break;
		default:
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
			Operation_State_Machine = OPERATION_ADDR_SET;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
		break;
	}
}


// ④ 开机键  短按
static void on_Button_4_Short_Press(void)
{
	
	To_Free_Mode(1);
}

// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Button_1_2_Short_Press(void)
{

}

// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Button_1_3_Short_Press(void)
{

}


// ② + ③  组合键  短按   
static void on_Button_2_3_Short_Press(void)
{
}

// ② + ④  组合键  短按
static void on_Button_2_4_Short_Press(void)
{
}

//------------------- 按键回调   长按 ----------------------------

static void on_Button_1_Long_Press(void)
{	
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value < 255)
		{
			button_cnt++;
			if(button_cnt > 5)
			{
				(Operation_Addr_Value) += 5;
			}
			else if(button_cnt > 10)
				(Operation_Addr_Value) += 10;
			else
				(Operation_Addr_Value)++;
			
			if(Operation_Addr_Value > 255)
				Operation_Addr_Value = 255;
		}
		else
			Operation_Addr_Value = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(button_cnt++ > 5)
		{
			button_cnt = 0;
			if(Operation_Baud_Rate < OPERATION_BAUD_MAX)
				(Operation_Baud_Rate)++;
			else
				Operation_Baud_Rate = 0;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
		}
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(button_cnt++ > 3)
		{
			button_cnt = 0;
			if(Operation_Shield_Value < OPERATION_SHIELD_MAX)
				(Operation_Shield_Value)++;
			else
				Operation_Shield_Value = 0;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		}
	}
}

static void on_Button_2_Long_Press(void)
{
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value > 0)
		{
			button_cnt++;
			if(button_cnt > 5)
			{
				(Operation_Addr_Value) -= 5;
			}
			else if(button_cnt > 10)
			{
				(Operation_Addr_Value) -= 10;
			}
			else
				(Operation_Addr_Value)--;
			
		}
		else
			Operation_Addr_Value = 255;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(button_cnt++ > 3)
		{
			button_cnt = 0;
			if(Operation_Baud_Rate > 0)
				(Operation_Baud_Rate)--;
			else
				Operation_Baud_Rate = OPERATION_BAUD_MAX;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
		}
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(button_cnt++ > 5)
		{
			button_cnt = 0;
			if(Operation_Shield_Value > 0)
				(Operation_Shield_Value)--;
			else
				Operation_Shield_Value = OPERATION_SHIELD_MAX;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		}
	}
}

static void on_Button_3_Long_Press(void)
{
}


static void on_Button_4_Long_Press(void)
{
}

static void on_Button_1_2_Long_Press(void)
{
}

static void on_Button_1_3_Long_Press(void)
{
}

// 恢复出厂设置
static void on_Button_2_3_Long_Press(void)
{

}

//
static void on_Button_2_4_Long_Press(void)
{

}


