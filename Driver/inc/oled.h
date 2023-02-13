/**
  ******************************************************************************
  * @file    oled.h
  * @author  Zhiran
  * @date    2021-8-3
  * @brief   This file contains all the functions prototypes for the IIC-OLED.
  ******************************************************************************
  * @attention
  *
  * �ô��뿪Դ��ѣ����Ծ����ؿ�ԴЭ�飬�������ͨ�������ô˴��룬һ���Ǳ����²��ˣ�
  * ͬʱ������ʹ�øô�������ɵ��κ�ֱ�ӻ�����ʧ�����߶��Ų�����
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
	 
#define OLED_IICx           I2C1
#define OLED_IIC_GPIOx      GPIOB
#define OLED_IIC_PIN        GPIO_Pin_6|GPIO_Pin_7
//#define OLED_IIC_REMAP      GPIO_Remap_I2C1
#define OLED_DMA1_Channel6
//#define OLED_DMA1_Channel4
 
#define OLED_SIZE_X 128
#define OLED_SIZE_Y 64	
#define OLED_PAGE_NUM OLED_SIZE_Y/8

#define USE_GRAM	//����ͼ�λ��棬�Լ����Ļ������ƺͻ��ƺ���
#define USE_PAGE	//����ҳ���ƺ�������DMA��ʽ��
#define SUPPORT_U8G2	//����u8g2������ͼ�ο⣬�������ã��ɽ���u8g2ɾ�����Լ��ٳ����С

#ifdef	SUPPORT_U8G2	
#define	USE_U8G2_EXAMPLE		//����u8g2���Ʋ�������
#ifndef USE_GRAM
#define USE_GRAM	//���ʹ��u8g2�⣬ǿ��ʹ��ͼ�λ���
#endif	/* USE_GRAM */
#endif	/* SUPPORT_U8G2 */

#ifdef	USE_GRAM
#define USE_GRAM_DRAW_BASE_FUNCTION	//ʹ��ͼ�λ���������ƺ���
#ifdef	USE_GRAM_DRAW_BASE_FUNCTION
#define OLED_COMPUT_TRANS_FPS					//���㴫��֡��
#define USE_GRAM_EXAMPLE		//����ͼ�λ���Ļ��Ʋ�������
#endif  /* USE_GRAM_DRAW_BASE_FUNCTION */
#endif	/* USE_GRAM */

#if defined OLED_COMPUT_TRANS_FPS || defined USE_GRAM_EXAMPLE	//����֡�ʺͲ���������Ҫʹ��һ���ɿ���ϵͳʱ���������������Ҫ�˹��ܿ���ע�͵�
#define OLED_SYSTIME_VARIABLE	times		//ϵͳʱ�������������
#define OLED_SYSTIME_PERIOD		10			//����һ�ε����ڣ���λ΢��us
#define OLED_SYSTIME_SIZE			0				//���������ķ�Χ��UINT32_MAX+1�ȼ���0
//������һ��ʾ����ʹ��SysTick->VAL�Ĵ�����Ϊϵͳʱ��
//#define OLED_SYSTIME_DOWN_CONT				//SysTick�Ǹ���������
//#define OLED_SYSTIME_USE_SYSTICK			//ʹ��SysTick->VAL��Ϊ��������ע�⸡��������ʱ�ϳ�
//#define OLED_SYSTIME_VARIABLE	SysTick->VAL
//#define OLED_SYSTIME_PERIOD		(1000000.0/((SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) ? SystemCoreClock : SystemCoreClock/8))			//����һ�ε����ڣ���λ΢��us
//#define OLED_SYSTIME_SIZE			SysTick->LOAD +1


#endif  /* OLED_COMPUT_TRANS_FPS || USE_GRAM_EXAMPLE */
	 
/**
  * @}
  */

#define IS_OLED_PAGE(PAGE) (PAGE >= 0 && PAGE < OLED_PAGE_NUM)
#define IS_OLED_COLUMN(COLUMN) (COLUMN >= 0 && COLUMN < OLED_SIZE_X)
	
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
//IIC�ӿڿ��ƺ���
int OLED_send_cmd(uint8_t o_command);
int OLED_send_data(uint8_t o_data);

//oled���ƺ���		   				   		    
void OLED_Init(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);

//�����ƺ���
void OLED_SetPage(uint8_t page);
void OLED_SetColumn(uint8_t col);
void OLED_SetPos(uint8_t page, uint8_t col);

//ҳд�뺯��
void OLED_Clear(void);
#ifdef	USE_PAGE
void OLED_Full(uint8_t val);
void OLED_ShowASCIIString(uint8_t page, uint8_t col, char *str);
void OLED_ShowCHZHString(uint8_t page, uint8_t col, char *str);
void OLED_ShowString(uint8_t page, uint8_t col, char *str);
#endif	/* USE_PAGE */

#ifdef USE_GRAM
//�Դ��������
void OLED_Clear_Gram(void);
int OLED_Refresh_Gram(int sync);
int OLED_Continuous_Refresh_Start(void);
void OLED_Continuous_Refresh_Stop(void);

#ifdef	USE_GRAM_DRAW_BASE_FUNCTION
//ͼ�λ���������ƺ���
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode);
void OLED_DrawByte(uint8_t x, uint8_t y,uint8_t data, uint8_t mode);
//void OLED_DrawChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
#define OLED_DrawChar(x,y,chr,size,mode) OLED_DrawASCIIChar(x,y,chr,size,mode)
void OLED_DrawASCIIChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void OLED_DrawCNZHString(uint8_t x, uint8_t y, char *str, uint8_t mode);
void OLED_DrawString(uint8_t x, uint8_t y, char *str, uint8_t size, uint8_t mode);

#ifdef	USE_GRAM_EXAMPLE
//ͼ�λ���������ƺ�����������
void OLED_Frame_Example(void);
void OLED_Simulated_Noise_Example(void);
void OLED_CNZH_Example(void);
#ifdef	USE_U8G2_EXAMPLE
//��ϲ�������
void OLED_Example_Loop(u8g2_t *u8g2);
#else		/* USE_U8G2_EXAMPLE */
//��ϲ�������
void OLED_Example_Loop(void);
#endif	/* USE_U8G2_EXAMPLE */
#endif  /* USE_GRAM_EXAMPLE */
#endif  /* USE_GRAM_DRAW_BASE_FUNCTION */

#ifdef SUPPORT_U8G2
//u8g2���������ƿ�֧�ֺ���
u8g2_t *OLED_U8G2_Init(void);
#endif	/* SUPPORT_U8G2 */

#ifdef	USE_U8G2_EXAMPLE
//u8g2���������ƿ�֧�ֺ�����������
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
