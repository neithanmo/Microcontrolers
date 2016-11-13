#ifndef ST7335_H
#define ST7335_H
#include <stdlib.h>
#include <assert.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/spi.h>
#include "color.h"


#define scr_w  128
#define scr_h 160

#define SPI_PORT SPI //using SPI1
#define LCD_CONTROL_PORT  GPIOD ///puerto de control para reloj y data
#define LCD_DATA_PORT GPIOA
#define BLK_PIN GPIO3
#define RST_PIN GPIO1
#define CS_PIN GPIO5 //CS* ???
#define DC_PIN GPIO4

#define DC_L() gpio_clear(LCD_CONTROL_PORT, DC_PIN)
#define DC_H() gpio_set(LCD_CONTROL_PORT, DC_PIN)
#define CS_L() gpio_clear(LCD_CONTROL_PORT, CS_PIN)
#define CS_H() gpio_set(LCD_CONTROL_PORT, CS_PIN)
#define RST_L() gpio_clear(LCD_CONTROL_PORT, RST_PIN)
#define RST_H() gpio_set(LCD_CONTROL_PORT, RST_PIN)

#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT_18  160

/* ST7735 Commands */
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define WIDTH	128
#define HEIGHT	160
typedef enum
{
  scr_normal = 0,
  scr_CW     = 1,
  scr_CCW    = 2,
  scr_180    = 3
} ScrOrientation_TypeDef;

enum st7735_cmd {
	ST7735_START,
	ST7735_END,
	ST7735_CMD,
	ST7735_DATA,
	ST7735_DELAY
};

struct st7735_function {
	uint16_t cmd;
	uint16_t data;
};


extern uint16_t scr_width;
extern uint16_t scr_height;


uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B);//
uint16_t swapcolor(uint16_t x);//

void delay_ms(const uint32_t delay);//
void init_lcd(void);//
void spi_setup(uint32_t SPI);//
void lcd_setAddrWindow(uint8_t XS, uint8_t YS,uint8_t XE, uint8_t YE);//
void lcd_orientacion(ScrOrientation_TypeDef orientation);//
void lcd_Clear(uint16_t color);//
void lcd_Pixel(uint16_t X, uint16_t Y, uint16_t color);//
void lcd_HLine(uint16_t X1, uint16_t X2, uint16_t Y, uint16_t color);//
void lcd_VLine(uint16_t X, uint16_t Y1, uint16_t Y2, uint16_t color);//
void lcd_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color);//
void lcd_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);//
void lcd_FillRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);//
void lcd_backLight(uint8_t on);
void delay_ms(const uint32_t delay);
void write_data_lcd(uint8_t data);//
void write_cmd_lcd(uint8_t cmd);//
void lcd_PutChar7x11(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color, uint16_t bgcolor);
void lcd_PutStr7x11(uint8_t X, uint8_t Y, char *str, uint16_t color, uint16_t bgcolor);


#endif
