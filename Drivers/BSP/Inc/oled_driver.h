#ifndef __OLED_DRIVER_H__
#define __OLED_DRIVER_H__

#include <stdint.h>

#define us_num 25

#define OLED_MODE   0
#define SIZE        8
#define XLevelL		0x00
#define XLevelH		0x104
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET) //SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)//SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define OLED_SDIN_Read() HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)

#define OLED_CMD  0
#define OLED_DATA 1

void PY_usDelayTest(void);
void PY_Delay_us_t(uint32_t Delay);
void PY_usDelayOptimize(void);
void PY_Delay_us(uint32_t Delay);
void py_f2s4printf(char *stra, float x, uint8_t flen);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void Write_IIC_Byte(unsigned char IIC_Byte);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void fill_picture(unsigned char fill_Data);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_On(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_Init(void);

#endif  /*__OLED_DRIVER_H__*/
