/**
  ******************************************************************************
  * @file    uart.h
  * @author  Zhiran
  * @date    2021-7-31
  * @brief   This file contains all the functions prototypes for the uart.
  ******************************************************************************
	* @attention
	*
  * 该程序开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，请自行申诉。
	* 同时，由于使用该程序所造成的任何直接或间接损失，作者都概不负责。
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
	 
/** @defgroup UART_Buffer_Size 
  * @{
  */	
	 
#define RXDATA_SIZE 500

/**
  * @}
  */

/** @defgroup UART_Exported_Variables
  * @{
  */
	 
extern uint8_t flag_full;
extern uint8_t rx1Data[RXDATA_SIZE];
extern uint8_t rx3Data[RXDATA_SIZE];

/**
  * @}
  */
	
/** @defgroup UART_Exported_Functions
  * @{
  */
	
void UART1_Init(u32 bound);
void UART3_Init(u32 bound);

/**
  * @}
  */
		
#ifdef __cplusplus
}
#endif

#endif	/* UART_H */
