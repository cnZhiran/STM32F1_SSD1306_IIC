/**
  ******************************************************************************
  * @file    ctrl.h
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file contains all the functions prototypes for the system control.
  ******************************************************************************
	* @attention
	*
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
	* ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CTRL_H
#define __CTRL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"                  // Device header

/** @defgroup FSM_stat_type_definition
  * @{
  */
	 
typedef enum {
	system_init,
	system_ready
} SYSTEM_STATE_Type;

/**
  * @}
  */

/** @defgroup System_Control_Exported_Variables
  * @{
  */

extern uint32_t system_stat;
extern uint32_t times;

/**
  * @}
  */

/** @defgroup System_Control_Exported_Functions
  * @{
  */

void delay_us(int nus); //����10us
void FSM_main(void);

/**
  * @}
  */
	
#ifdef __cplusplus
}
#endif

#endif	/* CTRL_H */
