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
#include "data.h"
#include "timing.h"
#include "display.h"
#include "Breath_light.h"
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

#define KEY_IO_NUMBER_MAX			4

#define KEY_MULTIPLE_CLICKS_MAX			8
#define KEY_MULTIPLE_CLICKS_TIME			3000
//------------------- ���� & ���� ----------------------------
#define KEY_SPEED_IO_PORT			GPIOC
#define KEY_SPEED_IO_PIN			GPIO_PIN_7

#define KEY_TIME_IO_PORT			GPIOC
#define KEY_TIME_IO_PIN				GPIO_PIN_8

#define KEY_TRAIN_IO_PORT			GPIOC
#define KEY_TRAIN_IO_PIN			GPIO_PIN_9

#define KEY_POWER_IO_PORT			GPIOA
#define KEY_POWER_IO_PIN			GPIO_PIN_8


//------------------- led & ���� ----------------------------
#define LED_SPEED_IO_PORT			GPIOC
#define LED_SPEED_IO_PIN			GPIO_PIN_3

#define LED_TIME_IO_PORT			GPIOC
#define LED_TIME_IO_PIN				GPIO_PIN_2

#define LED_TRAIN_IO_PORT			GPIOC
#define LED_TRAIN_IO_PIN			GPIO_PIN_1

#define LED_POWER_IO_PORT			GPIOC//GPIOA
#define LED_POWER_IO_PIN			GPIO_PIN_0//GPIO_PIN_1

/* Private variables ---------------------------------------------------------*/

// ������Ӧ ����
IO_Hardware_Pin Key_Gpio_Pin_Array[KEY_IO_NUMBER_MAX] = {
	{ KEY_SPEED_IO_PORT, KEY_SPEED_IO_PIN }, 
	{ KEY_TIME_IO_PORT, 	KEY_TIME_IO_PIN }, 
	{ KEY_TRAIN_IO_PORT, KEY_TRAIN_IO_PIN }, 
	{ KEY_POWER_IO_PORT, KEY_POWER_IO_PIN }
};

// �����ƶ�Ӧ ����
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

// �̰� �ۺ���
void (*p_Funtion_Button[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_clicked,  on_pushButton_2_clicked,  on_pushButton_3_clicked,  on_pushButton_4_Short_Press,
	on_pushButton_1_2_Short_Press, on_pushButton_1_3_Short_Press, 
	on_pushButton_2_3_Short_Press, on_pushButton_2_4_Short_Press,
};

// ���� �ۺ���
void (*p_Funtion_Long_Press[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_1_Long_Press,  on_pushButton_2_Long_Press,  on_pushButton_3_Long_Press,  on_pushButton_4_Long_Press,
	on_pushButton_1_2_Long_Press, on_pushButton_1_3_Long_Press,
	on_pushButton_2_3_Long_Press, on_pushButton_2_4_Long_Press,
};

// ���� �����ж� ʱ��
uint32_t Key_Long_Press_Confirm_Value[KEY_CALL_OUT_NUMBER_MAX]={
	KEY_LONG_PRESS_TIME_SHORT, KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME,
	KEY_LONG_PRESS_TIME, KEY_LONG_PRESS_TIME,KEY_LONG_PRESS_TIME,KEY_LONG_PRESS_TIME,
};



//------------------- ���� ���� ----------------------------
uint8_t Key_IO_Hardware = 0;		// ��ȡkey ֵ
uint8_t Key_IO_Old = 0;					// ��һ�� key ֵ


uint8_t Key_Multiple_Clicks_cnt = 0;				//8 ����������
uint8_t Key_Multiple_Clicks_Old = 0;		// ��һ�� key ֵ
uint32_t Key_Multiple_Clicks_time = 0;		// ��һ�� key ʱ��
uint32_t Key_Handler_Timer=0;

uint32_t Key_For_Sleep_time = 0;		// ˯��ʱ��

uint32_t Key_Long_Press_cnt[KEY_CALL_OUT_NUMBER_MAX]={0};	// ���� ������

uint8_t System_Self_Testing_State = 0;

uint8_t Key_Buzzer_cnt = 0;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/**********************************************************************************************
*
*						�����ص�    �̰�
*
**********************************************************************************************/
//================================== �� ��λ��
void on_pushButton_clicked(void)
{
	if((*p_System_State_Machine == POWER_OFF_STATUS) || System_is_Pause() || System_is_Stop())
			return;

	Clean_Timing_Timer_Cnt();
	Set_Temp_Slow_Down_Speed(0);//�����ٶȺ����¼���
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(*p_OP_ShowNow_Speed >= 100)
			*p_OP_ShowNow_Speed = 20;
		else
			*p_OP_ShowNow_Speed += 1;
		
		//if(Motor_is_Start())
		//if(System_is_Running())
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		//else if(System_is_Starting())
			//*p_OP_ShowNow_Time = p_OP_ShowLater->time;
		Arbitrarily_To_Initial();
		Lcd_Show();
	}
	else
	{
		if((*p_OP_ShowNow_Speed % 20) != 0)
			*p_OP_ShowNow_Speed += (20-(*p_OP_ShowNow_Speed % 20));
		else
		*p_OP_ShowNow_Speed += 20;
		
		if(*p_OP_ShowNow_Speed > 100)
			*p_OP_ShowNow_Speed = 20;
		
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		Arbitrarily_To_Initial();
		Lcd_Show();
	}
}

//================================== �� ʱ���
void on_pushButton_2_clicked(void)
{
	if(*p_System_State_Machine == POWER_OFF_STATUS)
		return;
	Clean_Timing_Timer_Cnt();
	
	if(*p_System_State_Machine == TIMING_MODE_INITIAL)
	{
			if(*p_OP_ShowNow_Time >= 5400)
					*p_OP_ShowNow_Time = 900;
			else
			{
					if((*p_OP_ShowNow_Time % 900) != 0)
							*p_OP_ShowNow_Time += (900-(*p_OP_ShowNow_Time % 900));
					else
							*p_OP_ShowNow_Time += 900;
			}
			Lcd_Show();
	}
	else
	{
			To_Timing_Mode();
			Fun_Change_Mode();
	}
	Arbitrarily_To_Initial();
	
	if(*p_System_State_Machine <= TIMING_MODE_STOP)	// ��ʱ
	{
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
	}
}

//================================== �� ģʽ��
void on_pushButton_3_clicked(void)
{
	if(*p_System_State_Machine == POWER_OFF_STATUS)
		return;
	
	Clean_Timing_Timer_Cnt();
	if(System_Mode_Train())
	{
			if(*p_PMode_Now >= TRAINING_MODE_NUMBER_MAX)
			{
					To_Free_Mode(0);
			}
			else
			{
					To_Train_Mode((*p_PMode_Now+1));
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

//================================== �� ������  �̰�
void on_pushButton_4_Short_Press(void)
{
	Clean_Timing_Timer_Cnt();
	if(*p_System_State_Machine == POWER_OFF_STATUS)//�ػ��� ִ�п���
	{
			System_Power_On();
			return;
	}

	if(System_is_Initial() && (Special_Status_Get(SPECIAL_BIT_SKIP_INITIAL)))	// ��ʼ --> ��������
	{
			Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
			Arbitrarily_To_Starting();
			Data_Set_Current_Speed(p_OP_ShowLater->speed);//ע��,��Ҫ����������״̬���������ٶ�,��"����"
	}
	else if(System_is_Pause())	// ��ͣ --> ����
	{
		Arbitrarily_To_Starting();
		Data_Set_Current_Speed(p_OP_ShowLater->speed);//ע��,��Ҫ����������״̬���������ٶ�,��"����"
	}
	else	// ���� --> ��ͣ
	{
		Arbitrarily_To_Pause();
		Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
	}
	
	//*p_OP_ShowNow_Time = 20;
	Lcd_Show();
}

//================================== �� + ��  ��ϼ�  ���� ���ù���
void on_pushButton_1_2_Short_Press(void)
{
	//Set_Fault_Data(0xAC);
}

//================================== �� + ��  ��ϼ�  �̰�   �л���λ 100�� or 5��
void on_pushButton_1_3_Short_Press(void)
{
	if(*p_System_State_Machine == POWER_OFF_STATUS)
		return;
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
		Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
	else
		Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
}

//================================== �� + ��  ��ϼ�  �ָ���������
void on_pushButton_2_3_Short_Press(void)
{
	//Restore_Factory_Settings();
}

//================================== �� + ��  ��ϼ�  ��������˵�
void on_pushButton_2_4_Short_Press(void)
{
	//To_Operation_Menu();
}



/**********************************************************************************************
*
*						�����ص�    ����
*
**********************************************************************************************/
//================================== �� ��λ��
void on_pushButton_1_Long_Press(void)
{
	if((*p_System_State_Machine == POWER_OFF_STATUS) || System_is_Pause() || System_is_Stop())
			return;
	
	if(Key_Long_Press_cnt[0] == KEY_LONG_PRESS_TIME)
	{
		//�������� �߾���ת��
		if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR) == 0)
			Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
	}
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(*p_OP_ShowNow_Speed >= 100)
				*p_OP_ShowNow_Speed = 20;
		else
			*p_OP_ShowNow_Speed += 1;

		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
		Arbitrarily_To_Initial();
		Lcd_Show();
	}

}
//================================== �� ʱ���
void on_pushButton_2_Long_Press(void)
{
	//Set_Fault_Data(0);
}
//================================== �� ģʽ��
void on_pushButton_3_Long_Press(void)
{
}
//================================== �� ������  �̰�
void on_pushButton_4_Long_Press(void)
{
    System_Power_Off();
}
//================================== �� + ��  ��ϼ�
//   wifi���
void on_pushButton_1_2_Long_Press(void)
{
	//if(Motor_is_Start() ==0)
	{
		WIFI_Get_In_Distribution();
	}
}

//================================== �� + ��  ��ϼ�
void on_pushButton_1_3_Long_Press(void)
{
	//test 
	//TM1621_LCD_Init();
}
//================================== �� + ��  ��ϼ�
//   �������
void on_pushButton_2_3_Long_Press(void)
{
//	if(System_is_Power_Off())
//	{
//		To_Operation_Menu();
//	}
	//if(Motor_is_Start() ==0)
	{
		BT_Get_In_Distribution();
	}
}
//================================== �� + ��  ��ϼ�
//  �ػ���   �ָ�����
void on_pushButton_2_4_Long_Press(void)
{
//	if(System_is_Power_Off())
//	{
//		Restore_Factory_Settings();
//	}
}

//***************************************************************************************************


//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
void App_Key_Init(void)
{
	Led_Button_On(0x0F);	// ����
	
	System_Boot_Screens();
	System_Power_Off();
}

//------------------- ������ ----------------------------
// ������
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

//���ⰴ������
void Special_Button_Rules()
{
	
	//�ػ��� ���� 8��
	if( Key_Multiple_Clicks_Old != Key_IO_Hardware)
	{
		Key_Multiple_Clicks_cnt = 0;
		Key_Multiple_Clicks_Old = Key_IO_Hardware;
		Key_Multiple_Clicks_time =  Key_Handler_Timer;
	}
	Key_Multiple_Clicks_cnt ++;
	if( ( Key_Handler_Timer - Key_Multiple_Clicks_time ) <= (KEY_MULTIPLE_CLICKS_TIME/KEY_THREAD_LIFECYCLE))
	{
		if(Key_Multiple_Clicks_cnt >= KEY_MULTIPLE_CLICKS_MAX)
		{
			// �Բ�
			if(Key_IO_Hardware == KEY_VALUE_BIT_BUTTON_1)
				System_Self_Testing_State = 0xAA;
			// �˵�
			else if(Key_IO_Hardware == KEY_VALUE_BIT_BUTTON_2)
				To_Operation_Menu();
			// �ָ�����
			else if(Key_IO_Hardware == KEY_VALUE_BIT_BUTTON_3)
				Restore_Factory_Settings();
		}
	}
	else
	{
		Key_Multiple_Clicks_cnt = 0;
		Key_Multiple_Clicks_time =  Key_Handler_Timer;
	}
}

void Buzzer_Click_On(void)
{
	Key_Buzzer_cnt = 1;
}

void Buzzer_Click_Handler(void)
{
	if(Key_Buzzer_cnt == 1)
		{
			TM1621_Buzzer_Off();
		}
		else if(Key_Buzzer_cnt == 2)
		{
			TM1621_Buzzer_Click();
		}
		else if(Key_Buzzer_cnt > (KEY_BUZZER_TIME+2))
		{
			TM1621_Buzzer_Off();
			Key_Buzzer_cnt = 0;
		}
		
		if(Key_Buzzer_cnt > 0)
			Key_Buzzer_cnt++;
}


// ������ѭ������
//  20 ms
void App_Key_Task(void)
{
	uint8_t i;
	uint8_t debug_buffer[32]={0};
	
	Key_Handler_Timer ++;
	
		//����˯��
		if(Key_Handler_Timer > (Key_For_Sleep_time + KEY_FOR_SLEEP_TIME_SHORT))
		{
			TM1621_Set_light_Mode(1);
		}
		
		for(i=0; i<KEY_CALL_OUT_NUMBER_MAX; i++)
		{
			if(Key_IO_Hardware == Key_IO_Ordering_Value[i])
			{
				Key_For_Sleep_time = Key_Handler_Timer;// ˯�߼�ʱ
				TM1621_Set_light_Mode(0);
				
				if(Key_IO_Old == Key_IO_Ordering_Value[i])
				{
					if(++Key_Long_Press_cnt[i] >= KEY_LONG_PRESS_TIME)//����
					{
						//���Է��ʹ���
					sprintf((char*)debug_buffer,"[��������]: %d\n",Key_IO_Ordering_Value[i]);
					UART_Send_Debug(debug_buffer,strlen((char*)debug_buffer));
						
						if(OPERATION_MENU_STATUS == *p_System_State_Machine)
							p_Operation_Long_Press[i]();
						else if(ERROR_DISPLAY_STATUS == *p_System_State_Machine)
							p_Fault_Long_Press[i]();
						else
							p_Funtion_Long_Press[i]();
					}
				}
				else
				{
					if(Key_Long_Press_cnt[i] == 0)
					{
						Buzzer_Click_On();
					}
					//���Է��ʹ���
					sprintf((char*)debug_buffer,"[�������]: %d\n",i);
					UART_Send_Debug(debug_buffer,strlen((char*)debug_buffer));
					
					if(OPERATION_MENU_STATUS == *p_System_State_Machine)
						p_Operation_Button[i]();
					else if(ERROR_DISPLAY_STATUS == *p_System_State_Machine)
						p_Fault_Button[i]();
					else
						p_Funtion_Button[i]();
					
					//�ػ��� ���� 8��
					if(POWER_OFF_STATUS == *p_System_State_Machine)
					{
						Special_Button_Rules();
					}
					
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

// ������ѭ������
//  20 ms
void App_Key_Handler(void)
{
	static uint8_t io_shake=0;
	static uint8_t io_shake_cnt=0;
	
	io_shake = Key_Get_IO_Input();
	if(Key_IO_Hardware == io_shake)
	{
		if(++io_shake_cnt >= KEY_VALUE_SHAKE_TIME)
		{
			Buzzer_Click_Handler();
			
			//�Բ�ģʽ
			if(System_Self_Testing_State == 0xAA)
			{
				if(Key_IO_Hardware > 0)
				{
					Buzzer_Click_On();
					
					Key_IO_Old |= Key_IO_Hardware;
					Led_Button_On(Key_IO_Old);	// ����
				}
			}
			else//����ʹ��
			{
				App_Key_Task();
			}
		}
	}
	else
	{
		Key_IO_Hardware = io_shake;
		io_shake_cnt = 0;
	}

	
}


// ��ȡ����
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
		if(read_io == 0)//����Ч
			result |= (1<<i);
	}
	
//	if(*p_System_State_Machine == POWER_OFF_STATUS)
//	{
//		result &= KEY_VALUE_BIT_BUTTON_4;
//	}
	
	return result;
}


//------------------- ���ܽӿ� ----------------------------

//	���� ��������ģʽ
void System_Power_On(void)
{
	// ��� ����
	Check_Data_Init();

	//	״̬
	To_Free_Mode(0);				// ui
	
	Led_Button_On(0x0F);	// ����
	
	//
	
	// ��̨��ʱ��
	//BlackGround_Task_On();
}

//	�ػ�
void System_Power_Off(void)
{
	
	//�������״̬
	CallOut_Fault_State();
	//���������
	Clean_Fault_Recovery_Cnt();
	//to �ػ�ģʽ
	To_Power_Off();
	//���õ��ת��
	Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"

	Led_Button_On(0x0F);	// ����
	
	// ��̨��ʱ��
	//BlackGround_Task_Off();
	
	//�˳�100��λģʽ
	Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
}

//	��������
void System_Boot_Screens(void)
{
	
////******************  ����ģʽ **************************
//#ifdef SYSTEM_DEBUG_MODE
//	return;
//#endif
////*******************************************************

	//ȫ�� 2s
	Breath_light_Max();
	TM1621_Show_All();
	osDelay(2000);
	//������ & ����״̬ 2s
	Lcd_System_Information();
	osDelay(2000);
	Breath_light_Off();
}

//	�ָ���������
void Restore_Factory_Settings(void)
{
	// data �ָ�
	App_Data_ReInit();
	
	// wifi �ָ�
	
	// ���� ����ģʽ ��ʼ״̬
	To_Free_Mode(1);
	
}

