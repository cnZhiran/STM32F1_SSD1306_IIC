/**
  ******************************************************************************
  * @file    oled.h
  * @author  Zhiran
  * @date    2021-8-3
  ******************************************************************************
  */
#include "oled.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WaitTime 0x1000


uint8_t OLED_GRAM[OLED_PAGE_NUM][OLED_SIZE_Y];
float fps;
int continuous_refresh=0,dma1_l6_busy=0;

void OLED_DMAConfig(void);


/**
  * @brief  ͨ��IIC�ӿ���SSD1306��������
  * @param  o_command: ��������
	* @note   �ú�������Ҫ���ⲿ����
  * @retval ������������򷵻�-1�����򷵻�0
  */
int OLED_send_cmd(unsigned char o_command)
{
	int timeout = WaitTime;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(I2C1, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(I2C1, 0x00);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(I2C1, o_command);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	timeout = WaitTime;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	return 0;
}

/**
  * @brief  ͨ��IIC�ӿ���SSD1306��������
  * @param  o_data: ��������
	* @note   �ú�������Ҫ���ⲿ����
  * @retval ������������򷵻�-1�����򷵻�0
  */
int OLED_send_data(unsigned char o_data)
{
	int timeout = WaitTime;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(I2C1, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(I2C1, 0x40);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(I2C1, o_data);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	timeout = WaitTime;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	return 0;
}

/**
  * @brief  OLED�ĳ�ʼ������
  * @param  ��
  * @retval ��
  */				    
void OLED_Init(void)
{
	int i;
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x78;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_Cmd(I2C1, ENABLE);
	
	for(i=0;i<27;i++) {
		while(OLED_send_cmd(OLED_init_cmd[i]));
	}
	OLED_Clear();
	
	OLED_DMAConfig();
}

/**
  * @brief  DMA���ú���
  * @param  ��
  * @retval ��
  */				 
void OLED_DMAConfig(void) {
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(I2C1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)OLED_GRAM[0];
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = OLED_PAGE_NUM * OLED_SIZE_Y;
	
	//DMA_DeInit(DMA1_Channel6);
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC|DMA_IT_TE, ENABLE);
}

/**
  * @brief  ����OLED��ʾ����
  * @param  ��
  * @retval ��
  */
void OLED_Display_On(void)
{
	while(OLED_send_cmd(0X8D));  //SET DCDC����
	while(OLED_send_cmd(0X14));  //DCDC ON
	while(OLED_send_cmd(0XAF));  //DISPLAY ON
}

/**
  * @brief  �ر�OLED��ʾ����
  * @param  ��
  * @retval ��
  */ 
void OLED_Display_Off(void)
{
	while(OLED_send_cmd(0X8D));  //SET DCDC����
	while(OLED_send_cmd(0X10));  //DCDC OFF
	while(OLED_send_cmd(0XAE));  //DISPLAY OFF
}

/**
  * @brief  ����ҳ��ַ������У�����
	* @param  page: ҳ��ַ��ҳ��ַ�������
  * @retval ��
  */
void OLED_SetPage(uint8_t page)
{
	assert_param(IS_OLED_PAGE(page));
	
	while(OLED_send_cmd(0xb0+page));
}
/**
  * @brief  �����е�ַ������У�����
	* @param  col: �е�ַ���е�ַ�������
  * @retval ��
  */
void OLED_SetColumn(uint8_t col)
{
	assert_param(IS_OLED_COLUMN(col));
	
	while(OLED_send_cmd(0x10|(col>>4)));		//�����е�ַ��λ
	while(OLED_send_cmd(0x00|(col&0x0f)));	//�����е�ַ��λ
}

/**
  * @brief  ���ù��λ�ú���
	* @param  page: ҳ��ַ��ҳ��ַ�������
	* @param  col: �е�ַ���е�ַ�������
  * @retval ��
  */
void OLED_SetPos(uint8_t page, uint8_t col)
{
	assert_param(IS_OLED_PAGE(page));
	assert_param(IS_OLED_COLUMN(col));
	
	while(OLED_send_cmd(0xb0+page));
	while(OLED_send_cmd(0x10|(col>>4)));
	while(OLED_send_cmd(0x00|(col&0x0f)));
}

/**
  * @brief  ��������
	* @param  ��
  * @retval ��
  */
void OLED_Clear(void)
{
	unsigned char page,col;
	for(page=0; page<OLED_PAGE_NUM; page++) {
		OLED_SetPos(page, 0);
		for(col=0; col<OLED_SIZE_Y; col++) {
			while(OLED_send_data(0x00));
		}
	}
}
/**
  * @brief  ��亯��
	* @param  val: Ҫȫ����������
  * @retval ��
  */
void OLED_Full(uint8_t val)
{
	unsigned char page,column;
	for(page=0; page<OLED_PAGE_NUM; page++) {
		OLED_SetPos(page, 0);
		for(column=0; column<OLED_SIZE_Y; column++) {
			while(OLED_send_data(val));
		}
	}
}
//��ʾASC�ַ�
//page:0~7
//col:0~127
//str:�ַ����׵�ַ
void OLED_ShowASCIIString(uint8_t page, uint8_t col, char *str)
{
	uint8_t c=0,i=0,j=0;

	while (str[j]!='\0')
	{
		c = str[j]-32;
		if(col>120){col=0;page++;}
		if(page>7){col=page=0;OLED_Clear();}
		OLED_SetPos(page,col);
		for(i=0;i<8;i++)
			OLED_send_data(oled_ascii_0816[c*16+i]);
		OLED_SetPos(page+1,col);
		for(i=8;i<16;i++)
			OLED_send_data(oled_ascii_0816[c*16+i]);
		col+=8;
		j++;
	}
}
void OLED_ShowCHZHString(uint8_t page, uint8_t col, char *str)
{
	uint8_t i,j;
	
	while(*str != '\0') {
		for(i=0; i<CNZHNUM; i++) {
			if(str[0] == oled_cnzh_1616[i].index[0] && str[1] == oled_cnzh_1616[i].index[1]) {
				break;
			}
		}
		if(i != CNZHNUM) {	//��ʾ����
			if(col>112){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			OLED_SetPos(page, col);
			for(j = 0;j < 16;j++)
			{
				OLED_send_data(oled_cnzh_1616[i].font[j]);
			}	
			OLED_SetPos(page+1, col);
			for(j = 16;j < 32;j++)
			{
				OLED_send_data(oled_cnzh_1616[i].font[j]);
			}
			str += 2;
			col += 16;
		}else {	//��ʾ�ո�
			if(col>120){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			OLED_SetPos(page , col);
			for(j = 0;j < 8;j++)
			{
				OLED_send_data(0);
			}	
			OLED_SetPos(page+1 , col);
			for(j = 0;j < 8;j++)
			{
				OLED_send_data(0);
			}
			str += 1;
			col += 8;
		}
	}
}

void OLED_ShowString(uint8_t page, uint8_t col, char *str)
{
	char chr[3];

	while(*str != '\0') {
		if(*str > 127) {
			if(col>112){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = str[1];
			chr[2] = '\0';          //����Ϊ�����ֽ�
			OLED_ShowCHZHString(page, col, chr);  //��ʾ����
			str += 2;
			col += 16;
		}else {
			if(col>120){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = '\0';          //��ĸռһ���ֽ�
			OLED_ShowASCIIString(page, col, chr);  //��ʾ��ĸ
			str += 1;
			col += 8;
		}
	}
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t page,temp=0;
	if(x>127||y>63)return;//������Χ��.
	page = y/8;
	temp=1<<(y % 8);
	if(t)OLED_GRAM[page][x]|=temp;
	else OLED_GRAM[page][x]&=~temp;	    
}

////��ָ��λ�û���һ���ַ�,���������ַ�
////x:0~127
////y:0~63
////mode:0,������ʾ;1,������ʾ				 
////size:ѡ������ 16/12 
//void OLED_DrawChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
//{      			    
//	uint8_t temp,i,j;
//	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		
//	 
//	if(size==12) {//����0612���� 
//		for(i=0;i<6;i++) {  
//			temp=oled_ascii_0612[chr*12+i];
//			OLED_DrawByte(x+i, y, temp, mode);
////			for(j=0;j<8;j++) {
////				if(temp & 0x01)
////					OLED_DrawPoint(x+i,y+j,mode);
////				else 
////					OLED_DrawPoint(x+i,y+j,!mode);
////				temp>>=1;
////			}
//		}
//		for(i=0;i<6;i++) {  
//			temp=oled_ascii_0612[chr*12+i+6];                         
//			for(j=8;j<12;j++) {
//				if(temp & 0x01)
//					OLED_DrawPoint(x+i,y+j,mode);
//				else 
//					OLED_DrawPoint(x+i,y+j,!mode);
//				temp>>=1;
//			}
//		}
//	}
//	else {//����0816����
//		for(i=0;i<8;i++) {
//			temp=oled_ascii_0816[chr*16+i];
//			OLED_DrawByte(x+i, y, temp, mode);
//		}
//		for(i=0;i<8;i++) {
//			temp=oled_ascii_0816[chr*16+i+8];
//			OLED_DrawByte(x+i, y+8, temp, mode);
//		}
//	}
//}
//��ָ��λ�û���һ��ASCII�ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_DrawASCIIChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{      			    
	uint8_t temp,i,j;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		
	 
	if(size==12) {//����0612���� 
		for(i=0;i<6;i++) {  
			temp=oled_ascii_0612[chr*12+i];  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<6;i++) {  
			temp=oled_ascii_0612[chr*12+i+6];                         
			for(j=8;j<12;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
	}
	else {//����0816����
		for(i=0;i<8;i++) {
			temp=oled_ascii_0816[chr*16+i];		  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<8;i++) {
			temp=oled_ascii_0816[chr*16+i+8];                         
			for(j=8;j<16;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
	}
}
void OLED_DrawByte(uint8_t x, uint8_t y, uint8_t data, uint8_t mode) {
	uint8_t page,bias,temp,mask;
	if(x>127||y>56)return;//������Χ��.
	page = y/8;
	bias = y%8;
	if(!mode) data = ~data;
	temp=data<<bias;
	mask=0xff<<bias;
	OLED_GRAM[page][x]|=temp & mask;
	OLED_GRAM[page][x]&=temp | ~mask;
	if(bias) {
		page++;
		temp = data >>(8-bias);
		mask = 0xff >>(8-bias);
		OLED_GRAM[page][x]|=temp & mask;
		OLED_GRAM[page][x]&=temp | ~mask;
	}
}
//��ָ��λ�û���һ�������ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_DrawCNZHString(uint8_t x, uint8_t y, char *str, uint8_t size, uint8_t mode)
{
	uint8_t i,j;
	
	while(*str != '\0') {
		if(x > 127) return;
		for(i=0; i<CNZHNUM; i++) {
			if(str[0] == oled_cnzh_1616[i].index[0] && str[1] == oled_cnzh_1616[i].index[1]) {
				break;
			}
		}
		if(i != CNZHNUM) {	//��ʾ����
			for(j = 0;j < 16;j++) {
				OLED_DrawByte(x+j, y, oled_cnzh_1616[i].font[j], mode);
			}
			for(j = 0;j < 16;j++) {
				OLED_DrawByte(x+j, y+8, oled_cnzh_1616[i].font[j+16], mode);
			}
			str += 2;
			x += 16;
		}else {	//��ʾ�ո�
			for(j = 0;j < 8;j++) {
				OLED_DrawByte(x+j, y, 0x00, mode);
				OLED_DrawByte(x+j, y+8, 0x00, mode);
			}
			str += 1;
			x += 8;
		}
	}
}

//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//size:�����С
//mode:0,������ʾ;1,������ʾ
void OLED_DrawString(uint8_t x, uint8_t y,char *str, uint8_t size, uint8_t mode)
{
	char chr[3];
	if(size == 12) {
    while(*str!='\0')
    {       
			if(x>120){x=0;y+=12;}
			if(y>58){y=x=0;}
			OLED_DrawASCIIChar(x,y,*str,12,mode);	 
			x+=6;
			str++;
    } 
	}else {
		while(*str != '\0') {
			if(*str > 127) {
				if(x>112){x=0;y+=16;}
				if(y>58){x=y=0;}
				chr[0] = str[0];
				chr[1] = str[1];
				chr[2] = '\0';          //����Ϊ�����ֽ�
				OLED_DrawCNZHString(x, y, chr, 16, mode);  //��ʾ����
				str += 2;
				x += 16;
			}else {
				if(x>120){x=0;y+=16;}
				if(y>58){x=y=0;}
				chr[0] = str[0];
				chr[1] = '\0';          //��ĸռһ���ֽ�
				OLED_DrawASCIIChar(x, y, chr[0], 16 ,mode);  //��ʾ��ĸ
				str += 1;
				x += 8;
			}
		}
	}
}

//���Ĳ������������Դ���ơ�ɽ���Ƽ���ѧ������
void OLED_CNZH_Example(void) {
	OLED_DrawString( 0+ 0+1, 0, "ɽ", 16 ,1);
	OLED_DrawString(16+ 6+1, 0, "��", 16 ,1);
	OLED_DrawString(32+12+1, 0, "��", 16 ,1);
	OLED_DrawString(48+18+1, 0, "��", 16 ,1);
	OLED_DrawString(64+24+1, 0, "��", 16 ,1);
	OLED_DrawString(80+30+1, 0, "ѧ", 16 ,1);
	OLED_DrawString(16, 20, "ɽ���Ƽ���ѧ", 16 ,0);
	OLED_DrawString(16, 40, "github: cnZhiran", 12 ,1);
}
//֡�ʡ�CPUռ���ʲ������������Դ���������ģ��İ�����
#ifdef	COMPUT_TRANS_FPS
void OLED_Simulated_Noise_Example(void) {
	int x,y,m;
	char chr[17];
	if(SYSTIME_VARIABLE % 20000 == 0) {
		sprintf(chr, "rate:%8.2ffps", fps);
		OLED_DrawString(0, 0, chr, 16, 1);
	}
	
	//���Դ���������ƺڵ��׵�
	x = rand() %OLED_SIZE_Y;
	y = rand() %(OLED_SIZE_X-16) +16;
	m = rand() %2;
	OLED_DrawPoint(x,y,m);
}
#else		/* COMPUT_TRANS_FPS */
void OLED_Simulated_Noise_Example(void) {
	int x,y,m;
	
	//���Դ���������ƺڵ��׵�
	x = rand() %OLED_SIZE_Y;
	y = rand() %OLED_SIZE_X;
	m = rand() %2;
	OLED_DrawPoint(x,y,m);
}
#endif	/* COMPUT_TRANS_FPS */

//֡�ʲ������������Դ������ͬ֡�ʵ��ƶ�������
void OLED_Frame_Example(void) {
#if (OLED_SIZE_Y >= 128)
	unsigned char pos = SYSTIME_VARIABLE *SYSTIME_PERIOD /10000 %109;
#elif (OLED_SIZE_Y >= 96)
	unsigned char pos = SYSTIME_VARIABLE *SYSTIME_PERIOD /10000 %73;
#elif (OLED_SIZE_Y >= 64)
	unsigned char pos = SYSTIME_VARIABLE *SYSTIME_PERIOD /10000 %49;
#elif (OLED_SIZE_Y >= 48)
	unsigned char pos = SYSTIME_VARIABLE *SYSTIME_PERIOD /10000 %25;
#endif
	static unsigned char pos_l=0;
	if(pos_l != pos) {
		if(pos % 2 == 0){
			pos_l = pos;
			memset(OLED_GRAM[0], 0x00, pos);
			memset(OLED_GRAM[1], 0x00, pos);
			OLED_DrawChar(pos   , 0, '5', 16, 1);
			OLED_DrawChar(pos+ 8, 0, '0', 16, 1);
		}
		if(pos % 3 == 0){
			memset(OLED_GRAM[2], 0x00, pos);
			memset(OLED_GRAM[3], 0x00, pos);
			OLED_DrawChar(pos   , 16, '3', 16, 1);
			OLED_DrawChar(pos+ 8, 16, '3', 16, 1);
		}
		if(pos % 4 == 0){
			memset(OLED_GRAM[4], 0x00, pos);
			memset(OLED_GRAM[5], 0x00, pos);
			OLED_DrawChar(pos   , 32, '2', 16, 1);
			OLED_DrawChar(pos+ 8, 32, '5', 16, 1);
		}
		if(pos % 6 == 0){
			memset(OLED_GRAM[6], 0x00, pos);
			memset(OLED_GRAM[7], 0x00, pos);
			OLED_DrawChar(pos   , 48, '1', 16, 1);
			OLED_DrawChar(pos+ 8, 48, '7', 16, 1);
		}
	}
}

#ifdef	USE_U8G2_EXAMPLE
void OLED_Example_Loop(u8g2_t *u8g2) {
	unsigned char stat=SYSTIME_VARIABLE *SYSTIME_PERIOD /10000000 %4;
	static unsigned char stat_l=0;
	if(stat_l != stat) {
		OLED_Clear_Gram();
		stat_l = stat;
	}
	switch(stat) {
		case 0:
			OLED_Frame_Example();
			break;
		case 1:
			OLED_Simulated_Noise_Example();
			break;
		case 2:
			OLED_CNZH_Example();
			break;
		case 3:
			OLED_U8G2_Draw_Example(u8g2);
			break;
		default:
			break;
	}
}
#else		/* USE_U8G2_EXAMPLE */
void OLED_Example_Loop(void) {
	unsigned char stat=SYSTIME_VARIABLE *SYSTIME_PERIOD /10000000 %3;
	static unsigned char stat_l=0;
	if(stat_l != stat) {
		OLED_Clear_Gram();
		stat_l = stat;
	}
	switch(stat) {
		case 0:
			OLED_Frame_Example();
			break;
		case 1:
			OLED_Simulated_Noise_Example();
			break;
		case 2:
			OLED_CNZH_Example();
			break;
		default:
			break;
	}
}
#endif	/* USE_U8G2_EXAMPLE */

//��պ���	  
void OLED_Clear_Gram(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[i][n]=0x00;
}
//ˢ�º���
int OLED_Refresh_Gram(int sync)
{
	static uint32_t times_l;
	int timeout;
	
#ifndef	SYSTIME_DOWN_CONT
	int32_t	times_bias = SYSTIME_VARIABLE - times_l;
#else		/* SYSTIME_DOWN_CONT */
	int32_t	times_bias = times_l - SYSTIME_VARIABLE;
#endif	/* SYSTIME_DOWN_CONT */
	if(times_bias < 0) times_bias += SYSTIME_SIZE;
	fps = 1000000.0 /SYSTIME_PERIOD /times_bias;
	times_l = SYSTIME_VARIABLE;
	
	while(OLED_send_cmd(0xb0));				//����ҳ��ַ��0~7��
	while(OLED_send_cmd(0x00));				//������ʾλ�á��е͵�ַ
	while(OLED_send_cmd(0x10));				//������ʾλ�á��иߵ�ַ 
	
	OLED_DMAConfig();
	
	timeout = WaitTime;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(I2C1, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(I2C1, 0x40);
	timeout = WaitTime;
	while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF))) {
		if(--timeout == 0) return -1;
	}
	dma1_l6_busy = 1;
	I2C_DMACmd(I2C1, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	if(sync){
		timeout = WaitTime;
		while(dma1_l6_busy) if(--timeout == 0) return -1;
	}
	
	return 0;
}

int OLED_Continuous_Refresh_Start(void) {
	continuous_refresh = 1;
	return OLED_Refresh_Gram(0);
}

void OLED_Continuous_Refresh_Stop(void) {
	continuous_refresh = 0;
}

void DMA1_Channel6_IRQHandler() {
	int timeout = WaitTime;
	if(DMA_GetITStatus(DMA1_IT_TC6)) {
		while (--timeout && !I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
		
		I2C_GenerateSTOP(I2C1, ENABLE);
		timeout = WaitTime;
		while(--timeout && I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
		I2C_DMACmd(I2C1, DISABLE);
		DMA_Cmd(DMA1_Channel6, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC6);
		(void) I2C1->SR1;
		(void) I2C1->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_l6_busy = 0;
	}else if(DMA_GetITStatus(DMA_IT_TE)) {
		DMA_DeInit(DMA1_Channel6);
		(void) I2C1->SR1;
		(void) I2C1->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_l6_busy = 0;
	}
}

uint8_t u8x8_msg_null_cb(u8x8_t *u8x8, uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr) {
	return NULL;
}

u8g2_t *OLED_U8G2_Init(void) {
	static u8g2_t u8g2;
	uint8_t *buffer = (uint8_t *)OLED_GRAM;
	
	u8g2_SetupDisplay(&u8g2, u8x8_d_ssd1306_128x64_noname, u8x8_msg_null_cb, u8x8_msg_null_cb, u8x8_msg_null_cb);
  u8g2_SetupBuffer(&u8g2, buffer, 8, u8g2_ll_hvline_vertical_top_lsb, U8G2_R0);
	return &u8g2;
}

void OLED_U8G2_Draw_Example(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
}
