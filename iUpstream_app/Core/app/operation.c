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


static uint8_t Operation_Addr_Value = 0;		//	地址	
static uint8_t Operation_Baud_Rate = 0;			//	波特率	
static uint8_t Operation_Shield_Value = 0;		//	控制方式	


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
		
}

static void _Delay(uint32_t mdelay)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < mdelay);
}


// 刷新 屏幕
void Lcd_Show_Operation(uint8_t type, uint8_t num)
{
	uint8_t i;
	
	// ===============
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// *                            *
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x20;
	operation_send_buffer[0] = 0x2A;
	operation_send_buffer[19] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	//speed
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x20;
	operation_send_buffer[0] = 0x2A;
	operation_send_buffer[19] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	
	operation_send_buffer[6] = (type / 10) + 0x30;
	operation_send_buffer[7] = (type % 10) + 0x30;
	
	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	
	// time
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x20;
	operation_send_buffer[0] = 0x2A;
	operation_send_buffer[19] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	
	operation_send_buffer[5] = (num / 100) + 0x30;
	operation_send_buffer[7] = ((num % 100) / 10) + 0x30;
	operation_send_buffer[8] = (num % 10) + 0x30;	

	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// *                            *
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x20;
	operation_send_buffer[0] = 0x2A;
	operation_send_buffer[19] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// ===============
	for(i=0; i<20; i++)
		operation_send_buffer[i] = 0x2A;
	operation_send_buffer[20] = 0x0A;
	operation_send_buffer[21] = 0x0A;
	operation_send_buffer[22] = 0x0A;
	operation_send_buffer[23] = 0x0A;
	HAL_UART_Transmit(p_huart_operation, operation_send_buffer, 24,0xFFFF);

	return;
}




//------------------- 切换模式  ----------------------------

// 进入操作菜单
void To_Operation_Menu(void)
{
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
}

// ③ 模式键
static void on_Button_3_clicked(void)
{
	button_cnt = 0;
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS, Operation_Addr_Value );
		Operation_State_Machine = OPERATION_BAUD_RATE;
		Lcd_Show_Operation( Operation_State_Machine, Operation_Baud_Rate);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE, Operation_Baud_Rate );
		Operation_State_Machine = OPERATION_SHIELD_MENU;
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		//Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
		Operation_State_Machine = OPERATION_DISPLAY_VERSION;
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_DISPLAY_VERSION)
	{
		//Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
		Operation_State_Machine = OPERATION_DRIVER_VERSION;
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else //if(Operation_State_Machine == OPERATION_DRIVER_VERSION)
	{
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
		Operation_State_Machine = OPERATION_ADDR_SET;
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
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


