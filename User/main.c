/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   Main program body
  ******************************************************************************
	* @attention
	*
  * �ó���Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬���������ߡ�
	* ͬʱ������ʹ�øó�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"                  // Device header
#include "driver.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Clock_Init(void);
void Interrpt_Init(void);
	
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ����ʱ�ӵĳ�ʼ��
  * @param  ��            
  * @retval ��
  */
void Clock_Init(void) {
	//GPIOʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  //AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	//��ʱ��ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
//	//ADCʱ��
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//I2Cʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	//DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
}

/**
  * @brief  �����жϼ������ȼ��ĳ�ʼ��
  * @param  ��          
  * @retval ��
  */
void Interrpt_Init(void) {
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	//�������ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//SysTick�ж�
	NVIC_SetPriority(SysTick_IRQn, (1<<3) | 2);
	
	//�����ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
  * @brief  �������
  * @param  ��                
  * @retval ��
  */
void OLED_Example_Loop(u8g2_t *u8g2);
int main(void) {
	Clock_Init();
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����SW
	SysTick_Config(72000000 / 100000);												//SysTick ÿ10us�ж�һ��
	UART1_Init(115200);
	UART3_Init(115200);
	OLED_Init();
	Interrpt_Init();
	
	//��ʼʹ��DMA��ʽˢ����Ļ
	while(OLED_Continuous_Refresh_Start());
	
	u8g2_t *u8g2 = OLED_U8G2_Init();

	//��ʼ������
	printf("init_already\n");
	system_stat = system_ready;
	
	while(1){
		//ÿ10���򴮿����֡��
		if(times % 1000000 == 0)
			printf("FPS:%.2f\n", fps);
		OLED_Example_Loop(u8g2);
	}
}

