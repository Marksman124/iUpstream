/**
******************************************************************************
* @file				display.c
* @brief			显示模块  显示模块本身不占用线程,由其它线程任务调用
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "tm1621.h"
#include "key.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
UART_HandleTypeDef* p_huart_display = &huart3;		 //调试串口 UART句柄

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

// 显示图标
uint8_t LCD_Show_Bit;

// 发送缓冲区
uint8_t send_buffer[24] = {0};

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 硬件 & 驱动 ----------------------------
// 初始化
void App_Display_Init(void)
{
	
	App_State_Machine_Init();
	
	LCD_Show_Bit = 0;
	
	Led_Button_On(0x0F);	// 按键
}
//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Speed	

显示速度， 0-100
******************************************************************************
*/  
void Display_Show_Speed(uint8_t speed)
{
	if(speed > 100)
		speed = 100;
	
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(speed));
	TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, GET_NUMBER_TEN_DIGIT(speed));
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, GET_NUMBER_ONE_DIGIT(speed));
	
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Min	

显示分钟， 0-99
******************************************************************************
*/  
void Display_Show_Min(uint8_t min)
{
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, GET_NUMBER_TEN_DIGIT(min));
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  GET_NUMBER_ONE_DIGIT(min));
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Sec	

显示 秒， 0-60
******************************************************************************
*/  
void Display_Show_Sec(uint8_t sec)
{
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH, GET_NUMBER_TEN_DIGIT(sec));
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW, GET_NUMBER_ONE_DIGIT(sec));
	
	//TM1621_LCD_Redraw();
}

/*
******************************************************************************
Display_Show_Mode	

显示模式， P1~P3
******************************************************************************
*/  
void Display_Show_Mode(uint8_t mode)
{
	//字母 p
	TM1621_display_Letter(TM1621_COORDINATE_MODE_HIGH,  'P');
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(mode));
	
	//TM1621_LCD_Redraw();
}
//------------------- 显示屏 熄灭 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Speed	

隐藏 速度， 0-100
******************************************************************************
*/  
void Display_Hide_Speed(uint8_t para)
{
	if(para & (1<<2))
		TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, 0);
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, 0xFF);
}
/*
******************************************************************************
Display_Hide_Min	

隐藏 分钟  0-99
******************************************************************************
*/  
void Display_Hide_Min(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  0xFF);
}
/*
******************************************************************************
Display_Hide_Sec	

隐藏 秒 0-60
******************************************************************************
*/  
void Display_Hide_Sec(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_SEC_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  0xFF);
}

/*
******************************************************************************
Display_Hide_Mode	

隐藏 模式， P1~P3
******************************************************************************
*/  
void Display_Hide_Mode(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_MODE_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  0xFF);
}

//------------------- 显示 & 界面 ----------------------------

void Lcd_Display_Symbol(uint8_t status_para)
{
	TM1621_Show_Symbol(TM1621_COORDINATE_BLUETOOTH, 		(status_para & STATUS_BIT_BLUETOOTH));
	TM1621_Show_Symbol(TM1621_COORDINATE_PERCENTAGE, 		(status_para & STATUS_BIT_PERCENTAGE));
	TM1621_Show_Symbol(TM1621_COORDINATE_WIFI, 					(status_para & STATUS_BIT_WIFI));
	TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		(status_para & STATUS_BIT_COLON));
	TM1621_Show_Symbol(TM1621_COORDINATE_DECIMAL_POINT, (status_para & STATUS_BIT_POINT));
	
	return;
}

void Lcd_Test(uint8_t num)
{
	Display_Show_Speed(num*11);
	Display_Show_Min((num*11));
	Display_Show_Sec((num*11));
	
	return;
}

void Lcd_Display(Operating_Parameters op_para, uint8_t status_para, uint8_t mode)
{
	//speed
	Display_Show_Speed(op_para.speed);
	// time
	Display_Show_Min(GET_TIME_MINUTE_DIGIT(op_para.time));
	Display_Show_Sec(GET_TIME_SECOND_DIGIT(op_para.time));
	// mode
	if(mode == 0)
		Display_Hide_Mode(0xFF);
	else
		Display_Show_Mode(mode);
	
	Lcd_Display_Symbol(status_para);
	return;
}



// 息屏
void Lcd_Off(void)
{
	TM1621_Show_Off();
}


// 速度 熄灭
void Lcd_No_Speed(Operating_Parameters op_para, uint8_t status_para, uint8_t mode)
{
	//speed
	Display_Hide_Speed(0xFF);
	// time
	Display_Show_Min(GET_TIME_MINUTE_DIGIT(op_para.time));
	Display_Show_Sec(GET_TIME_SECOND_DIGIT(op_para.time));

}

//------------------- 外部接口  ----------------------------
/***********************************************************************
*		显示 函数总入口
*
*
***********************************************************************/
void Lcd_Show(void)
{
	
	if(System_is_Operation() || System_is_Error())
	{
			return ;
	}
	taskENTER_CRITICAL();
	//
	Lcd_Display(OP_ShowNow, LCD_Show_Bit,PMode_Now);
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}

// 机型码 & 拨码
void Lcd_System_Information(void)
{
	uint8_t dial_switch;
	//speed
	Display_Hide_Speed(0xFF);
	// time
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, (SYSTEM_PRODUCT_MODEL_CODE/1000)%10);
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  (SYSTEM_PRODUCT_MODEL_CODE/100)%10);
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  (SYSTEM_PRODUCT_MODEL_CODE/10)%10);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  (SYSTEM_PRODUCT_MODEL_CODE)%10);
	//
	dial_switch = Gpio_Get_Dial_Switch();
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  GET_NUMBER_TEN_DIGIT(dial_switch));
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(dial_switch));
	
	Lcd_Display_Symbol(0);
	
	TM1621_LCD_Redraw();
}

// 速度 熄灭
void Lcd_Speed_Off(void)
{
	//背光 关
	//TM1621_BLACK_OFF()
	//
	taskENTER_CRITICAL();
	Lcd_No_Speed(OP_ShowNow, LCD_Show_Bit,PMode_Now);
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}

//------------------- 切换模式  ----------------------------
// 切换模式
void Fun_Change_Mode(void)
{
    if(Motor_is_Start())
    {
			OP_ShowNow.speed = 20;
			
			Lcd_Show();
    }
		else
		{
			
		}
}

// 关机
void To_Power_Off(void)
{
	
	PMode_Now = 0;
	Period_Now = 0;
	
	Special_Status_Bit = 0;
	
	System_State_Machine = POWER_OFF_STATUS;
		
	Lcd_Off();
}

// 自由模式
void To_Free_Mode(uint8_t mode)
{
	if(mode == 0)
		Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	else
		Special_Status_Add(SPECIAL_BIT_SKIP_INITIAL);
	
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	
	Set_System_State_Machine(FREE_MODE_INITIAL);
	*p_OP_ShowLater = *p_OP_Free_Mode;
	OP_ShowNow = *p_OP_ShowLater;
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
	PMode_Now = 0;
	Lcd_Show();
}

// 定时模式
void To_Timing_Mode(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	Set_System_State_Machine(TIMING_MODE_INITIAL);

	*p_OP_ShowLater = *p_OP_Timing_Mode;
	OP_ShowNow = *p_OP_ShowLater;
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
	PMode_Now = 0;
	Lcd_Show();
}

//	训练模式  num:1-3
void To_Train_Mode(uint8_t num)
{
	if(Is_Mode_Legal(num) == 0)
		return;
	
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	PMode_Now = num;
	
	Set_System_State_Machine(TRAINING_MODE_INITIAL);

	p_OP_ShowLater->speed = p_OP_PMode[num-1][0].speed;
	p_OP_ShowLater->time = 0;
	
	OP_ShowNow.speed = p_OP_PMode[num-1][0].speed;
	OP_ShowNow.time = p_OP_PMode[num-1][TRAINING_MODE_PERIOD_MAX-1].time;
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
	
	
	Lcd_Show();
}




