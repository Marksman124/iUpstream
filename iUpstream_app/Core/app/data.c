/**
******************************************************************************
* @file				data.c
* @brief			���ݹ���ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "data.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

Operating_Parameters OP_ShowNow;

Operating_Parameters* p_OP_ShowLater;

// ѵ��ģʽ ��ǰ״̬
uint8_t PMode_Now = 0;

uint8_t Period_Now = 0;

// ��ģʽ ���� ��ʼֵ
Operating_Parameters OP_Init_Free = { 60 , 0};

Operating_Parameters OP_Init_Timing = { 60 , 1800};

Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX] = {
{{30,120},{40,300},{30,360},{80,240},{45,540},{30,600},{40,780},{30,900}},
{{50,180},{60,360},{50,480},{70,720},{50,780},{60,1020},{50,1200}},
{{70,300},{80,540},{70,600},{85,840},{70,900},{80,1200},{70,1500}},
{{50,420},{65,1440},{50,1800}}
};

	
// ��ģʽ ����
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;
	
uint16_t* p_System_Fault_Static;			//����״̬
uint16_t* p_Motor_Fault_Static;				//����״̬
uint16_t* p_Mos_Temperature;					//mos �¶�
uint16_t* p_Motor_Temperature;				//��� �¶�
uint32_t* p_Motor_Current;						//��� ����
uint32_t* p_Motor_Reality_Speed;			//��� ʵ�� ת��
uint16_t* p_Motor_Bus_Voltage;				//ĸ�� ��ѹ
	
	
uint16_t* p_Modbus_Node_Addr;					//��ַ
uint16_t* p_Modbus_Baud_Rate;					//������
uint16_t* p_Support_Control_Methods;	//���ο��Ʒ�ʽ
uint16_t* p_Motor_Pole_Number;				//�������
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//���״̬
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------

// ��ʼ�� ����ִ��
void Check_Data_Init(void)
{
	static uint8_t x=0,y=0;
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
	}
	*p_Modbus_Node_Addr = 21;
	*p_Modbus_Baud_Rate = 4;
	*p_Support_Control_Methods = 0;
	
	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	}
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				return;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					return;
				}
			}
		}
	}
	
	
}


extern void Get_Mapping_Register(void);
// ��ʼ��
void App_Data_Init(void)
{
	// ��ȡӳ��  flash�Ѷ�
	Get_Mapping_Register();
	
	// ѵ��ģʽ ��ǰ״̬
	PMode_Now = 0;
	Period_Now = 0;
	
	// ��ģʽ ����
	Check_Data_Init();
	
	//�洢  ��һ�� ���� ������
	Memset_OPMode();
	
	// ��Ļ��ʼ��
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
}


// �ָ� ��ʼ��
void App_Data_ReInit(void)
{
	// ѵ��ģʽ ��ǰ״̬
	PMode_Now = 0;
	Period_Now = 0;
	
	// ��ģʽ ����
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;
	
	*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode));
	
	//�洢  ��һ�� ���� ������
	Memset_OPMode();
}

// �� flash
extern void MB_Flash_Buffer_Write(void);
uint8_t Memset_OPMode(void)
{
	MB_Flash_Buffer_Write();
	return 1;
}

//�洢 ������
void Update_OP_Data(void)
{
	
	if(System_State_Machine <= FREE_MODE_STOP)	// ����
	{
		p_OP_Free_Mode->speed = OP_ShowNow.speed;
	}
	else if(System_State_Machine <= TIMING_MODE_STOP)	// ��ʱ
	{
		p_OP_Timing_Mode->speed = OP_ShowNow.speed;
		p_OP_Timing_Mode->time = OP_ShowNow.time;
	}
	else if(System_State_Machine <= TRAINING_MODE_STOP)	// ѵ��
	{
		if(Is_Mode_Legal(PMode_Now))
			p_OP_PMode[PMode_Now-1][Period_Now-1].speed = OP_ShowNow.speed;
	}
	
	//�洢  ��һ�� ���� ������
	Memset_OPMode();
}

//------------------- �ж� ģʽ �Ϸ� ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode)
{
	if((mode > 0) && (mode <= TRAINING_MODE_NUMBER_MAX) )
	{
		return 1;
	}
	else
		return 0;
}
//------------------- �ж��ٶ� �Ϸ� ----------------------------
uint8_t Is_Speed_Legal(uint16_t speed)
{
	if((speed >= SPEED_LEGAL_MIN) && (speed <= SPEED_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- �ж�ʱ�� �Ϸ� ----------------------------
uint8_t Is_Time_Legal(uint16_t time)
{
	if((time >= TIME_LEGAL_MIN) && (time <= TIME_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- ���� ��ǰ �ٶ� ----------------------------
void Data_Set_Current_Speed(uint8_t speed)
{
	//if(System_is_Starting())
		//return;
	
	OP_ShowNow.speed = speed;	
	Motor_Speed_Target_Set(speed);
	
	if(System_is_Running())
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
}

//------------------- ���� ��ǰ ʱ�� ----------------------------
void Data_Set_Current_Time(uint16_t time)
{
	if(System_is_Starting())
		return;
	
	OP_ShowNow.time = time;
}
