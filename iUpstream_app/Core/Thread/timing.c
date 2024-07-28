/**
******************************************************************************
* @file				timing.c
* @brief			��ʱ�߳�  ��� 1 s, ִ�е����ͼ�ʱ����
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
#include "motor.h"
#include "display.h"
#include "key.h"
#include "debug_protocol.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
// ͨ�ü�����
static uint8_t Timing_Timer_Cnt = 0;

// �����ʱ��
static uint32_t Timing_Half_Second_Cnt = 0;

static uint32_t WIFI_Distribution_Timing_Cnt=0;		// wifi״̬ͼ�� ��ʱ��
static uint32_t BT_Distribution_Timing_Cnt=0;			// ����״̬ͼ�� ��ʱ��
static uint32_t System_Fault_Timing_Cnt=0;				// ���ϻָ� ��ʱ��
static uint8_t System_Fault_Recovery_Cnt=0;				// ���ϻָ����� ������
static uint8_t Fault_Recovery_Timing_Cnt=0;			// ���ϻָ����� ��ʱ��

static uint32_t Automatic_Shutdown_Timing_Cnt=0;				// �Զ��ػ� ��ʱ��

//  ���� ����
static uint32_t Temp_Slow_Down_Timing_Cnt= 0;		//��ʱ��

static uint8_t Temp_Slow_Down_State=0;//			���½��� ��־  bit 1 :mos  bit 2 �����¶�
static uint8_t Temp_Slow_Down_Speed=0;//			���½��� �ٶ�  �ٷֱ�

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

// Ӳ�� & ����
void App_Timing_Init(void)
{
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
}

void Clean_Timing_Timer_Cnt(void)
{
	Timing_Timer_Cnt = 0;
}

void Speed_Save_Flash(Operating_Parameters op_node,uint8_t system_state)
{
	if(system_state == TIMING_MODE_INITIAL)
	{
		p_OP_Timing_Mode->speed = op_node.speed;
		p_OP_Timing_Mode->time = op_node.time;
		MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_TIME_MODE_SPEED*2), (uint16_t *)p_OP_Timing_Mode, 2 );// REG_HOLDING_NREGS
	}
	else if(system_state == FREE_MODE_INITIAL)
	{
		p_OP_Free_Mode->speed = op_node.speed;
		p_OP_Free_Mode->time = op_node.time;
		MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_FREE_MODE_SPEED*2), (uint16_t *)p_OP_Free_Mode, 2 );// REG_HOLDING_NREGS
	}
}

// wifi ״̬��  0.5���һ��
void WIFI_State_Handler(void)
{
	if(WIFI_Get_Machine_State() == WIFI_DISTRIBUTION)
	{
		if(( WIFI_Distribution_Timing_Cnt == 0)||(WIFI_Distribution_Timing_Cnt > Timing_Half_Second_Cnt))
		{
			WIFI_Distribution_Timing_Cnt = Timing_Half_Second_Cnt;
		}
		
		if((Timing_Half_Second_Cnt % WIFI_DISTRIBUTION_BLINK_TIME) == 0)
		{
			if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
				LCD_Show_Bit |= STATUS_BIT_WIFI;
			else
				LCD_Show_Bit &= ~STATUS_BIT_WIFI;
		}
		
		//��ʱ�����ָ�
		if( (Timing_Half_Second_Cnt - WIFI_Distribution_Timing_Cnt) > WIFI_DISTRIBUTION_TIME_CALLOUT)
		{
			Timing_Half_Second_Cnt = 0;
			WIFI_Set_Machine_State(WIFI_NO_CONNECT);
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
	else if(WIFI_Get_Machine_State() == WIFI_WORKING)
	{
		LCD_Show_Bit |= STATUS_BIT_WIFI;
	}
	else
	{
		LCD_Show_Bit &= ~STATUS_BIT_WIFI;
	}
}

// ���� ״̬��  0.5���һ��
void BT_State_Handler(void)
{
	if(BT_Get_Machine_State() == BT_DISTRIBUTION)
	{
		if(( BT_Distribution_Timing_Cnt == 0)||(BT_Distribution_Timing_Cnt > Timing_Half_Second_Cnt))
		{
			BT_Distribution_Timing_Cnt = Timing_Half_Second_Cnt;
		}
		
		if((Timing_Half_Second_Cnt % BT_DISTRIBUTION_BLINK_TIME) == 0)
		{
			if((LCD_Show_Bit & STATUS_BIT_BLUETOOTH) == 0)
				LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
			else
				LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
		}
		
		//��ʱ�����ָ�
		if( (Timing_Half_Second_Cnt - BT_Distribution_Timing_Cnt) > BT_DISTRIBUTION_TIME_CALLOUT)
		{
			Timing_Half_Second_Cnt = 0;
			BT_Set_Machine_State(BT_NO_CONNECT);
			LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
		}
		
		Lcd_Show();
	}
	else if(BT_Get_Machine_State() == BT_ERROR)
	{
		if((Timing_Half_Second_Cnt % BT_ERROR_BLINK_TIME) == 0)
		{
			if((LCD_Show_Bit & STATUS_BIT_BLUETOOTH) == 0)
				LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
			else
				LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
		}
		Lcd_Show();
	}
	else if(BT_Get_Machine_State() == WIFI_WORKING)
	{
		LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
	}
	else
	{
		LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
	}
}



// ��� ���� ״̬��
void CallOut_Fault_State(void)
{
	Clean_Fault_State();
	System_Fault_Timing_Cnt = 0;
}

// ���� ״̬��  1���һ��
void Fault_State_Handler(void)
{
	if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
	{
		To_Fault_Menu();
	}
	else if(System_is_Error())//���� 2min�ָ� ��ͨѶ����
	{
		if(If_Fault_Recovery_Max()==0)
		{
			// 3������ �˳�����, ����3��ֻ������
			if((Timing_Half_Second_Cnt - Fault_Recovery_Timing_Cnt) > SYSTEM_FAULT_RECOVERY_TIME)
			{
				uint8_t debug_send_buffer[DEBUG_PROTOCOL_TX_MAX]={0};
				sprintf((char*)debug_send_buffer,"���ϻָ���ʱ����1Сʱ,���������:\t%d\n",System_Fault_Recovery_Cnt);
				UART_Send_Debug(debug_send_buffer,strlen((char*)debug_send_buffer));
				
				//���� 1 Сʱ ���������
				Clean_Fault_Recovery_Cnt();
				Fault_Recovery_Timing_Cnt = Timing_Half_Second_Cnt; // ���¼�ʱ
			}

			if(( System_Fault_Timing_Cnt == 0)||(System_Fault_Timing_Cnt > Timing_Half_Second_Cnt))
			{
				System_Fault_Timing_Cnt = Timing_Half_Second_Cnt;
				if( Fault_Recovery_Timing_Cnt == 0)
				{
					Fault_Recovery_Timing_Cnt = Timing_Half_Second_Cnt;
				}
			}
			
			//��ʱ ���� �ָ�
			if( (Timing_Half_Second_Cnt - System_Fault_Timing_Cnt) > SYSTEM_FAULT_TIME_CALLOUT)
			{
				System_Fault_Recovery_Cnt++;
				CallOut_Fault_State();
			}
		}
	}
	
	Update_Fault_Menu();
}

// ������ ״̬��  1���һ��
void Starting_State_Handler(void)
{
	Arbitrarily_To_Running();
	return;
	
//	OP_ShowNow.time --;
//	Lcd_Show();
//	if(OP_ShowNow.time == 0)
//	{
//		Arbitrarily_To_Running();
//		OP_ShowNow.time = p_OP_ShowLater->time;
//		Lcd_Show();
//	}
}

// ���� ״̬��  1���һ��
void Running_State_Handler(void)
{
	uint8_t slow_down_speed;//���½���ʹ��
	
	if(System_State_Machine == FREE_MODE_RUNNING)									// ����
	{
		OP_ShowNow.time ++;
		if(OP_ShowNow.time >= 6000)
		{
			OP_ShowNow.time = 0;
		}
	}
	else if(System_State_Machine == TIMING_MODE_RUNNING)					// ��ʱ
	{
		OP_ShowNow.time --;
		Lcd_Show();
		if(OP_ShowNow.time == 0)
		{
			Arbitrarily_To_Stop();
			OP_ShowNow.time = p_OP_Timing_Mode->time;
		}
	}
	else if(System_State_Machine == TRAINING_MODE_RUNNING)					// ѵ��
	{
		OP_ShowNow.time ++;
		if(Is_Mode_Legal(PMode_Now))
		{
			if(OP_ShowNow.time >= p_OP_PMode[PMode_Now-1][Period_Now].time)
			{
				if((Period_Now == (TRAINING_MODE_PERIOD_MAX-1))||(p_OP_PMode[PMode_Now-1][Period_Now+1].time < p_OP_PMode[PMode_Now-1][Period_Now].time))
				{
					Arbitrarily_To_Stop();
					OP_ShowNow.speed = p_OP_PMode[PMode_Now-1][0].speed;
				}
				else
				{
					Period_Now ++;
					Data_Set_Current_Speed(p_OP_PMode[PMode_Now-1][Period_Now].speed);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
				}
			}
		}
	}
	
	if(Motor_Speed_Is_Reach())
	{
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		{
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
		}
	}
	
	if(Get_Temp_Slow_Down_State())
	{
		Temp_Slow_Down_Timing_Cnt ++;
		if(( Temp_Slow_Down_Timing_Cnt == 1 ) ||((Temp_Slow_Down_Timing_Cnt % TIME_SLOW_DOWN_TIME)==0))
		{
			if( Temp_Slow_Down_Timing_Cnt == 1 )
				slow_down_speed = TIME_SLOW_DOWN_SPEED_01;
			else
				slow_down_speed = TIME_SLOW_DOWN_SPEED_02;
			
			if( Motor_Speed_Target_Get()  >= (slow_down_speed + TIME_SLOW_DOWN_SPEED_MIX))
			{
				Temp_Slow_Down_Speed += slow_down_speed;
				//���� ����
				Data_Set_Current_Speed(Motor_Speed_Target_Get() - slow_down_speed);
			}
		}
		
	}
	else if(Temp_Slow_Down_Timing_Cnt > 0)
	{

		//���� �ָ�
		if(Motor_Speed_Target_Get() >= TIME_SLOW_DOWN_SPEED_MAX)
		{
			slow_down_speed = 0;
			Temp_Slow_Down_Speed = 0;
			Temp_Slow_Down_Timing_Cnt = 0;//����ǰ�˳�
		}
				
		if(( Temp_Slow_Down_Timing_Cnt == 1 ) ||((Temp_Slow_Down_Timing_Cnt % TIME_SLOW_DOWN_TIME)==0))
		{
			if( Temp_Slow_Down_Timing_Cnt == 1 )
				slow_down_speed = TIME_SLOW_DOWN_SPEED_01;
			else
				slow_down_speed = TIME_SLOW_DOWN_SPEED_02;
			
			if(Temp_Slow_Down_Speed >= slow_down_speed)
			{
				Temp_Slow_Down_Speed -= slow_down_speed;
				
				Data_Set_Current_Speed(Motor_Speed_Target_Get() + slow_down_speed);
			}
			else if(Temp_Slow_Down_Speed == 0)
				Temp_Slow_Down_Timing_Cnt = 0;//����ǰ�˳�
			else
			{
				Data_Set_Current_Speed(Motor_Speed_Target_Get() + Temp_Slow_Down_Speed);
				Temp_Slow_Down_Speed = 0;
			}
				
		}
		
		Temp_Slow_Down_Timing_Cnt --;
	}
	
	//���ٽ��� 2��1ˢ
	if(((Temp_Slow_Down_Timing_Cnt % 4)==2)||((Temp_Slow_Down_Timing_Cnt % 4)==3))
	{
		Lcd_Show_Slow_Down(Get_Temp_Slow_Down_State());
	}
	else
		Lcd_Show();
}

// ��ͣ ״̬��  1���һ��
void Pause_State_Handler(void)
{
	if((Timing_Half_Second_Cnt - Automatic_Shutdown_Timing_Cnt) > AUTOMATIC_SHUTDOWN_TIME)
	{
		System_Power_Off();
	}
//	if(OP_ShowNow.time > 0)
//	{
//		OP_ShowNow.time --;
//		Lcd_Show();
//	}
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}

// ֹͣ ״̬��  1���һ��
void Stop_State_Handler(void)
{
	Timing_Timer_Cnt++;
	// 3�� ��˸
	if(Timing_Timer_Cnt < 4)
	{
		if( (Timing_Timer_Cnt % 2) == 1)
			Lcd_Off();
		else
			Lcd_Show();
	}
	else
	{
		// ���� ����ģʽ ��ʼ״̬
		To_Free_Mode(1);
		Lcd_Show();
		Timing_Timer_Cnt = 0;
	}
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}

// ��ʼ ״̬��  1���һ��
void Initial_State_Handler(void)
{
	if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// ���� �Զ�����
		return;
	
	Timing_Timer_Cnt++;
	// 3�� ��˸
	if(Timing_Timer_Cnt < 6)
	{
		if(( (Timing_Timer_Cnt % 2) == 1)&&(Timing_Timer_Cnt > 1))
			Lcd_Speed_Off();
		else
			Lcd_Show();
	}
	else
	{
		//Update_OP_Data();	// ��������ת��
		if(System_State_Machine <= TIMING_MODE_STOP)	// ��ʱ
			Memset_OPMode();//��flash

		//Speed_Save_Flash(OP_ShowNow,System_State_Machine);
		if(System_State_Machine == TIMING_MODE_INITIAL)
		{
			p_OP_ShowLater->time = OP_ShowNow.time;
		}
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
		
		
//		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))	//���� ������
		{
			//Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING); //�ײ�ת��ͬ������ɾ��
			p_OP_ShowLater->speed = OP_ShowNow.speed;
			
			Motor_Speed_Target_Set(OP_ShowNow.speed);
			
			//����
			
			Lcd_Show();
			Arbitrarily_To_Running();
		}
//		else
//		{
//			// ���� ������
//			Arbitrarily_To_Starting();
//			//*p_OP_ShowLater->time = OP_ShowNow.time;
//			OP_ShowNow.time = 20;
//			Motor_Speed_Target_Set(OP_ShowNow.speed);
//			Lcd_Show();
//		}
		Timing_Timer_Cnt = 0;
	}
}


// ��ʱ�������߳�
void App_Timing_Task(void)
{
	if((System_is_Power_Off())||(System_is_Operation()))
		return;
	
	Timing_Half_Second_Cnt ++;
//	if(Timing_Half_Second_Cnt > 10000)
//		Timing_Half_Second_Cnt = 0;
	
	WIFI_State_Handler();
	BT_State_Handler();

	if((Timing_Half_Second_Cnt % 2) == 1)
	{
		// ʱ�� : ��˸  ����
		if(Motor_is_Start())
		{
			LCD_Show_Bit |= STATUS_BIT_COLON;
			TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		1);
			TM1621_LCD_Redraw();
		}
	}
	else
	{
		// ʱ�� : ��˸  ����
		LCD_Show_Bit &= ~STATUS_BIT_COLON;
		TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		0);
		TM1621_LCD_Redraw();
		
		if(If_System_Is_Error())
		{
			Fault_State_Handler();
		}
		else
		{
			System_Fault_Timing_Cnt = 0;
			
			if(ERROR_DISPLAY_STATUS == Get_System_State_Machine())// && (If_Fault_Recovery_Max() == 0))
				Clean_Fault_State();
			
			if(System_is_Initial())
			{
				Initial_State_Handler();
			}
			else if(System_is_Starting())
			{
				Starting_State_Handler();
			}
			else if(System_is_Running())
			{
				Running_State_Handler();
			}
			else if(System_is_Pause())//��ͣ
			{
				Pause_State_Handler();
			}
			else if(System_is_Stop())//����
			{
				Stop_State_Handler();
			}
		}
	}
}

// ��ʱ�������߳�
void App_Timing_Handler(void)
{
	
	if(System_Self_Testing_State == 0xAA)
	{
		System_Self_Testing_Porgram();
		System_Power_Off();
		System_Self_Testing_State = 0;
	}
	else
	{
		App_Timing_Task();
	}
}


// �����߳�
void App_Timing_On(void)
{
	
}


// �ر��߳�
void App_Timing_Off(void)
{
	
}



//-------------------- ���ø��½���ģʽ 0:��   1�� ----------------------------
void Set_Temp_Slow_Down_State(uint8_t vaule)
{
	Temp_Slow_Down_State = vaule;
}

//-------------------- ��ȡ���½���ģʽ ----------------------------
uint8_t Get_Temp_Slow_Down_State(void)
{
	return Temp_Slow_Down_State;
}

//-------------------- ���ø��½��� �ٶ� �½����ٶ� ----------------------------
void Set_Temp_Slow_Down_Speed(uint8_t vaule)
{
	Temp_Slow_Down_Speed = vaule;
}

//-------------------- ��ȡ���½��� �ٶ� ----------------------------
uint8_t Get_Temp_Slow_Down_Speed(void)
{
	return Temp_Slow_Down_Speed;
}

//-------------------- ������ϻָ������� ----------------------------
void Clean_Fault_Recovery_Cnt(void)
{
	 System_Fault_Recovery_Cnt = 0;
}

//-------------------- �ۼƹ��ϻָ������� ----------------------------
void Add_Fault_Recovery_Cnt(void)
{
	 System_Fault_Recovery_Cnt ++;
}

//-------------------- ���������� ----------------------------
uint8_t If_Fault_Recovery_Max(void)
{
	if(System_Fault_Recovery_Cnt >= (SYSTEM_FAULT_RECOVERY_MAX))
		return 1;
	else
		return 0;
}

//-------------------- ��� �Զ��ػ���ʱ�� ----------------------------
void Clean_Automatic_Shutdown_Timer(void)
{
	 Automatic_Shutdown_Timing_Cnt = Timing_Half_Second_Cnt;
}
