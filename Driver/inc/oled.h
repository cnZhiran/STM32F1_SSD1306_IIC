/**
  ******************************************************************************
  * @file    oled.h
  * @author  Zhiran
  * @date    2021-8-3
  * @brief   This file contains all the functions prototypes for the IIC-OLED.
  ******************************************************************************
	* @attention
	*
  * 该代码开源免费，请自觉遵守开源协议，如果您是通过购买获得此代码，一定是被噶韭菜了！
	* 同时，由于使用该代码所造成的任何直接或间接损失，作者都概不负责。
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup OLED_Configuration_Option
  * @{
  */
	 
#define OLED_IICx					I2C1
#define OLED_IIC_GPIOx		GPIOB
#define OLED_IIC_PIN			GPIO_Pin_6|GPIO_Pin_7
//#define OLED_IIC_REMAP	GPIO_Remap_I2C1
	 
#define OLED_SIZE_X 64
#define OLED_SIZE_Y 128
#define OLED_PAGE_NUM OLED_SIZE_X/8

#define USE_GRAM	//启用图形缓存，以及它的基本控制和绘制函数
#define USE_PAGE	//启用页绘制函数（非DMA方式）
#define SUPPORT_U8G2	//启用u8g2第三方图形库，若不启用，可将组u8g2删除，以减少程序大小
	 
#ifdef	SUPPORT_U8G2	
#define	USE_U8G2_EXAMPLE		//启用u8g2绘制测试用例
#ifndef USE_GRAM
#define USE_GRAM	//如果使用u8g2库，强制使用图形缓存
#endif	/* USE_GRAM */
#endif	/* SUPPORT_U8G2 */

#ifdef	USE_GRAM
#define USE_GRAM_DRAW_BASE_FUNCTION	//使用图形缓存基本绘制函数
#ifdef	USE_GRAM_DRAW_BASE_FUNCTION
#define OLED_COMPUT_TRANS_FPS					//计算传输帧率
#define USE_GRAM_EXAMPLE		//启用图形缓存的绘制测试用例
#endif  /* USE_GRAM_DRAW_BASE_FUNCTION */
#endif	/* USE_GRAM */

#if defined OLED_COMPUT_TRANS_FPS || defined USE_GRAM_EXAMPLE	//计算帧率和测试用例需要使用一个可靠的系统时间计数变量，不需要此功能可以注释掉
#define OLED_SYSTIME_VARIABLE	times		//系统时间计数变量名称
#define OLED_SYSTIME_PERIOD		10			//计数一次的周期，单位微秒us
#define OLED_SYSTIME_SIZE			500000				//计数变量的范围，UINT32_MAX+1等价于0
//以下是一个示例，使用SysTick->VAL寄存器作为系统时间
//#define OLED_SYSTIME_DOWN_CONT				//SysTick是个减计数器
//#define OLED_SYSTIME_USE_SYSTICK			//使用SysTick->VAL作为计数器，注意浮点运算用时较长
//#define OLED_SYSTIME_VARIABLE	SysTick->VAL
//#define OLED_SYSTIME_PERIOD		(1000000.0/((SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) ? SystemCoreClock : SystemCoreClock/8))			//计数一次的周期，单位微秒us
//#define OLED_SYSTIME_SIZE			SysTick->LOAD +1


#endif  /* OLED_COMPUT_TRANS_FPS || USE_GRAM_EXAMPLE */
	 
/**
  * @}
  */

#define IS_OLED_PAGE(PAGE) (PAGE >= 0 && PAGE < OLED_PAGE_NUM)
#define IS_OLED_COLUMN(COLUMN) (COLUMN >= 0 && COLUMN < OLED_SIZE_Y)
	

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"                  // Device header
#ifdef	SUPPORT_U8G2
	#include "u8g2.h"
#endif	/* SUPPORT_U8G2 */
/** @defgroup OLED_Exported_Variables
  * @{
  */	 
#ifdef	OLED_COMPUT_TRANS_FPS
extern float fps;
extern uint32_t OLED_SYSTIME_VARIABLE;
#endif	/* OLED_COMPUT_TRANS_FPS */
	 
/**
  * @}
  */
	
/** @defgroup OLED_Exported_Functions
  * @{
  */
//IIC接口控制函数
int OLED_send_cmd(uint8_t o_command);
int OLED_send_data(uint8_t o_data);

//oled控制函数		   				   		    
void OLED_Init(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);

//光标控制函数
void OLED_SetPage(uint8_t page);
void OLED_SetColumn(uint8_t col);
void OLED_SetPos(uint8_t page, uint8_t col);

//页绘制函数
void OLED_Clear(void);
#ifdef	USE_PAGE
void OLED_Full(uint8_t val);
void OLED_ShowASCIIString(uint8_t page, uint8_t col, char *str);
void OLED_ShowCHZHString(uint8_t page, uint8_t col, char *str);
void OLED_ShowString(uint8_t page, uint8_t col, char *str);
#endif	/* USE_PAGE */

#ifdef USE_GRAM
//显存操作函数
void OLED_Clear_Gram(void);
int OLED_Refresh_Gram(int sync);
int OLED_Continuous_Refresh_Start(void);
void OLED_Continuous_Refresh_Stop(void);

#ifdef	USE_GRAM_DRAW_BASE_FUNCTION
//图形缓存基本绘制函数
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode);
void OLED_DrawByte(uint8_t x, uint8_t y,uint8_t data, uint8_t mode);
//void OLED_DrawChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
#define OLED_DrawChar(x,y,chr,size,mode) OLED_DrawASCIIChar(x,y,chr,size,mode)
void OLED_DrawASCIIChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void OLED_DrawCNZHString(uint8_t x, uint8_t y, char *str, uint8_t mode);
void OLED_DrawString(uint8_t x, uint8_t y, char *str, uint8_t size, uint8_t mode);

#ifdef	USE_GRAM_EXAMPLE
//图形缓存基本绘制函数测试用例
void OLED_CNZH_Example(void);
void OLED_Simulated_Noise_Example(void);
void OLED_Frame_Example(void);
#ifdef	USE_U8G2_EXAMPLE
//混合测试用例
void OLED_Example_Loop(u8g2_t *u8g2);
#else		/* USE_U8G2_EXAMPLE */
//混合测试用例
void OLED_Example_Loop(void);
#endif	/* USE_U8G2_EXAMPLE */
#endif  /* USE_GRAM_EXAMPLE */
#endif  /* USE_GRAM_DRAW_BASE_FUNCTION */

#ifdef SUPPORT_U8G2
//u8g2第三方绘制库支持函数
u8g2_t *OLED_U8G2_Init(void);
#endif	/* SUPPORT_U8G2 */

#ifdef	USE_U8G2_EXAMPLE
//u8g2第三方绘制库支持函数绘制用例
void OLED_U8G2_Draw_Example(u8g2_t *u8g2);
#endif	/* USE_U8G2_EXAMPLE */


#endif	/* USE_GRAM */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif	/* OLED_H */
