/**
  ******************************************************************************
  * @file    ctrl.c
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file provides the system control functions.
  ******************************************************************************
	* @attention
	*
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
	* ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
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
  * @brief  ΢�뼶��ʱ����
  * @param  nus: ��ʱʱ������λus
	* @note		��ʱ����ΪSysTick�ж����ڣ���10us
  * @retval ��
  */
void delay_us(int nus) { //����10us
	int times_l = times;
	nus /= 10;
	while(times - times_l < nus);
}

/**
  * @brief  ��������״̬��(FSM)��ϵͳ���ƺ���
  * @param  ��
  * @retval ��
  */
void FSM_main(void) {
	
}

/**
  * @brief  SysTick�жϷ�����
  * @param  ��
  * @retval ��
  */
void SysTick_Handler(void) {
	times++;
}
