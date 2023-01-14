/**
  ******************************************************************************
  * @file    ctrl.h
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file contains all the functions prototypes for the system control.
  ******************************************************************************
	* @attention
	*
  * 该代码开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，一定是被噶韭菜了！
	* 同时，由于使用该代码所造成的任何直接或间接损失，作者都概不负责。
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

void delay_us(int nus); //精度10us
void FSM_main(void);

/**
  * @}
  */
	
#ifdef __cplusplus
}
#endif

#endif	/* CTRL_H */
