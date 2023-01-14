/**
  ******************************************************************************
  * @file    uart.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file provides the uart driver functions.
  ******************************************************************************
	* @attention
	*
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
	* ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t flag_full=0;
uint8_t rx1Data[RXDATA_SIZE];
uint8_t rx3Data[RXDATA_SIZE];

/* Private function prototypes -----------------------------------------------*/
void UART1_Init(u32 bound);
void UART3_Init(u32 bound);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ��ʼ������1������GPIO��USART�ĳ�ʼ��
  * @param  bound: ���ڵĲ�����
  * @retval None
  */
void UART1_Init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//����ʹ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;					//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//�ر�Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//������պͷ���

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//��������ж�
  USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  ��ʼ������3������GPIO��USART�ĳ�ʼ��
  * @param  bound: ���ڵĲ�����
  * @retval None
  */
void UART3_Init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//����ʹ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;					//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//�ر�Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//������պͷ���

  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//��������ж�
  USART_Cmd(USART3, ENABLE);
}

/**
  * @brief  ����1���жϷ�����
  * @param  ��
  * @retval ��
  */
void USART1_IRQHandler(void)                
	{
  static unsigned int i=0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{ 
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		rx1Data[i++]=USART_ReceiveData(USART1);	
		if(i>=RXDATA_SIZE) i=0;	
	} 
} 

/**
  * @brief  ����3���жϷ�����
  * @param  ��
  * @retval ��
  */
void USART3_IRQHandler(void)                
	{
  static unsigned int i=0;
		
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{ 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		
		rx3Data[i++]=USART_ReceiveData(USART3);	
		if(i>=RXDATA_SIZE) i=0;	
	} 
}

#pragma import(__use_no_semihosting) 
struct __FILE { 
    int handle;                 // Add whatever you need here  
}; 
FILE __stdout; 
FILE __stdin; 

int fputc(int ch, FILE *f) 
{ 
	while((USART1->SR & USART_SR_TC)==0);
	USART1->DR = (uint8_t) ch;      
	return ch; 
} 

int fgetc(FILE *f) 
{ 
  static unsigned int i=0;
	uint8_t ch = rx1Data[i++];
	if(i>=RXDATA_SIZE) i=0;
  return ch; 
} 

void _ttywrch(int ch) 
{ 
  while((USART1->SR & USART_SR_TC)==0);
	USART1->DR = (uint8_t) ch;  
} 

int ferror(FILE *f) 
{                           
	// Your implementation of ferror 
	return EOF; 
} 

void _sys_exit(int return_code) 
{ 
	label:  goto label;           // endless loop 
} 
