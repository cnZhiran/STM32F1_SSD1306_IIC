/**
  ******************************************************************************
  * @file    uart.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file provides the uart driver functions.
  ******************************************************************************
	* @attention
	*
  * 该程序开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，请自行申诉。
	* 同时，由于使用该程序所造成的任何直接或间接损失，作者都概不负责。
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
  * @brief  初始化串口1，包括GPIO、USART的初始化
  * @param  bound: 串口的波特率
  * @retval None
  */
void UART1_Init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//引脚使能
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;					//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//关闭硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//允许接收和发送

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//允许接收中断
  USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  初始化串口3，包括GPIO、USART的初始化
  * @param  bound: 串口的波特率
  * @retval None
  */
void UART3_Init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//引脚使能
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;					//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//关闭硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//允许接收和发送

  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//允许接收中断
  USART_Cmd(USART3, ENABLE);
}

/**
  * @brief  串口1的中断服务函数
  * @param  无
  * @retval 无
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
  * @brief  串口3的中断服务函数
  * @param  无
  * @retval 无
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
	u8 ch = rx1Data[i++];
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
