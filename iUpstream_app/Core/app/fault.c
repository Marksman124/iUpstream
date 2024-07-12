/**
******************************************************************************
* @file				fault.c
* @brief			故障模块 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-13
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "fault.h"
#include "tm1621.h"

/* Private includes ----------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
extern void To_Free_Mode(uint8_t mode);

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 上
void on_Fault_Button_1_clicked(void);
// ② 下
void on_Fault_Button_2_clicked(void);
// ③ 确认
void on_Fault_Button_3_clicked(void);
// ④ 取消
void on_Fault_Button_4_Short_Press(void);
void on_Fault_Button_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
void on_Fault_Button_1_3_Short_Press(void);
// ② + ③  组合键  短按
void on_Fault_Button_2_3_Short_Press(void);
// ② + ④  组合键  短按
void on_Fault_Button_2_4_Short_Press(void);
//--------------- 长按 -----------------------
// 长按
void on_Fault_Button_1_Long_Press(void);
void on_Fault_Button_2_Long_Press(void);
void on_Fault_Button_3_Long_Press(void);
void on_Fault_Button_4_Long_Press(void);
void on_Fault_Button_1_2_Long_Press(void);
void on_Fault_Button_1_3_Long_Press(void);
void on_Fault_Button_2_3_Long_Press(void);
void on_Fault_Button_2_4_Long_Press(void);

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

#define LCD_SYMBOL_FOT_FAULT								(STATUS_BIT_BLUETOOTH & STATUS_BIT_WIFI)

/* Private variables ---------------------------------------------------------*/

uint8_t Fault_Number_Sum = 0;	// 故障总数
uint8_t Fault_Number_Cnt = 0;	// 当前故障

uint16_t *p_MB_Fault_State;

// 发送缓冲区
uint8_t fault_send_buffer[24] = {0};


// 短按 槽函数
void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_clicked,  on_Fault_Button_2_clicked,  on_Fault_Button_3_clicked,  on_Fault_Button_4_Short_Press,
	on_Fault_Button_1_2_Short_Press, on_Fault_Button_1_3_Short_Press,
	on_Fault_Button_2_3_Short_Press, on_Fault_Button_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_Long_Press,  on_Fault_Button_2_Long_Press,  on_Fault_Button_3_Long_Press,  on_Fault_Button_4_Long_Press,
	on_Fault_Button_1_2_Long_Press, on_Fault_Button_1_3_Long_Press,
	on_Fault_Button_2_3_Long_Press, on_Fault_Button_2_4_Long_Press,
};


/* Private user code ---------------------------------------------------------*/


// 初始化
void App_Fault_Init(void)
{
	p_MB_Fault_State = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER, MB_SYSTEM_FAULT_STATUS );
	
}

// 检测故障
uint8_t If_System_Is_Error(void)
{
	// 读驱动板故障
	//Motor_ReadFault_Send();
	// 检查本地故障
	
	//
	if(*p_System_Fault_Static > 0)
		return 1;
	else
		return 0;
}

// 设置故障值
void Set_Fault_Data(uint16_t type)
{
	
	*p_MB_Fault_State = type;
	
}

//-------------------   ----------------------------

// 获取总数
uint8_t Get_Fault_Number_Sum(uint16_t para)
{
	uint8_t i;
	uint8_t result = 0;
	
	for(i=0; i<16; i++)
	{
		if(para & (1<<i))
			result ++;
	}
	
	return result;
}

// 获取故障号
uint8_t Get_Fault_Number_Now(uint16_t para, uint8_t num)
{
	uint8_t i;
	uint8_t result = 0;
	
	for(i=0; i<16; i++)
	{
		if(para & (1<<i))
		{
			result ++;
			if(num == result)
				return i+1;
		}
	}
	
	return 0;
}

// 进入故障界面
void To_Fault_Menu(void)
{
	
	//功能暂停, 电机关闭
	Set_System_State_Machine(ERROR_DISPLAY_STATUS);

	Fault_Number_Sum = Get_Fault_Number_Sum(*p_MB_Fault_State);
	
	Fault_Number_Cnt = 1;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
	
}

// 清除故障状态
void Clean_Fault_State(void)
{
	*p_MB_Fault_State = 0;
	
	Fault_Number_Sum = 0;
	
	Fault_Number_Cnt = 0;
	
	To_Free_Mode(1);
}
/* Private function prototypes -----------------------------------------------*/

//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Number	

显示当前故障编号， 0-100
******************************************************************************
*/  
void Display_Show_Number(uint8_t no)
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

显示故障代码， E001 - E205
******************************************************************************
*/  
void Display_Show_FaultCode(uint16_t code)
{
	//字母 E
	TM1621_display_Letter(TM1621_COORDINATE_MIN_HIGH,  0x02);
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  	(code & 0x0F00)>>8);
	
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  	(code & 0x00F0)>>4);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  	(code & 0x000F));
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Sum

显示故障总数
******************************************************************************
*/  
void Display_Show_Sum(uint8_t sum)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  GET_NUMBER_TEN_DIGIT(sum));
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(sum));
	
	//TM1621_LCD_Redraw();
}

/***********************************************************************
*		显示 函数总入口
*
*
***********************************************************************/
void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint16_t type)
{
	uint16_t fault_label[16] = {0x001,0x002,0x003,
															0x101,0x102,0x103,0x104,0x105,
															0x201,0x202,0x203,
															0x301,0x302,0x303,0x304,0x305};
	if(System_is_Error() == 0)
	{
			return ;
	}
	taskENTER_CRITICAL();
	// sum
	Display_Show_Sum(sum);
	Display_Show_Number(now);
	Display_Show_FaultCode(fault_label[type-1]);
	
	Lcd_Display_Symbol( LCD_Show_Bit & LCD_SYMBOL_FOT_FAULT);
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
	return;
}

/* Private user code ---------------------------------------------------------*/
//------------------- 按键回调 ----------------------------

// ① 档位键
static void on_Fault_Button_1_clicked(void)
{
	if(Fault_Number_Cnt < Fault_Number_Sum)
		Fault_Number_Cnt ++;
	else
		Fault_Number_Cnt = 1;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
}

// ② 时间键
static void on_Fault_Button_2_clicked(void)
{
	if(Fault_Number_Cnt > 1)
		Fault_Number_Cnt --;
	else
		Fault_Number_Cnt = Fault_Number_Sum;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
}

// ③ 模式键
static void on_Fault_Button_3_clicked(void)
{

}


// ④ 开机键  短按
static void on_Fault_Button_4_Short_Press(void)
{
	
}

// ① + ②  组合键 
static void on_Fault_Button_1_2_Short_Press(void)
{
	Clean_Fault_State();
}


// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Fault_Button_1_3_Short_Press(void)
{

}


// ② + ③  组合键  短按   
static void on_Fault_Button_2_3_Short_Press(void)
{
}

// ② + ④  组合键  短按
static void on_Fault_Button_2_4_Short_Press(void)
{
}

//------------------- 按键回调   长按 ----------------------------

static void on_Fault_Button_1_Long_Press(void)
{

}

static void on_Fault_Button_2_Long_Press(void)
{
}

static void on_Fault_Button_3_Long_Press(void)
{
}


static void on_Fault_Button_4_Long_Press(void)
{
}

static void on_Fault_Button_1_2_Long_Press(void)
{
}

static void on_Fault_Button_1_3_Long_Press(void)
{
}

// 复位
static void on_Fault_Button_2_3_Long_Press(void)
{
	Set_Fault_Data(0);
	To_Free_Mode(0);				// ui
}

//
static void on_Fault_Button_2_4_Long_Press(void)
{

}


