#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "../Inc/oled_driver.h"
#include "../Inc/oled_font.h"
#include "../Inc/bmp.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"




__IO float usDelayBase;

void PY_usDelayTest(void)
{
	__IO uint32_t firstms, secondms;
	__IO uint32_t counter = 0;

	firstms = HAL_GetTick() + 1;
	secondms = firstms + 1;

	while (uwTick != firstms)
		;

	while (uwTick != secondms)
		counter++;

	usDelayBase = ((float)counter) / 1000;
}

void PY_Delay_us_t(uint32_t Delay)
{
	__IO uint32_t delayReg;
	__IO uint32_t usNum = (uint32_t)(Delay * usDelayBase);

	delayReg = 0;
	while (delayReg != usNum)
		delayReg++;
}

void PY_usDelayOptimize(void)
{
	__IO uint32_t firstms, secondms;
	__IO float coe = 1.0;

	firstms = HAL_GetTick();
	PY_Delay_us_t(1000000);
	secondms = HAL_GetTick();

	coe = ((float)1000) / (secondms - firstms);
	usDelayBase = coe * usDelayBase;
}

void PY_Delay_us(uint32_t Delay)
{
	__IO uint32_t delayReg;

	__IO uint32_t msNum = Delay / 1000;
	__IO uint32_t usNum = (uint32_t)((Delay % 1000) * usDelayBase);

	if (msNum > 0)
		HAL_Delay(msNum);

	delayReg = 0;
	while (delayReg != usNum)
		delayReg++;
}

void py_f2s4printf(char *stra, float x, uint8_t flen)
{
  uint32_t base;
  int64_t dn;
  char mc[32];

  base = pow(10, flen);
  dn = x * base;
  sprintf(stra, "%d.", (int)(dn / base));
  dn = abs(dn);
  if (dn % base == 0)
  {
    for (uint8_t j = 1; j <= flen; j++)
    {
      stra = strcat(stra, "0");
    }
    return;
  }
  else
  {
    if (flen == 1)
    {
      sprintf(mc, "%d", (int)(dn % base));
      stra = strcat(stra, mc);
      return;
    }

    for (uint8_t j = 1; j < flen; j++)
    {
      if ((dn % base) < pow(10, j))
      {
        for (uint8_t k = 1; k <= (flen - j); k++)
        {
          stra = strcat(stra, "0");
        }
        sprintf(mc, "%d", (int)(dn % base));
        stra = strcat(stra, mc);
        return;
      }
    }
    sprintf(mc, "%d", (int)(dn % base));
    stra = strcat(stra, mc);
    return;
  }
}

// OLED ACCESS
// IIC Start
void IIC_Start(void)
{
	PY_Delay_us_t(us_num);
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	PY_Delay_us_t(us_num);
	OLED_SDIN_Clr();
	PY_Delay_us_t(us_num);
}

// IIC Stop
void IIC_Stop(void)
{
	OLED_SCLK_Set();
	PY_Delay_us_t(us_num);
	OLED_SDIN_Clr();
	PY_Delay_us_t(us_num);
	OLED_SCLK_Set();
	PY_Delay_us_t(us_num);
	OLED_SDIN_Set();
	PY_Delay_us_t(us_num);
}

uint8_t IIC_Wait_Ack(void)
{
	uint8_t status = 0;

	OLED_SDIN_Set();
	PY_Delay_us_t(us_num);
	status = OLED_SDIN_Read();
	OLED_SCLK_Set();
	PY_Delay_us_t(us_num);
	OLED_SCLK_Clr();
	return status;
}

// IIC Write byte
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m, da;
	da = IIC_Byte;
	OLED_SCLK_Clr();
	for (i = 0; i < 8; i++)
	{
		m = da;
		//	OLED_SCLK_Clr();
		m = m & 0x80;
		if (m == 0x80)
		{
			OLED_SDIN_Set();
		}
		else
			OLED_SDIN_Clr();
		da = da << 1;
		PY_Delay_us_t(us_num);
		OLED_SCLK_Set();
		PY_Delay_us_t(us_num);
		OLED_SCLK_Clr();
	}
}
// IIC Write Command
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78); // Slave address,SA0=0
	IIC_Wait_Ack();
	Write_IIC_Byte(0x00); // write command
	IIC_Wait_Ack();
	Write_IIC_Byte(IIC_Command);
	IIC_Wait_Ack();
	IIC_Stop();
}
// IIC Write Data
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78); // D/C#=0; R/W#=0
	IIC_Wait_Ack();
	Write_IIC_Byte(0x40); // write data
	IIC_Wait_Ack();
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();
	IIC_Stop();
}
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
	if (cmd)
	{

		Write_IIC_Data(dat);
	}
	else
	{
		Write_IIC_Command(dat);
	}
}

// fill_Picture
void fill_picture(unsigned char fill_Data)
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		OLED_WR_Byte(0xb0 + m, 0); // page0-page1
		OLED_WR_Byte(0x00, 0);	   // low column start address
		OLED_WR_Byte(0x10, 0);	   // high column start address
		for (n = 0; n < 128; n++)
		{
			OLED_WR_Byte(fill_Data, 1);
		}
	}
}
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC
	OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC
	OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	}
}
void OLED_On(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(1, OLED_DATA);
	}
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

uint32_t oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
}

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t;
	OLED_Set_Pos(x, y);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
}

void OLED_Init(void)
{
	PY_Delay_us_t(200000);
	OLED_WR_Byte(0xAE, OLED_CMD);

	OLED_WR_Byte(0x40, OLED_CMD); //---set low column address
	OLED_WR_Byte(0xB0, OLED_CMD); //---set high column address

	OLED_WR_Byte(0xC8, OLED_CMD); //-not offset

	OLED_WR_Byte(0x81, OLED_CMD);
	OLED_WR_Byte(0xff, OLED_CMD);

	OLED_WR_Byte(0xa1, OLED_CMD);

	OLED_WR_Byte(0xa6, OLED_CMD);

	OLED_WR_Byte(0xa8, OLED_CMD);
	OLED_WR_Byte(0x1f, OLED_CMD);

	OLED_WR_Byte(0xd3, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_WR_Byte(0xd5, OLED_CMD);
	OLED_WR_Byte(0xf0, OLED_CMD);

	OLED_WR_Byte(0xd9, OLED_CMD);
	OLED_WR_Byte(0x22, OLED_CMD);

	OLED_WR_Byte(0xda, OLED_CMD);
	OLED_WR_Byte(0x02, OLED_CMD);

	OLED_WR_Byte(0xdb, OLED_CMD);
	OLED_WR_Byte(0x49, OLED_CMD);

	OLED_WR_Byte(0x8d, OLED_CMD);
	OLED_WR_Byte(0x14, OLED_CMD);

	OLED_WR_Byte(0xaf, OLED_CMD);
	OLED_Clear();
}
