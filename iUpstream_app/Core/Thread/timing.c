/**
******************************************************************************
* @file				timing.c
* @brief			定时线程  间隔 1 s, 执行倒数和计时工作
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "timing.h"
#include "wifi.h"
#include "tm1621.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static uint8_t Timing_Timer_Cnt = 0;


static uint32_t Timing_Half_Second_Cnt = 0;

																		
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

// 硬件 & 驱动
void App_Timing_Init(void)
{
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
}

void Clean_Timing_Timer_Cnt(void)
{
	Timing_Timer_Cnt = 0;
}




// 定时任务主线程
void App_Timing_Handler(void)
{
	Timing_Half_Second_Cnt ++;
	if(Timing_Half_Second_Cnt > 10000)
		Timing_Half_Second_Cnt = 0;
	
	
	if((Timing_Half_Second_Cnt % 2) == 1)
	{
		/*static uint8_t cnt=0;
		Lcd_Test(cnt);
		cnt++;
		if(cnt> 9)
			cnt = 0;*/
		
		// 时间 : 闪烁  半秒
		if(Motor_is_Start())
		{
			LCD_Show_Bit |= STATUS_BIT_COLON;
			TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		1);
			TM1621_LCD_Redraw();
		}
		if(WIFI_Get_Machine_State() == WIFI_DISTRIBUTION)
		{
			if((Timing_Half_Second_Cnt % WIFI_DISTRIBUTION_BLINK_TIME) == 0)
			{
				if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
					LCD_Show_Bit |= STATUS_BIT_WIFI;
				else
					LCD_Show_Bit &= ~STATUS_BIT_WIFI;
			}
			Lcd_Show();
		}
		else if(WIFI_Get_Machine_State() == WIFI_ERROR)
		{
			if((Timing_Half_Second_Cnt % WIFI_ERROR_BLINK_TIME) == 0)
			{
				if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
					LCD_Show_Bit |= STATUS_BIT_WIFI;
				else
					LCD_Show_Bit &= ~STATUS_BIT_WIFI;
			}
			Lcd_Show();
		}
	}
	else
	{
		// 时间 : 闪烁  半秒
		LCD_Show_Bit &= ~STATUS_BIT_COLON;
		TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		0);
		TM1621_LCD_Redraw();
		
		if(If_System_Is_Error())
		{
			if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
			{
				To_Fault_Menu();
			}
		}
		else
		{
			if(System_is_Initial())
			{
				if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// 跳过 自动启动
					return;
				
				Timing_Timer_Cnt++;
				// 3秒 闪烁
				if(Timing_Timer_Cnt > 2)//第一秒不动
				{
					if(Timing_Timer_Cnt < 7)
					{
						if( (Timing_Timer_Cnt % 2) == 1)
							Lcd_Speed_Off();
						else
							Lcd_Show();
					}
					else
					{
						Update_OP_Data();	// 保存最新转速
						if(System_State_Machine == TIMING_MODE_INITIAL)
							p_OP_ShowLater->time = OP_ShowNow.time;
						if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))	//跳过 软启动
						{
							//Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING); //底层转速同步后再删除
							p_OP_ShowLater->speed = OP_ShowNow.speed;
							
							Motor_Speed_Target = OP_ShowNow.speed;
							Lcd_Show();
							Arbitrarily_To_Running();
						}
						else
						{
							// 进入 软启动
							Arbitrarily_To_Starting();
							//*p_OP_ShowLater->time = OP_ShowNow.time;
							OP_ShowNow.time = 20;
							Motor_Speed_Target = OP_ShowNow.speed;
							Lcd_Show();
						}
						Timing_Timer_Cnt = 0;
					}
				}
			}
			else if(System_is_Starting())
			{
				OP_ShowNow.time --;
				Lcd_Show();
				if(OP_ShowNow.time == 0)
				{
					Arbitrarily_To_Running();
					OP_ShowNow.time = p_OP_ShowLater->time;
					Lcd_Show();
				}
			}
			else if(System_is_Running())
			{
				if(System_State_Machine == FREE_MODE_RUNNING)
				{
					OP_ShowNow.time ++;
					Lcd_Show();
					if(OP_ShowNow.time >= 6000)
					{
						OP_ShowNow.time = 0;
					}
				}
				else if(System_State_Machine == TIMING_MODE_RUNNING)
				{
					OP_ShowNow.time --;
					Lcd_Show();
					if(OP_ShowNow.time == 0)
					{
						Arbitrarily_To_Stop();
						OP_ShowNow.time = p_OP_Timing_Mode->time;
						Lcd_Show();
					}
				}
				else if(System_State_Machine == TRAINING_MODE_RUNNING)
				{
					OP_ShowNow.time ++;
					if(Is_Mode_Legal(PMode_Now))
					{
						if(OP_ShowNow.time >= p_OP_PMode[PMode_Now-1][Period_Now].time)
						{
							if(Period_Now == (TRAINING_MODE_PERIOD_MAX-1))
							{
								Arbitrarily_To_Stop();
								OP_ShowNow.speed = p_OP_PMode[PMode_Now-1][0].speed;
							}
							else
							{
								Period_Now ++;
								OP_ShowNow.speed = p_OP_PMode[PMode_Now-1][Period_Now].speed;
							}
						}
					}
					Lcd_Show();
				}
				
//				if(Motor_Speed_Is_Reach())
//					if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
//						Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
			}
			else if(System_is_Pause())//暂停
			{
				if(OP_ShowNow.time > 0)
				{
					OP_ShowNow.time --;
					Lcd_Show();
				}
				if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
					Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
			}
			else if(System_is_Stop())//结束
			{
				Timing_Timer_Cnt++;
				// 3秒 闪烁
				if(Timing_Timer_Cnt < 4)
				{
					if( (Timing_Timer_Cnt % 2) == 1)
						Lcd_Off();
					else
						Lcd_Show();
				}
				else
				{
					// 返回 自由模式 初始状态
					To_Free_Mode(1);
					Lcd_Show();
					Timing_Timer_Cnt = 0;
				}
				if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
					Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
			}
		}
	}
}


// 开启线程
void App_Timing_On(void)
{
	
}


// 关闭线程
void App_Timing_Off(void)
{
	
}




