/**
  ******************************************************************************
  * @file    oled.h
  * @author  Zhiran
  * @date    2021-8-3
  * @brief   This file provides the IIC-OLED functions.
  ******************************************************************************
	* @attention
	*
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
	* ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
  *
  ******************************************************************************
  */
#include "oled.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WaitTime 0x1000

uint8_t OLED_GRAM[OLED_PAGE_NUM][OLED_SIZE_X];

#ifdef	OLED_COMPUT_TRANS_FPS
float fps;
#endif	/* OLED_COMPUT_TRANS_FPS */

static int continuous_refresh=0,dma1_busy=0;

void OLED_DMAConfig(void);


/**
  * @brief  ͨ��IIC�ӿ���SSD1306��������
  * @param  o_command: ��������
  * @retval ������������򷵻�-1�����򷵻�0
  */
int OLED_send_cmd(uint8_t o_command)
{
	int timeout = WaitTime;
	while(I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(OLED_IICx, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(OLED_IICx, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(OLED_IICx, 0x00);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(OLED_IICx, o_command);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTOP(OLED_IICx, ENABLE);
	timeout = WaitTime;
	while(I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	(void) OLED_IICx->SR1;
	(void) OLED_IICx->SR2;
	return 0;
}

/**
  * @brief  ͨ��IIC�ӿ���SSD1306��������
  * @param  o_data: ��������
  * @retval ������������򷵻�-1�����򷵻�0
  */
int OLED_send_data(uint8_t o_data)
{
	int timeout = WaitTime;
	while(I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(OLED_IICx, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(OLED_IICx, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(OLED_IICx, 0x40);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(OLED_IICx, o_data);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTOP(OLED_IICx, ENABLE);
	timeout = WaitTime;
	while(I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	(void) OLED_IICx->SR1;
	(void) OLED_IICx->SR2;
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
	
  GPIO_InitStructure.GPIO_Pin = OLED_IIC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OLED_IIC_GPIOx, &GPIO_InitStructure);
	
#ifdef OLED_IIC_REMAP
	GPIO_PinRemapConfig(OLED_IIC_REMAP, ENABLE);
#endif /* OLED_IIC_REMAP */
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x78;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Init(OLED_IICx, &I2C_InitStructure);
	
	I2C_Cmd(OLED_IICx, ENABLE);
	
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
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(OLED_IICx->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)OLED_GRAM[0];
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = OLED_PAGE_NUM * OLED_SIZE_X;
	
#ifdef OLED_DMA1_Channel6
	//DMA_DeInit(DMA1_Channel6);
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC|DMA_IT_TE, ENABLE);
#endif /* OLED_DMA1_Channel6 */
	
#ifdef OLED_DMA1_Channel4
	//DMA_DeInit(DMA1_Channel4);
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC|DMA_IT_TE, ENABLE);
#endif /* OLED_DMA1_Channel4 */
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
		for(col=0; col<OLED_SIZE_X; col++) {
			while(OLED_send_data(0x00));
		}
	}
}

#ifdef	USE_PAGE
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
		for(column=0; column<OLED_SIZE_X; column++) {
			while(OLED_send_data(val));
		}
	}
}

/**
  * @brief  ��SSD1306д��ASCII�ַ���
	* @param  page��ҳ��λ��
	* @param  col���е�λ��
	* @param  str���ַ������׵�ַ
  * @retval ��
  */
void OLED_ShowASCIIString(uint8_t page, uint8_t col, char *str)
{
	uint8_t c=0,i=0,j=0;

	while (str[j]!='\0')
	{
		c = str[j]-32;
		if(c > sizeof(oled_ascii_0816)/16) c = 0;
		if(col>OLED_SIZE_X-8){col=0;page++;}
		if(page>OLED_PAGE_NUM-2){col=page=0;OLED_Clear();}
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

/**
  * @brief  ��SSD1306д��GBK˫�ֽ��ַ���
	* @param  page��ҳ��λ��
	* @param  col���е�λ��
	* @param  str���ַ������׵�ַ
  * @retval ��
  */
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
			if(col>OLED_SIZE_X-16){col=0;page+=2;}
			if(page>OLED_PAGE_NUM-2){col=page=0;OLED_Clear();}
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
			if(col>OLED_SIZE_X-16){col=0;page+=2;}
			if(page>OLED_PAGE_NUM-2){col=page=0;OLED_Clear();}
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
/**
  * @brief  ��SSD1306д��GBK�ַ���
	* @param  page��ҳ��λ��
	* @param  col���е�λ��
	* @param  str���ַ������׵�ַ
  * @retval ��
  */
void OLED_ShowString(uint8_t page, uint8_t col, char *str)
{
	char chr[3];

	while(*str != '\0') {
		if(*str > 127) {
			if(col>OLED_SIZE_X-16){col=0;page+=2;}
			if(page>OLED_PAGE_NUM-2){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = str[1];
			chr[2] = '\0';          //����Ϊ�����ֽ�
			OLED_ShowCHZHString(page, col, chr);  //��ʾ����
			str += 2;
			col += 16;
		}else {
			if(col>OLED_SIZE_X-8){col=0;page+=2;}
			if(page>OLED_PAGE_NUM-2){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = '\0';          //ASCIIռһ���ֽ�
			OLED_ShowASCIIString(page, col, chr);  //��ʾASCII�ַ�
			str += 1;
			col += 8;
		}
	}
}
#endif	/* USE_PAGE */


#ifdef	USE_GRAM_DRAW_BASE_FUNCTION
/**
  * @brief  ���Դ����һ����
	* @param  x���е�λ��
	* @param  y���е�λ��
	* @param  mode�������ʽ
	*		@arg	0�����
	*		@arg	1�����
	*		@arg	2��ȡ��
  * @retval ��
  */		   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)
{
	uint8_t page,temp=0;
	if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
	page = y/8;
	temp=1<<(y % 8);
	if(mode == 2) OLED_GRAM[page][x]^=temp;
	else if(mode == 1)OLED_GRAM[page][x]|=temp;
	else OLED_GRAM[page][x]&=~temp;	    
}

/**
  * @brief  ���Դ����һ���ֽ�
	* @param  x���е�λ��
	* @param  y���е�λ��
	* @param  data�����µ�����
	* @param  mode����ʽ
	*		@arg	0���ڵ�
	*		@arg	1������
  * @retval ��
  */
void OLED_DrawByte(uint8_t x, uint8_t y, uint8_t data, uint8_t mode) {
	uint8_t page,bias,temp,mask;
	if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
	page = y/8;
	bias = y%8;
	if(!mode) data = ~data;
	temp=data<<bias;
	mask=0xff<<bias;
	OLED_GRAM[page][x]|=temp & mask;
	OLED_GRAM[page][x]&=temp | ~mask;
	if(bias) {
		if(++page < OLED_PAGE_NUM){
			temp = data >>(8-bias);
			mask = 0xff >>(8-bias);
			OLED_GRAM[page][x]|=temp & mask;
			OLED_GRAM[page][x]&=temp | ~mask;
		}
	}
}

/**
  * @brief  ���Դ����һ��ASCII�ַ�
	* @param  x���е�λ��
	* @param  y���е�λ��
	* @param  size���ַ��Ĵ�С
	*		@arg	12��0612����
	*		@arg	16��0816����
	* @param  mode���ַ�����ʽ
	*		@arg	0���ڵ�
	*		@arg	1������
  * @retval ��
  */
void OLED_DrawASCIIChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{      			    
	uint8_t temp,i,j;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		
	 
	if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
	if(size==12) {//����0612���� 
		for(i=0;i<6;i++) {  
			OLED_DrawByte(x+i, y, oled_ascii_0612[chr*12+i], mode);
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
			OLED_DrawByte(x+i, y, oled_ascii_0816[chr*16+i], mode);
			OLED_DrawByte(x+i, y+8, oled_ascii_0816[chr*16+i+8], mode);
		}
	}
}

/**
  * @brief  ���Դ����һ�������ַ���
	* @param  x���е�λ��
	* @param  y���е�λ��
	* @param  mode���ַ�����ʽ
	*		@arg	0���ڵ�
	*		@arg	1������
  * @retval ��
  */
void OLED_DrawCNZHString(uint8_t x, uint8_t y, char *str, uint8_t mode)
{
	uint8_t i,j;
	
	while(*str != '\0') {
		if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
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

/**
  * @brief  ���Դ�����ַ���
	* @param  x���е�λ��
	* @param  y���е�λ��
	* @param  size���ַ��Ĵ�С
	*		@arg	12��0612����
	*		@arg	16��0816����
	* @param  mode���ַ�����ʽ
	*		@arg	0���ڵ�
	*		@arg	1������
  * @retval ��
  */
void OLED_DrawString(uint8_t x, uint8_t y,char *str, uint8_t size, uint8_t mode)
{
	char chr[3];
	if(size == 12) {
    while(*str!='\0')
    {       
			if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
			OLED_DrawASCIIChar(x,y,*str,12,mode);	 
			x+=6;
			str++;
    } 
	}else {
		while(*str != '\0') {
			if(x>=OLED_SIZE_X||y>=OLED_SIZE_Y)return;//������Χ��.
			if(*str > 127) {
				chr[0] = str[0];
				chr[1] = str[1];
				chr[2] = '\0';          //����Ϊ�����ֽ�
				OLED_DrawCNZHString(x, y, chr, mode);  //��ʾ����
				str += 2;
				x += 16;
			}else {
				chr[0] = str[0];
				chr[1] = '\0';          //��ĸռһ���ֽ�
				OLED_DrawASCIIChar(x, y, chr[0], 16 ,mode);  //��ʾ��ĸ
				str += 1;
				x += 8;
			}
		}
	}
}

/**
  * @brief  ���Ĳ������������Դ���ơ�ɽ���Ƽ���ѧ��������
	* @param  ��
  * @retval ��
  */
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

/**
  * @brief  ֡�ʡ�CPUռ���ʲ������������Դ���������ģ��İ�����
	* @param  ��
  * @retval ��
  */
void OLED_Simulated_Noise_Example(void) { //<10Hz
	int x,y,m;
	char chr[17];
#ifdef	OLED_COMPUT_TRANS_FPS
	uint32_t times_n = OLED_SYSTIME_VARIABLE *OLED_SYSTIME_PERIOD /100000;
	static uint32_t times_l=0;
	if(times_n != times_l) {
		times_l = times_n;
		sprintf(chr, "rate:%8.2ffps", fps);
		OLED_DrawString(0, 0, chr, 16, 1);
	}
#endif	/* OLED_COMPUT_TRANS_FPS */
	
	//���Դ���������ƺڵ��׵�
	x = rand() %OLED_SIZE_X;
	y = rand() %(OLED_SIZE_Y-16) +16;
	m = rand() %2;
	OLED_DrawPoint(x,y,m);
}

/**
  * @brief  ֡�ʲ������������Դ������ͬ֡�ʵ��ƶ�������
	* @param  ��
  * @retval ��
  */
void OLED_Frame_Example(void) {
#if (OLED_SIZE_X >= 128)
	#define OLED_POS_MAX 108
#elif (OLED_SIZE_X >= 96)
	#define OLED_POS_MAX 72
#elif (OLED_SIZE_X >= 64)
	#define OLED_POS_MAX 48
#elif (OLED_SIZE_X >= 48)
	#define OLED_POS_MAX 24
#endif
	static uint8_t pos=8;
	static uint32_t times_l=0, times_d=0;
	uint32_t times_n = OLED_SYSTIME_VARIABLE;
#ifdef OLED_SYSTIME_DOWN_CONT
	times_d += times_l -times_n +((int32_t)(times_l -times_n) < 0 ? OLED_SYSTIME_SIZE : 0);
#else 
	times_d += times_n -times_l +((int32_t)(times_n -times_l) < 0 ? OLED_SYSTIME_SIZE : 0);
#endif /* OLED_SYSTIME_DOWN_CONT */
	times_l = times_n;
	if(times_d >= 10000 /OLED_SYSTIME_PERIOD) {
		times_d = 0;
		if(++pos > OLED_POS_MAX) {
			pos = 0;
		}
		if(pos % 2 == 0){
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
/**
  * @brief  ���ֲ�������ѭ��չʾ
	* @param  u8g2��u8g2��ʹ�õĽṹ��
  * @retval ��
  */
void OLED_Example_Loop(u8g2_t *u8g2) {
	static uint8_t stat=0;
	static uint32_t times_l=0, times_d=0;
	uint32_t times_n = OLED_SYSTIME_VARIABLE;
#ifdef OLED_SYSTIME_DOWN_CONT
	times_d += times_l -times_n +((int32_t)(times_l -times_n) < 0 ? OLED_SYSTIME_SIZE : 0);
#else 
	times_d += times_n -times_l +((int32_t)(times_n -times_l) < 0 ? OLED_SYSTIME_SIZE : 0);
#endif /* OLED_SYSTIME_DOWN_CONT */
	times_l = times_n;
	if(times_d >= (10000000 /OLED_SYSTIME_PERIOD)) {
		times_d = 0;
		if(++stat > 3) stat = 0;
		OLED_Clear_Gram();
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
/**
  * @brief  ���ֲ�������ѭ��չʾ
	* @param  ��
  * @retval ��
  */
void OLED_Example_Loop(void) {
	static uint8_t stat=0;
	static uint32_t times_l=0, times_d=0;
	uint32_t times_n = OLED_SYSTIME_VARIABLE *OLED_SYSTIME_PERIOD;
#ifdef OLED_SYSTIME_DOWN_CONT
	times_d += times_l -times_n;
#else 
	times_d += times_n -times_l;
#endif /* OLED_SYSTIME_DOWN_CONT */
	times_l = times_n;
	if(times_d >= 10000000) {
		times_d %= 10000000;
		if(++stat > 2) stat = 0;
		OLED_Clear_Gram();
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
#endif  /* USE_GRAM_DRAW_BASE_FUNCTION */

/**
  * @brief  �Դ���պ���
	* @param  ��
  * @retval ��
  */
void OLED_Clear_Gram(void)  
{
	memset(OLED_GRAM, 0x00, OLED_PAGE_NUM*OLED_SIZE_X);
}

/**
  * @brief  ��SSD1603ˢд�Դ�
	* @param  sync��ˢд��ʽ
	*		@arg	0���첽��ʽ��CPU���ȴ�ˢд����
	*		@arg	1��ͬ����ʽ��CPU�ȴ�ˢд����
  * @retval ��Ϊ-1��ˢдʧ�ܣ�Ϊ0ˢд�ɹ���ʼ
  */
int OLED_Refresh_Gram(int sync)
{
	int32_t timeout;
#ifdef	OLED_COMPUT_TRANS_FPS
	static uint32_t times_l;
	uint32_t times_n = OLED_SYSTIME_VARIABLE;
	
#ifndef	OLED_SYSTIME_DOWN_CONT
	int32_t	times_bias = times_n - times_l;
#else		/* OLED_SYSTIME_DOWN_CONT */
	int32_t	times_bias = times_l - times_n;
#endif	/* OLED_SYSTIME_DOWN_CONT */
	if(times_bias < 0) times_bias += OLED_SYSTIME_SIZE;
	fps = 1000000.0 /OLED_SYSTIME_PERIOD /times_bias;
	times_l = times_n;
#endif	/* OLED_COMPUT_TRANS_FPS */
	
	while(OLED_send_cmd(0xb0));				//����ҳ��ַ��0~7��
	while(OLED_send_cmd(0x00));				//������ʾλ�á��е͵�ַ
	while(OLED_send_cmd(0x10));				//������ʾλ�á��иߵ�ַ 
	
	OLED_DMAConfig();
	
	timeout = WaitTime;
	while(I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_GenerateSTART(OLED_IICx, ENABLE);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_Send7bitAddress(OLED_IICx, 0x78, I2C_Direction_Transmitter);
	timeout = WaitTime;
	while(!I2C_CheckEvent(OLED_IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if(--timeout == 0) return -1;
	}
	
	I2C_SendData(OLED_IICx, 0x40);
	timeout = WaitTime;
	while ((!I2C_GetFlagStatus(OLED_IICx,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(OLED_IICx,I2C_FLAG_BTF))) {
		if(--timeout == 0) return -1;
	}
	dma1_busy = 1;
	I2C_DMACmd(OLED_IICx, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	if(sync){
		timeout = WaitTime;
		while(dma1_busy) if(--timeout == 0) return -1;
	}
	
	return 0;
}

/**
  * @brief  ��ʼ��SSD1603����ˢд�Դ�
	* @param  ��
  * @retval ��Ϊ-1��ʧ�ܣ�Ϊ0�ɹ�
  */
int OLED_Continuous_Refresh_Start(void) {
	continuous_refresh = 1;
	return OLED_Refresh_Gram(0);
}

/**
  * @brief  ֹͣ��SSD1603����ˢд�Դ�
	* @param  ��
  * @retval ��
  */
void OLED_Continuous_Refresh_Stop(void) {
	continuous_refresh = 0;
}

/**
  * @brief  DMA1ͨ��6�жϷ�����
	* @param  ��
  * @retval ��
  */
#ifdef OLED_DMA1_Channel6
void DMA1_Channel6_IRQHandler() {
	int timeout = WaitTime;
	if(DMA_GetITStatus(DMA1_IT_TC6)) {
		while (--timeout && !I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BTF));
		
		I2C_GenerateSTOP(OLED_IICx, ENABLE);
		timeout = WaitTime;
		while(--timeout && I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY));
		
		I2C_DMACmd(OLED_IICx, DISABLE);
		DMA_Cmd(DMA1_Channel6, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC6);
		(void) OLED_IICx->SR1;
		(void) OLED_IICx->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_busy = 0;
	}else if(DMA_GetITStatus(DMA_IT_TE)) {
		DMA_DeInit(DMA1_Channel6);
		(void) OLED_IICx->SR1;
		(void) OLED_IICx->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_busy = 0;
	}
}
#endif
/**
  * @brief  DMA1ͨ��4�жϷ�����
	* @param  ��
  * @retval ��
  */
#ifdef OLED_DMA1_Channel4
void DMA1_Channel4_IRQHandler() {
	int timeout = WaitTime;
	if(DMA_GetITStatus(DMA1_IT_TC4)) {
		while (--timeout && !I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BTF));
		
		I2C_GenerateSTOP(OLED_IICx, ENABLE);
		timeout = WaitTime;
		while(--timeout && I2C_GetFlagStatus(OLED_IICx, I2C_FLAG_BUSY));
		
		I2C_DMACmd(OLED_IICx, DISABLE);
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC6);
		(void) OLED_IICx->SR1;
		(void) OLED_IICx->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_busy = 0;
	}else if(DMA_GetITStatus(DMA_IT_TE)) {
		DMA_DeInit(DMA1_Channel4);
		(void) OLED_IICx->SR1;
		(void) OLED_IICx->SR2;
		
		if(continuous_refresh)
			while(OLED_Refresh_Gram(0));
		else
			dma1_busy = 0;
	}
}
#endif

#ifdef	SUPPORT_U8G2
/**
  * @brief  u8g2�ջص�����
	* @param  u8x8��u8g2��ʹ�õĽṹ��
	* @param  msg������
	* @param  arg_int��uint8_t���͵��������
	* @param  arg_ptr��ָ�����͵��������
  * @retval ��
  */
uint8_t OLED_u8x8_empty_cb(u8x8_t *u8x8, uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr) {
	return NULL;
}

/**
  * @brief  ֧��u8g2�ĳ�ʼ������
	* @param  ��
  * @retval ����u8g2����Ҫʹ�õĽṹ��
  */
u8g2_t *OLED_U8G2_Init(void) {
	static u8g2_t u8g2;
	uint8_t *buffer = (uint8_t *)OLED_GRAM;
	
	u8g2_SetupDisplay(&u8g2, u8x8_d_ssd1306_128x64_noname, OLED_u8x8_empty_cb, OLED_u8x8_empty_cb, OLED_u8x8_empty_cb);
  u8g2_SetupBuffer(&u8g2, buffer, 8, u8g2_ll_hvline_vertical_top_lsb, U8G2_R0);
	return &u8g2;
}

#ifdef	USE_U8G2_EXAMPLE
/**
  * @brief  u8g2�ṩ�Ĳ�������
	* @param  u8g2��u8g2��ʹ�õĽṹ��
  * @retval ��
  */
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
#endif	/* USE_U8G2_EXAMPLE */
#endif	/* SUPPORT_U8G2 */
