/**
******************************************************************************
* @file				state_machine.c
* @brief			״̬�� 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "state_machine.h"
#include "timing.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

uint8_t System_State_Machine = 0;		//	״̬��	


// �ر�״̬ λ
uint8_t Special_Status_Bit = 0;


// ���뿪��
uint8_t System_Dial_Switch = 0;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
void App_State_Machine_Init(void)
{
	System_State_Machine = POWER_OFF_STATUS;  //״̬��
	
	Special_Status_Bit = 0;
}

//------------------- ����״̬��  ----------------------------

uint8_t Set_System_State_Machine(uint8_t val)
{
	if(val >= SYSTEM_STATE_END) //���
		return 0;
	
	System_State_Machine = val;
	
	return 1;
}


//------------------- ��ȡ״̬��  ----------------------------

uint8_t Get_System_State_Machine(void)
{
	return System_State_Machine;
}

//------------------- ���ٻ�ȡ״̬  ----------------------------

// �������״̬
uint8_t Motor_is_Start(void)
{
	if((System_State_Machine == FREE_MODE_STARTING) || (System_State_Machine == FREE_MODE_RUNNING) 
			|| (System_State_Machine == TIMING_MODE_STARTING) || (System_State_Machine == TIMING_MODE_RUNNING)
			|| (System_State_Machine == TRAINING_MODE_STARTING) || (System_State_Machine == TRAINING_MODE_RUNNING))
	return 1;
	else
		return 0;
}

// ��������״̬
uint8_t System_is_Normal_Operation(void)
{
	if((System_State_Machine >= FREE_MODE_INITIAL) && (System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// ��ʼ״̬
uint8_t System_is_Initial(void)
{
	if((System_State_Machine == FREE_MODE_INITIAL) || (System_State_Machine == TIMING_MODE_INITIAL) || (System_State_Machine == TRAINING_MODE_INITIAL))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Starting(void)
{
	if((System_State_Machine == FREE_MODE_STARTING) || (System_State_Machine == TIMING_MODE_STARTING) || (System_State_Machine == TRAINING_MODE_STARTING))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Running(void)
{
	if((System_State_Machine == FREE_MODE_RUNNING) || (System_State_Machine == TIMING_MODE_RUNNING) || (System_State_Machine == TRAINING_MODE_RUNNING))
		return 1;
	else
		return 0;
}

// ��ͣ״̬
uint8_t System_is_Pause(void)
{
	if((System_State_Machine == FREE_MODE_SUSPEND) || (System_State_Machine == TIMING_MODE_SUSPEND) || (System_State_Machine == TRAINING_MODE_SUSPEND))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Stop(void)
{
	if((System_State_Machine == FREE_MODE_STOP) || (System_State_Machine == TIMING_MODE_STOP) || (System_State_Machine == TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// �����˵�
uint8_t System_is_Operation(void)
{
	if(System_State_Machine == OPERATION_MENU_STATUS)
		return 1;
	else
		return 0;
}

// ���Ͻ���
uint8_t System_is_Error(void)
{
	if(System_State_Machine == ERROR_DISPLAY_STATUS)
		return 1;
	else
		return 0;
}

// �ػ�
uint8_t System_is_Power_Off(void)
{
	if(System_State_Machine == POWER_OFF_STATUS)
		return 1;
	else
		return 0;
}


// ���� ģʽ
uint8_t System_Mode_Free(void)
{
	if( (System_State_Machine >= FREE_MODE_INITIAL) && (System_State_Machine <= FREE_MODE_STOP))
		return 1;
	else
		return 0;
}

// ��ʱ ģʽ
uint8_t System_Mode_Time(void)
{
	if( (System_State_Machine >= TIMING_MODE_INITIAL) && (System_State_Machine <= TIMING_MODE_STOP))
		return 1;
	else
		return 0;
}

// ѵ�� ģʽ
uint8_t System_Mode_Train(void)
{
	if( (System_State_Machine >= TRAINING_MODE_INITIAL) && (System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}


//------------------- ����״̬ ���ı�ģʽ  ----------------------------
// --> ��ʼ״̬ ��������ģʽ��
void Arbitrarily_To_Initial(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	if(System_is_Initial())
		return;
	else if(System_is_Starting())
		System_State_Machine -= 1;
	else if(System_is_Running())
		System_State_Machine -= 2;
	else if(System_is_Pause())
		System_State_Machine -= 3;
	else if(System_is_Stop())
		System_State_Machine -= 4;
	
	return;
}
	
// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Starting(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	if(System_is_Initial())
		System_State_Machine += 1;
	else if(System_is_Starting())
		return;
	else if(System_is_Running())
		System_State_Machine -= 1;
	else if(System_is_Pause())
		System_State_Machine -= 2;
	else if(System_is_Stop())
		System_State_Machine -= 3;
	
	return;
}

// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Running(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	if(System_is_Initial())
		System_State_Machine += 2;
	else if(System_is_Starting())
		System_State_Machine += 1;
	else if(System_is_Running())
		return;
	else if(System_is_Pause())
		System_State_Machine -= 1;
	else if(System_is_Stop())
		System_State_Machine -= 2;
	
	return;
}

// --> ��ͣ״̬ ��������ģʽ��
void Arbitrarily_To_Pause(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	if(System_is_Initial())
		System_State_Machine += 3;
	else if(System_is_Starting())
		System_State_Machine += 2;
	else if(System_is_Running())
		System_State_Machine += 1;
	else if(System_is_Pause())
		return;
	else if(System_is_Stop())
		System_State_Machine -= 1;
	
	Clean_Automatic_Shutdown_Timer();
	return;
}

// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Stop(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	if(System_is_Initial())
		System_State_Machine += 4;
	else if(System_is_Starting())
		System_State_Machine += 3;
	else if(System_is_Running())
		System_State_Machine += 2;
	else if(System_is_Pause())
		System_State_Machine += 1;
	else if(System_is_Stop())
		return;
	
	Motor_Speed_Target_Set(0);
	Clean_Automatic_Shutdown_Timer();
	return;
}


//------------------- �ر�״̬ ��  ----------------------------

// ��� �ر�״̬ ��־
void Special_Status_Add(uint8_t num)
{
	Special_Status_Bit |= num;
}


// ɾ�� �ر�״̬ ��־
void Special_Status_Delete(uint8_t num)
{
	Special_Status_Bit &= ~num;
}

// ��ȡ �ر�״̬ ��־
uint8_t Special_Status_Get(uint8_t num)
{
	return (Special_Status_Bit & num);
}






