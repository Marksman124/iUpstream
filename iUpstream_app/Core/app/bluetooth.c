/**
******************************************************************************
* @file				bluetooth.c
* @brief			bluetooth 模组
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"


/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static BT_STATE_MODE_E BT_State_Machine=0;

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

//------------------- 设置wifi状态机 ----------------------------
void BT_Set_Machine_State(BT_STATE_MODE_E para)
{
	if(para <= BT_ERROR)
	{
		BT_State_Machine = para;
	}
}
	
//------------------- 获取wifi状态机 ----------------------------
BT_STATE_MODE_E BT_Get_Machine_State(void)
{
	return BT_State_Machine;
}


//------------------- 接收处理函数 ----------------------------
void BT_Read_Handler(void)
{

}

//------------------- 进入配网 ----------------------------
void BT_Get_In_Distribution(void)
{
	BT_Set_Machine_State( BT_DISTRIBUTION );
	
}

//------------------- 进入故障 ----------------------------
void BT_Get_In_Error(void)
{
	BT_Set_Machine_State( BT_ERROR );
	
}



