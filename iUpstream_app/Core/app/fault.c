/**
******************************************************************************
* @file				fault.c
* @brief			����ģ�� 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-13
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "fault.h"


/* Private includes ----------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
extern void To_Free_Mode(uint8_t mode);

//------------------- �����ص� ----------------------------
//--------------- �̰� -----------------------
// �� ��
void on_Fault_Button_1_clicked(void);
// �� ��
void on_Fault_Button_2_clicked(void);
// �� ȷ��
void on_Fault_Button_3_clicked(void);
// �� ȡ��
void on_Fault_Button_4_Short_Press(void);
void on_Fault_Button_1_2_Short_Press(void);
// �� + ��  ��ϼ�  �̰�   �л���λ 80�� or 5��
void on_Fault_Button_1_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
void on_Fault_Button_2_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
void on_Fault_Button_2_4_Short_Press(void);
//--------------- ���� -----------------------
// ����
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

UART_HandleTypeDef* p_huart_fault = &huart3;		 //���Դ��� UART���

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

uint8_t Fault_Number_Sum = 0;	// ��������
uint8_t Fault_Number_Cnt = 0;	// ��ǰ����

uint16_t *p_MB_Fault_State;

// ���ͻ�����
uint8_t fault_send_buffer[24] = {0};


// �̰� �ۺ���
void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_clicked,  on_Fault_Button_2_clicked,  on_Fault_Button_3_clicked,  on_Fault_Button_4_Short_Press,
	on_Fault_Button_1_2_Short_Press, on_Fault_Button_1_3_Short_Press,
	on_Fault_Button_2_3_Short_Press, on_Fault_Button_2_4_Short_Press,
};

// ���� �ۺ���
void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_Long_Press,  on_Fault_Button_2_Long_Press,  on_Fault_Button_3_Long_Press,  on_Fault_Button_4_Long_Press,
	on_Fault_Button_1_2_Long_Press, on_Fault_Button_1_3_Long_Press,
	on_Fault_Button_2_3_Long_Press, on_Fault_Button_2_4_Long_Press,
};


/* Private user code ---------------------------------------------------------*/


//������
extern TIM_HandleTypeDef htim1;
static void _Delay(uint32_t mdelay)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < mdelay);
}


// ��ʼ��
void App_Fault_Init(void)
{
	p_MB_Fault_State = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER, MB_SYSTEM_FAULT_STATUS );
	
}

// ������
uint8_t If_System_Is_Error(void)
{
	// �����������
	//Motor_ReadFault_Send();
	// ��鱾�ع���
	
	//
	if(*p_System_Fault_Static > 0)
		return 1;
	else
		return 0;
}

// ���ù���ֵ
void Set_Fault_Data(uint16_t type)
{
	
	*p_MB_Fault_State = type;
	
}

//-------------------   ----------------------------

// ��ȡ����
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

// ��ȡ���Ϻ�
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

// ��������˵�
void To_Fault_Menu(void)
{
	//������ͣ, ����ر�
	Set_System_State_Machine(ERROR_DISPLAY_STATUS);

	Fault_Number_Sum = Get_Fault_Number_Sum(*p_MB_Fault_State);
	
	Fault_Number_Cnt = 1;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
	
}

// �������״̬
void Clean_Fault_State(void)
{
	*p_MB_Fault_State = 0;
	
	Fault_Number_Sum = 0;
	
	Fault_Number_Cnt = 0;
	
	To_Free_Mode(1);
}
/* Private function prototypes -----------------------------------------------*/

// ���� ��ʾ
void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint8_t type)
{
	uint8_t i;
	
	// ===============
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// *                            *
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x20;
	fault_send_buffer[0] = 0x2A;
	fault_send_buffer[19] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// sum
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x20;
	fault_send_buffer[0] = 0x2A;
	fault_send_buffer[19] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	
	fault_send_buffer[6] = (sum / 10) + 0x30;
	fault_send_buffer[7] = (sum % 10) + 0x30;
	
	fault_send_buffer[12] = (now / 10) + 0x30;
	fault_send_buffer[13] = (now % 10) + 0x30;
	
	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	
	// time
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x20;
	fault_send_buffer[0] = 0x2A;
	fault_send_buffer[19] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	
	fault_send_buffer[4] = 0x45;	//	E
	fault_send_buffer[5] = 0x72;	//	r
	
	fault_send_buffer[7] = (type / 10) + 0x30;
	fault_send_buffer[8] = (type % 10) + 0x30;

	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// *                            *
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x20;
	fault_send_buffer[0] = 0x2A;
	fault_send_buffer[19] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 21,0xFFFF);
	_Delay(20);
	
	// ===============
	for(i=0; i<20; i++)
		fault_send_buffer[i] = 0x2A;
	fault_send_buffer[20] = 0x0A;
	fault_send_buffer[21] = 0x0A;
	fault_send_buffer[22] = 0x0A;
	fault_send_buffer[23] = 0x0A;
	HAL_UART_Transmit(p_huart_fault, fault_send_buffer, 24,0xFFFF);

	return;
}

/* Private user code ---------------------------------------------------------*/
//------------------- �����ص� ----------------------------

// �� ��λ��
static void on_Fault_Button_1_clicked(void)
{
	if(Fault_Number_Cnt < Fault_Number_Sum)
		Fault_Number_Cnt ++;
	else
		Fault_Number_Cnt = 1;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
}

// �� ʱ���
static void on_Fault_Button_2_clicked(void)
{
	if(Fault_Number_Cnt > 1)
		Fault_Number_Cnt --;
	else
		Fault_Number_Cnt = Fault_Number_Sum;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_MB_Fault_State,Fault_Number_Cnt));
}

// �� ģʽ��
static void on_Fault_Button_3_clicked(void)
{

}


// �� ������  �̰�
static void on_Fault_Button_4_Short_Press(void)
{
	
}

// �� + ��  ��ϼ� 
static void on_Fault_Button_1_2_Short_Press(void)
{
	Clean_Fault_State();
}


// �� + ��  ��ϼ�  �̰�   �л���λ 100�� or 5��
static void on_Fault_Button_1_3_Short_Press(void)
{

}


// �� + ��  ��ϼ�  �̰�   
static void on_Fault_Button_2_3_Short_Press(void)
{
}

// �� + ��  ��ϼ�  �̰�
static void on_Fault_Button_2_4_Short_Press(void)
{
}

//------------------- �����ص�   ���� ----------------------------

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

// �ָ���������
static void on_Fault_Button_2_3_Long_Press(void)
{

}

//
static void on_Fault_Button_2_4_Long_Press(void)
{

}


