/**
  ******************************************************************************
  * @file    main.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   Main program body
  ******************************************************************************
	* @attention
	*
  * 该代码开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，一定是被噶韭菜了！
	* 同时，由于使用该代码所造成的任何直接或间接损失，作者都概不负责。
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
  * @brief  所有时钟的初始化
  * @param  无            
  * @retval 无
  */
void Clock_Init(void) {
	//GPIO时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  //AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//串口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
//	//ADC时钟
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//I2C时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	//DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
}

/**
  * @brief  所有中断及其优先级的初始化
  * @param  无          
  * @retval 无
  */
void Interrpt_Init(void) {
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	//设置优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//SysTick中断
	NVIC_SetPriority(SysTick_IRQn, (1<<3) | 2);
	
	//串口中断
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
  * @brief  程序入口
  * @param  无                
  * @retval 无
  */
int main(void) {
	Clock_Init();
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭JTAG保留SW
	SysTick_Config(SystemCoreClock / 100000);									//SysTick 每10us中断一次
	
#ifdef OLED_SYSTIME_USE_SYSTICK		//使用SysTick->VAL作为计数器
	SysTick->LOAD  = 0xFFFFFF;																	/* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);	/* set Priority for Cortex-M0 System Interrupts */
  SysTick->VAL   = 0;																					/* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_ENABLE_Msk;										/* Enable SysTick IRQ and SysTick Timer */
#endif /* OLED_SYSTIME_USE_SYSTICK */
	
	UART1_Init(115200);
	UART3_Init(115200);
	OLED_Init();
	Interrpt_Init();
	
	//开始使用DMA方式刷新屏幕
	while(OLED_Continuous_Refresh_Start());
	
	//u8g2_t *u8g2 = OLED_U8G2_Init();

	//初始化结束
	printf("init_already\n");
	system_stat = system_ready;
	
	while(1){
		//每10秒向串口输出帧率
#ifdef OLED_COMPUT_TRANS_FPS
		if(times % 1000000 == 0)
			printf("FPS:%.2f\n", fps);
#endif /* OLED_COMPUT_TRANS_FPS */

		//OLED_Example_Loop(u8g2);
		OLED_Example_Loop();
	}
}

