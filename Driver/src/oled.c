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

u8 OLED_GRAM[OLED_SIZE_X][OLED_SIZE_Y];
uint32_t times_l;
float fps;

void OLED_DMAConfig(void);

#define WaitTime 0x1000
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

//初始化OLED					    
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
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
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
	DMA_InitStructure.DMA_BufferSize = OLED_SIZE_X * OLED_SIZE_Y;
	
	//DMA_DeInit(DMA1_Channel6);
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

//开启OLED显示    
void OLED_Display_On(void)
{
	while(OLED_send_cmd(0X8D));  //SET DCDC命令
	while(OLED_send_cmd(0X14));  //DCDC ON
	while(OLED_send_cmd(0XAF));  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	while(OLED_send_cmd(0X8D));  //SET DCDC命令
	while(OLED_send_cmd(0X10));  //DCDC OFF
	while(OLED_send_cmd(0XAE));  //DISPLAY OFF
}

//设置页地址（光标行）
void OLED_SetPage(u8 page)
{
	while(OLED_send_cmd(0xb0+page));
}

//设置列地址（光标列）
void OLED_SetColumn(u8 col)
{
	while(OLED_send_cmd(0x10|(col>>4)));		//设置列地址高位
	while(OLED_send_cmd(0x00|(col&0x0f)));	//设置列地址低位
}

//设置光标
void OLED_SetPos(u8 page, u8 col)
{
	while(OLED_send_cmd(0xb0+page));
	while(OLED_send_cmd(0x10|(col>>4)));
	while(OLED_send_cmd(0x00|(col&0x0f)));
}

//清屏函数
void OLED_Clear(void)
{
	unsigned char page,col;
	for(page=0; page<OLED_SIZE_X; page++) {
		OLED_SetPos(page, 0);
		for(col=0; col<OLED_SIZE_Y; col++) {
			while(OLED_send_data(0x00));
		}
	}
}
//填充函数
void OLED_Full(void)
{
	unsigned char page,column;
	for(page=0; page<OLED_SIZE_X; page++) {
		OLED_SetPos(page, 0);
		for(column=0; column<OLED_SIZE_Y; column++) {
			while(OLED_send_data(0xff));
		}
	}
}
//显示ASC字符
//page:0~7
//col:0~127
//str:字符串首地址
void OLED_ShowASCIIString(u8 page, u8 col, char *str)
{
	u8 c=0,i=0,j=0;

	while (str[j]!='\0')
	{
		c = str[j]-32;
		if(col>120){col=0;page++;}
		if(page>7){col=page=0;OLED_Clear();}
		OLED_SetPos(page,col);
		for(i=0;i<8;i++)
			OLED_send_data(oled_asc2_0816[c*16+i]);
		OLED_SetPos(page+1,col);
		for(i=8;i<16;i++)
			OLED_send_data(oled_asc2_0816[c*16+i]);
		col+=8;
		j++;
	}
}
void OLED_ShowCHZHString(u8 page, u8 col, char *str)
{
	u8 i,j;
	
	while(*str != '\0') {
		for(i=0; i<CNZHNUM; i++) {
			if(str[0] == oled_cnzh_1616[i].index[0] && str[1] == oled_cnzh_1616[i].index[1]) {
				break;
			}
		}
		if(i != CNZHNUM) {	//显示汉字
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
		}else {	//显示空格
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

void OLED_ShowString(u8 page, u8 col, char *str)
{
	char chr[3];

	while(*str != '\0') {
		if(*str > 127) {
			if(col>112){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = str[1];
			chr[2] = '\0';          //汉字为两个字节
			OLED_ShowCHZHString(page, col, chr);  //显示汉字
			str += 2;
			col += 16;
		}else {
			if(col>120){col=0;page+=2;}
			if(page>6){col=page=0;OLED_Clear();}
			chr[0] = str[0];
			chr[1] = '\0';          //字母占一个字节
			OLED_ShowASCIIString(page, col, chr);  //显示字母
			str += 1;
			col += 8;
		}
	}
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 page,temp=0;
	if(x>127||y>63)return;//超出范围了.
	page = y/8;
	temp=1<<(y % 8);
	if(t)OLED_GRAM[page][x]|=temp;
	else OLED_GRAM[page][x]&=~temp;	    
}

//在指定位置绘制一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_DrawChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,i,j;
	chr=chr-' ';//得到偏移后的值		
	 
	if(size==12) {//调用0612字体 
		for(i=0;i<6;i++) {  
			temp=oled_asc2_0612[chr*12+i];  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<6;i++) {  
			temp=oled_asc2_0612[chr*12+i+6];                         
			for(j=8;j<12;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
	}
	else {//调用0816字体
		for(i=0;i<8;i++) {
			temp=oled_asc2_0816[chr*16+i];		  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<8;i++) {
			temp=oled_asc2_0816[chr*16+i+8];                         
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
//在指定位置绘制一个ASCII字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_DrawASCIIChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{      			    
	u8 temp,i,j;
	chr=chr-' ';//得到偏移后的值		
	 
	if(size==12) {//调用0612字体 
		for(i=0;i<6;i++) {  
			temp=oled_asc2_0612[chr*12+i];  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<6;i++) {  
			temp=oled_asc2_0612[chr*12+i+6];                         
			for(j=8;j<12;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
	}
	else {//调用0816字体
		for(i=0;i<8;i++) {
			temp=oled_asc2_0816[chr*16+i];		  	                          
			for(j=0;j<8;j++) {
				if(temp & 0x01)
					OLED_DrawPoint(x+i,y+j,mode);
				else 
					OLED_DrawPoint(x+i,y+j,!mode);
				temp>>=1;
			}
		}
		for(i=0;i<8;i++) {
			temp=oled_asc2_0816[chr*16+i+8];                         
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
void OLED_DrawByte(u8 x, u8 y, u8 data, u8 mode) {
	u8 page,bias,temp,mask;
	if(x>127||y>56)return;//超出范围了.
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
//在指定位置绘制一个汉字字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_DrawCNZHString(u8 x, u8 y, char *str, u8 size, u8 mode)
{
	u8 i,j;
	
	while(*str != '\0') {
		if(x > 127) return;
		for(i=0; i<CNZHNUM; i++) {
			if(str[0] == oled_cnzh_1616[i].index[0] && str[1] == oled_cnzh_1616[i].index[1]) {
				break;
			}
		}
		if(i != CNZHNUM) {	//显示汉字
			for(j = 0;j < 16;j++) {
				OLED_DrawByte(x+j, y, oled_cnzh_1616[i].font[j], mode);
			}
			for(j = 0;j < 16;j++) {
				OLED_DrawByte(x+j, y+8, oled_cnzh_1616[i].font[j+16], mode);
			}
			str += 2;
			x += 16;
		}else {	//显示空格
			for(j = 0;j < 8;j++) {
				OLED_DrawByte(x+j, y, 0x00, mode);
				OLED_DrawByte(x+j, y+8, 0x00, mode);
			}
			str += 1;
			x += 8;
		}
	}
}

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//size:字体大小
//mode:0,反白显示;1,正常显示
void OLED_DrawString(u8 x, u8 y,char *str, u8 size, u8 mode)
{
	char chr[3];
	if(size == 12) {
    while(*str!='\0')
    {       
			if(x>120){x=0;y+=16;}
			if(y>58){y=x=0;}
			OLED_DrawChar(x,y,*str,12,mode);	 
			x+=8;
			str++;
    } 
	}else {
		while(*str != '\0') {
			if(*str > 127) {
				if(x>112){x=0;y+=16;}
				if(y>58){x=y=0;}
				chr[0] = str[0];
				chr[1] = str[1];
				chr[2] = '\0';          //汉字为两个字节
				OLED_DrawCNZHString(x, y, chr, 16, 1);  //显示汉字
				str += 2;
				x += 16;
			}else {
				if(x>120){x=0;y+=16;}
				if(y>58){x=y=0;}
				chr[0] = str[0];
				chr[1] = '\0';          //字母占一个字节
				OLED_DrawASCIIChar(x, y, chr[0], 16 ,1);  //显示字母
				str += 1;
				x += 8;
			}
		}
	}
}

//清空函数	  
void OLED_Clear_Gram(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0x00;
}
//刷新函数
void OLED_Reflush_Gram(void)
{
	int timeout;
//	for(page=0;page<8;page++)  
//	{  
		while(OLED_send_cmd(0xb0));	//设置页地址（0~7）
		while(OLED_send_cmd(0x00));				//设置显示位置—列低地址
		while(OLED_send_cmd(0x10));				//设置显示位置—列高地址 
	
		OLED_DMAConfig();
	
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		I2C_SendData(I2C1, 0x40);
		while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)));
		
		I2C_DMACmd(I2C1, ENABLE);
		DMA_Cmd(DMA1_Channel6, ENABLE);
		
		timeout = 100000;
		while (timeout-- && !DMA_GetFlagStatus(DMA1_FLAG_TC6));
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
		
//		for(col=0; col<128; col++) {
//			I2C_SendData(I2C1, OLED_GRAM[page][col]);
//			while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)));
//		}
		I2C_GenerateSTOP(I2C1, ENABLE);
		
		I2C_DMACmd(I2C1, DISABLE);
		DMA_Cmd(DMA1_Channel6, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC6);
		(void) I2C1->SR1;
		(void) I2C1->SR2;
//	}   
}

int OLED_StratDMAFlush(void) {
	int timeout = WaitTime;
	fps = 100000.0/(times - times_l);
	times_l = times;
	while(OLED_send_cmd(0xb0));				//设置页地址（0~7）
	while(OLED_send_cmd(0x00));				//设置显示位置—列低地址
	while(OLED_send_cmd(0x10));				//设置显示位置—列高地址 
	
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
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC|DMA_IT_TE, ENABLE);
	
	I2C_DMACmd(I2C1, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	return 0;
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
		
		while(OLED_StratDMAFlush());
	}else if(DMA_GetITStatus(DMA_IT_TE)) {
		DMA_DeInit(DMA1_Channel6);
		(void) I2C1->SR1;
		(void) I2C1->SR2;
		while(OLED_StratDMAFlush());
	}
}
