/**
  ******************************************************************************
  * @file    ctrl.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file provides the system control functions.
  ******************************************************************************
	* @attention
	*
  * 该程序开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，请自行申诉。
	* 同时，由于使用该程序所造成的任何直接或间接损失，作者都概不负责。
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "ctrl.h"
#include "driver.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void delay_us(int nus);
void FSM_main(void);

/* Private functions ---------------------------------------------------------*/
uint32_t times = 0;
char str[17];
uint32_t system_stat = system_init;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  微秒级延时函数
  * @param  nus: 延时时长，单位us
	* @note		延时精度为SysTick中断周期，即10us
  * @retval 无
  */
void delay_us(int nus) { //精度10us
	int times_l = times;
	nus /= 10;
	while(times - times_l < nus);
}

/**
  * @brief  基于有限状态机(FSM)的系统控制函数
  * @param  无
  * @retval 无
  */
void FSM_main(void) {
	
}

/**
  * @brief  SysTick中断服务函数
  * @param  无
  * @retval 无
  */
void SysTick_Handler(void) {
	times++;
}
