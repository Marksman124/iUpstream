/**
******************************************************************************
* @file				wifi.c
* @brief			wifi ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "wifi.h"


/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

WIFI_STATE_MODE_E WIFI_State_Machine=0;

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

//------------------- ����wifi״̬�� ----------------------------
void WIFI_Set_Machine_State(WIFI_STATE_MODE_E para)
{
	if(para <= WIFI_ERROR)
	{
		WIFI_State_Machine = para;
	}
}
	
//------------------- ��ȡwifi״̬�� ----------------------------
WIFI_STATE_MODE_E WIFI_Get_Machine_State(void)
{
	return WIFI_State_Machine;
}


//------------------- ���մ����� ----------------------------
void WIFI_Read_Handler(void)
{

}

//------------------- �������� ----------------------------
void WIFI_Get_In_Distribution(void)
{
	WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
	
}

//------------------- ������� ----------------------------
void WIFI_Get_In_Error(void)
{
	WIFI_Set_Machine_State( WIFI_ERROR );
	
}



