/**
******************************************************************************
* @file				Breath_light.c
* @brief			������  100 �׼�  50ms��ʱ
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "Breath_light.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

//******************  ����ģʽ **************************
#ifdef SYSTEM_DEBUG_MODE
#define LIGHT_BRIGHTNESS_MAX					0
#else
#define LIGHT_BRIGHTNESS_MAX					350			// �������  0~500
#endif
//*******************************************************

/* Private variables ---------------------------------------------------------*/

uint16_t Light_Brightness = 0;

uint16_t Light_Brightness_cmp = 0;

uint8_t Breath_light_direction=0;

/* Private function prototypes -----------------------------------------------*/

void Breath_light_PwmOut(uint16_t pul);

/* Private user code ---------------------------------------------------------*/

//------------------- Ӳ�� & ���� ----------------------------

void App_Breath_light_Init(void)
{
	//Ĭ�Ϲر�
	Breath_light_PwmOut(0);
}


//------------------- ��ѭ�� ----------------------------

void App_Breath_light_Handler(void)
{
	if( Light_Brightness_cmp != Light_Brightness )
	{
		if(Light_Brightness > LIGHT_BRIGHTNESS_MAX)
			Light_Brightness = LIGHT_BRIGHTNESS_MAX;
		{
			Breath_light_PwmOut(Light_Brightness);
			Light_Brightness_cmp = Light_Brightness;
		}
	}
	
	
	if(System_is_Starting() || (System_is_Running() && Special_Status_Get(SPECIAL_BIT_SKIP_STARTING)))	// ������
	{
		if(Breath_light_direction == 0)
		{
			if(Light_Brightness < LIGHT_BRIGHTNESS_MAX)
				Light_Brightness += (LIGHT_BRIGHTNESS_MAX/BREATH_LIGHT_GEAR_POSITION);
			else
				Breath_light_direction = 1;
		}
		else
		{
			if(Light_Brightness > 0)
			{
				if( Light_Brightness > (LIGHT_BRIGHTNESS_MAX/BREATH_LIGHT_GEAR_POSITION) )
					Light_Brightness -= (LIGHT_BRIGHTNESS_MAX/BREATH_LIGHT_GEAR_POSITION);
				else
					Light_Brightness = 0;
			}
			else
			{
				Breath_light_direction = 0;
			}
		}
	}
	else if(System_is_Pause())	// ��ͣ
	{
		
		if(Breath_light_direction == 0)
		{
			if(Light_Brightness < LIGHT_BRIGHTNESS_MAX)
				Light_Brightness += (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2));
			else
				Breath_light_direction = 1;
		}
		else
		{
			if(Light_Brightness > 0)
			{
				if( Light_Brightness > (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2)) )
					Light_Brightness -= (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2));
				else
					Light_Brightness = 0;
			}
			else
			{
				Breath_light_direction = 0;
			}
		}
	}
	else if(System_State_Machine == POWER_OFF_STATUS)	// �ػ�
	{
		if(Light_Brightness > 0)
		{
			if( Light_Brightness > (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2)) )
				Light_Brightness -= (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2));
			else
				Light_Brightness = 0;
		}
	}
	else
	{
		if(Light_Brightness < LIGHT_BRIGHTNESS_MAX)
				Light_Brightness += (LIGHT_BRIGHTNESS_MAX/(BREATH_LIGHT_GEAR_POSITION*2));
		//Light_Brightness = LIGHT_BRIGHTNESS_MAX;
	}
}


//------------------- pwm���� ----------------------------
// ���ڷ�Χ : 1 - 100
void Breath_light_PwmOut(uint16_t pul)
{
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pul);//pul
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

//------------------- ģ��������� ----------------------------

void Led_Light_On(void)
{
	
}

void Led_Light_Off(void)
{
	
}


