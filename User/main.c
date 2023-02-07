/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   Main program body
  ******************************************************************************
	* @attention
	*
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
	* ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
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
int main(void) {
	Clock_Init();
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����SW
	SysTick_Config(SystemCoreClock / 100000);									//SysTick ÿ10us�ж�һ��
	
#ifdef OLED_SYSTIME_USE_SYSTICK		//ʹ��SysTick->VAL��Ϊ������
	SysTick->LOAD  = 0xFFFFFF;																	/* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);	/* set Priority for Cortex-M0 System Interrupts */
  SysTick->VAL   = 0;																					/* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_ENABLE_Msk;										/* Enable SysTick IRQ and SysTick Timer */
#endif /* OLED_SYSTIME_USE_SYSTICK */
	
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
#ifdef OLED_COMPUT_TRANS_FPS
		//ÿ10���򴮿����֡��
		uint32_t times_n = OLED_SYSTIME_VARIABLE *OLED_SYSTIME_PERIOD /10000000;
		static uint32_t times_l=0;
		if(times_n != times_l) {
			times_l = times_n;
			printf("frame rate:%.2f\n", fps);
		}
#endif /* OLED_COMPUT_TRANS_FPS */

		OLED_Example_Loop(u8g2);
		//OLED_Example_Loop();
	}
}

