/**
******************************************************************************
* @file				tm1621.c
* @brief			tm1621 function implementation
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "tm1621.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

#define NOP __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

/********************** TM1261 ģ������ ********************************/

#define SYSDIS 		0x00
#define SYSEN			0x02
#define LCDOFF		0x04
#define LCDON			0x06
#define TONEON		0x12
#define TONEOFF		0x10

#define TONE_4K		0x80
#define TONE_2K		0xC0

#define XTAL 			0x28
#define RC				0x30
#define BIAS			0x52
#define WDTDIS 		0x0A
#define WDTEN			0x0E

/* Private variables ---------------------------------------------------------*/

uint8_t Lcd_ram[SCREEN_NUMBER_MAX]={0};

// ������ʾ0-9���κ���ʾ���ֵ����򣬶��ǵ������������

//uint8_t Lcd_Number_table_1[10]={
//0x3f,0x06,
//0x5b,0x4f,
//0x66,0x3d,//0x6d
//0x7d,0x07,
//0x7f,0x6f};

// 1-4
uint8_t Lcd_Number_table_1[10]={0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F};

// ���� 5 -8
uint8_t Lcd_Number_table_2[10]={0xF5,0x60,0xD3,0xF2,0x66,0xB6,0xB7,0xE0,0xF7,0xF6};

// ��ĸ��ʾ 1-4
uint8_t Lcd_Letter_table_1[11] = {
	0x00, // A
	0x00, // C
	0x00, // E
	0x00, // F
	0x00, // H
	0x00, // I
	0x00, // L
	0x00, // O
	0x73, // P
	0x00, // S
	0x00  // U
};

// ��ĸ��ʾ 5-8
uint8_t Lcd_Letter_table_2[11] = {
	0x00, // A
	0x00, // C
	0x00, // E
	0x00, // F
	0x00, // H
	0x00, // I
	0x00, // L
	0x00, // O
	0xE3, // P
	0x00, // S
	0x00  // U
};

void Delay_us(uint16_t us)
{ 
	uint16_t i;
	while(us--)
		for(i=0;i<500;i++);
}

/**************************************************************************************
* FunctionName   : TM1621_SendBitMsb()
* Description    : ���ͷ��Ͷ�λ[��λ��ǰ]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitMsb(uint8_t dat, uint8_t cnt)
{
	for (uint8_t i=0; i<cnt; i++)
	{
		(dat & 0x80) ?  TM1621_DATA_HIG() :
                        TM1621_DATA_LOW();
		dat <<= 1;
		TM1621_WR_LOW();
		Delay_us(3);
		TM1621_WR_HIG();
	}
}

/**************************************************************************************
* FunctionName   : TM1621_SendBitLsb()
* Description    : ���Ͷ�λ[��λ��ǰ]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitLsb(uint8_t dat, uint8_t cnt)
{
	for (uint8_t i=0; i<cnt; i++)
	{
		(dat & 0x01) ?  TM1621_DATA_HIG() :TM1621_DATA_LOW();
		dat >>= 1;
		TM1621_WR_LOW();
		Delay_us(3);
		TM1621_WR_HIG();
	}
}

///**************************************************************************************
//* FunctionName   : TM1621_SendCmd()
//* Description    : ��������
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TM1621_SendCmd(uint8_t cmd)
{
    TM1621_CS_LOW();
    TM1621_SendBitMsb(0x80, 3);                                                     // ǰ��3λ�������
    TM1621_SendBitMsb(cmd, 9);                                                      // ����10λ: a5~a0[RAM��ַ]+d3~d0[RAM����]
    TM1621_CS_HIG();
}

/**************************************************************************************
* FunctionName   : HTBSendNDat()
* Description    : ����N����
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendNDat(uint8_t addr, uint8_t *pDat, uint8_t cnt, uint8_t bitNum)
{
    TM1621_CS_LOW();
    TM1621_SendBitMsb(0xA0, 3);                                                    // ǰ��3λ�������
    TM1621_SendBitMsb(addr<<2, 6);                                                 // a5~a0[RAM��ַ]

    for (uint8_t i=0; i<cnt; i++)
    {        
			TM1621_SendBitLsb(*pDat++, bitNum);   // RAM��TM1621_SendBitLsb��
    }
    TM1621_CS_HIG();
}



/**
  * @brief  TM1621 Write CMD.
  * @param  cmd ָ��д�������.
  * @return void
  */
void TM1621_Write_CMD(uint8_t cmd)
{
	TM1621_CS_LOW();
  TM1621_SendBitMsb(0x80, 4);                                                    // ǰ��3λ�������
  TM1621_SendBitMsb(cmd, 8);                                                 // a5~a0[RAM��ַ]
  TM1621_CS_HIG();
}

/*
******************************************************************************
TM1621_display_number	

��ʾ����
******************************************************************************
*/  
void TM1621_display_number(uint8_t coordinate, uint8_t value)
{
	uint8_t symbol;
	//uint8_t ram;
	
	if(coordinate < 4)
	{
		symbol = Lcd_ram[coordinate] & 0x08;
		if(value == 0xFF)// ����ʾ
			Lcd_ram[coordinate] = 0;
		else
			Lcd_ram[coordinate] = Lcd_Number_table_2[value];
		
		Lcd_ram[coordinate] |= symbol;
		
	}
	else
	{
		symbol = Lcd_ram[coordinate] & 0x80;
		if(value == 0xFF)// ����ʾ
			Lcd_ram[coordinate] = 0;
		else
			Lcd_ram[coordinate] = Lcd_Number_table_1[value];
		
		Lcd_ram[coordinate] |= symbol;
	}
		
}

/*
******************************************************************************
TM1621_display_Letter	

��ʾ ��ĸ
******************************************************************************
*/  
void TM1621_display_Letter(uint8_t coordinate, uint8_t value)
{
	uint8_t symbol;
	//uint8_t ram;
	
	if(coordinate < 4)
	{
		symbol = Lcd_ram[coordinate] & 0x08;
		if(value == 0xFF)// ����ʾ
			Lcd_ram[coordinate] = 0;
		else
			Lcd_ram[coordinate] = Lcd_Letter_table_2[value];
		
		Lcd_ram[coordinate] |= symbol;
		
	}
	else
	{
		symbol = Lcd_ram[coordinate] & 0x80;
		if(value == 0xFF)// ����ʾ
			Lcd_ram[coordinate] = 0;
		else
			Lcd_ram[coordinate] = Lcd_Letter_table_1[value];
		
		Lcd_ram[coordinate] |= symbol;
	}
		
}
/*
******************************************************************************
TM1621_Show_Colon	

��ʾ����
******************************************************************************
*/  
void TM1621_Show_Symbol(uint8_t coordinate, uint8_t value)
{
	
	if(coordinate < 4)
	{
		if(value)
			Lcd_ram[coordinate]  |= 0x08;
		else
			Lcd_ram[coordinate]  &= 0xF7;
		
	}
	else
	{
		if(value)
			Lcd_ram[coordinate]  |= 0x80;
		else
			Lcd_ram[coordinate]  &= 0x7F;
	}
}

/*
******************************************************************************
TM1621_LCD_Redraw	

��Ļˢ��
******************************************************************************
*/ 
void TM1621_LCD_Redraw(void)
{
	//����
	TM1621_BLACK_ON();
	
 	TM1621_SendNDat(0,Lcd_ram,SCREEN_NUMBER_MAX,SCREEN_NUMBER_MAX);
}

/*
******************************************************************************
TM1621_Show_All	

ȫ��ʾ
******************************************************************************
*/ 
void TM1621_Show_All(void)
{
	uint8_t show_all[SCREEN_NUMBER_MAX] = { 0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xFF};
	
	memcpy(Lcd_ram, show_all, SCREEN_NUMBER_MAX);
	
 	TM1621_LCD_Redraw();
}
/*
******************************************************************************
TM1621_Show_Off	

Ϣ��
******************************************************************************
*/ 
void TM1621_Show_Off(void)
{	
	memset(Lcd_ram, 0, SCREEN_NUMBER_MAX);
	
 	TM1621_LCD_Redraw();
}
/*
******************************************************************************
TM1621_Show_Off	

Ϣ��
******************************************************************************
*/ 
void TM1621_Buzzer_Off(void)
{	
	TM1621_Write_CMD(TONEOFF);
}
/*
******************************************************************************
TM1621_Show_Off	

Ϣ��
******************************************************************************
*/ 
void TM1621_Buzzer_On(void)
{	
	TM1621_Write_CMD(TONEON);
}

// ����  ���ɴ��,������Ҫ�����
void TM1621_Buzzer_Whistle(uint16_t us) 
{
	TM1621_Buzzer_On();
	Delay_us(us);
	TM1621_Buzzer_Off();
}

// ��һ��
void TM1621_Buzzer_Click(void) 
{
	TM1621_Buzzer_On();
	TM1621_Buzzer_Off();
}

void TM1621_Buzzer_Init(void) 
{
	Delay_us(1);
	//������
	TM1621_Write_CMD(TONE_4K);
	TM1621_Write_CMD(TONEOFF);
}
/*
******************************************************************************
LCD_Init	
��ʼ�����HT1621 
******************************************************************************
*/  
 
void TM1621_LCD_Init(void) 
{	
	TM1621_CS_HIG();
	TM1621_DATA_HIG();
	TM1621_WR_HIG();
	
	NOP
	Delay_us(1);
	
	TM1621_Write_CMD(BIAS);
	TM1621_Write_CMD(RC);
	TM1621_Write_CMD(SYSDIS);
	TM1621_Write_CMD(WDTDIS);
	TM1621_Write_CMD(SYSEN);
	TM1621_Write_CMD(LCDON);
	
}









