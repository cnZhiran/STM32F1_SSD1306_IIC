/**
  ******************************************************************************
  * @file    oled.h
  * @author  Zhiran
  * @date    2021-8-3
  * @brief   This file contains all the functions prototypes for the IIC-OLED.
  ******************************************************************************
	* @attention
	*
  * 该程序开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，请自行申诉。
	* 同时，由于使用该程序所造成的任何直接或间接损失，作者都概不负责。
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"                  // Device header
#include "ctrl.h"
	 
/** @addtogroup OLED_Configuration_Option
  * @{
  */
	 
#define USE_GRAM
#define USE_PAGE
#ifdef	USE_GRAM
	 #define COMPUT_FPS
#endif	/* USE_GRAM */

#define OLED_SIZE_X 8
#define OLED_SIZE_Y 128

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
	 
/**
  * @}
  */

/** @defgroup OLED_Exported_Variables
  * @{
  */	 
#ifdef	COMPUT_FPS
extern float fps;
#endif	/* COMPUT_FPS */
	 
/**
  * @}
  */
	
/** @defgroup OLED_Exported_Functions
  * @{
  */

//oled控制函数		   				   		    
void OLED_Init(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);

//光标控制函数
void OLED_SetPage(u8 page);
void OLED_SetColumn(u8 col);
void OLED_SetPos(u8 page, u8 col);

//页绘制函数 
void OLED_Clear(void);
void OLED_Full(void);
void OLED_ShowASCIIString(u8 page, u8 col, char *str);
void OLED_ShowCHZHString(u8 page, u8 col, char *str);
void OLED_ShowString(u8 page, u8 col, char *str);

#ifdef USE_GRAM
//显存操作函数
void OLED_Clear_Gram(void);
void OLED_Reflush_Gram(void);
int OLED_StratDMAFlush(void);
//显存绘制函数
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawByte(u8 x, u8 y,u8 data, u8 mode);
void OLED_DrawChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_DrawASCIIChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_DrawCNZHString(u8 x, u8 y, char *str, u8 size, u8 mode);
void OLED_DrawString(u8 x, u8 y, char *str, u8 size, u8 mode);
#endif	/* USE_GRAM */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif	/* OLED_H */
