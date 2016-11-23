#ifndef ST7335_H
#define ST7335_H

/** @brief 
@file
Esta es una libreria para utilizar la pantalla TFT ST7735S utilizando la tarjeta STM32F4/F3,
	se baso en la libreria de adafruit la cual es compatible con arduino y escrita completamente en C++.
	se integran ciertas funcionalidades y primitivas encontradas en la libraria de adafruit, sin embargo se omiten otras por compatibilidad
	Esta libreria se encarga de la inicialización de alguno de los 3 SPI disponibles en las tarjetas ya mencionadas.
    @author Natanael Mojica Jimenez
    @date Noviembre 2016
    */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/exti.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "color.h"
#include "font5x7.h"


#define SPI_PORT SPI //using SPI1
#define LCD_CONTROL_PORT  GPIOD ///puerto de control para reloj y data
/** 
      @brief usado para controlar la luminosidad de la pantalla
  */
#define BLK_PIN GPIO3
/** 
     @brief gpio para hardware reset de la pantalla
  */
#define RST_PIN GPIO1
/** 
      @brief gpio para "chip select"
  */
#define CS_PIN GPIO5 //CS* ???
/** 
      @brief gpio "DATA/COMMAND"
  */
#define DC_PIN GPIO4

/** 
      @brief set low the DC pin
  */
#define DC_L() gpio_clear(LCD_CONTROL_PORT, DC_PIN)
/** 
      @brief set hight the DC pin
  */
#define DC_H() gpio_set(LCD_CONTROL_PORT, DC_PIN)
/** 
      @brief set low the CS pin
  */
#define CS_L() gpio_clear(LCD_CONTROL_PORT, CS_PIN)
/** 
      @brief set hight the CS pin
  */
#define CS_H() gpio_set(LCD_CONTROL_PORT, CS_PIN)
/** 
      @brief set low the RST pin
  */
#define RST_L() gpio_clear(LCD_CONTROL_PORT, RST_PIN)
/**
      @brief set hight the RST pin
  */
#define RST_H() gpio_set(LCD_CONTROL_PORT, RST_PIN)
#ifndef _swap_uint8_t
/** 
      @brief swap data a, b, function is not part of the API, but is a utility
  */
#define _swap_uint8_t(a, b) { uint8_t t = a; a = b; b = t; }
#endif


#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1


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
  scr_CW     = 1,/// X-Y Exchange,Y-Mirror
  scr_CCW    = 2,/// X-Y Exchange,X-Mirror
  scr_180    = 3 /// X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
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

/** 
      @brief retorna el color de un pixel RGB565 de 16 bits.
      @param R nivel de rojo, valor entre 0 y 255(unsigned char)
      @param G nivel de verde, valor entre 0 y 255(unsigned char)
      @param B nivel de azul, valor entre 0 y 255(unsigned char)
      \warning Esta funcion retorna un valor de color de 16 bits. Lo cual es compatible con la ST7735S, 
	si quiere utilizar una mayor cantidad de bits de color debe de  configurar la 
	tarjeta con el comando adecuado, por defecto se configura en 16-bits
  */
uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B);
/** 
      @brief Intercambia el orden de los componentes de color en un pixel, de RGB a BGR
      @param x color actual del pixel en 16 bits.
  */
uint16_t swapcolor(uint16_t x);
/** 
      @brief retorna el nivel de rojo de un pixel 
      @param rgbPixel color actual del pixel en 16 bits.
  */
uint8_t getRed(uint16_t rgbPixel);
/** 
      @brief retorna el nivel de azul de un pixel 
      @param rgbPixel color actual del pixel en 16 bits.
  */
uint8_t getBlue(uint16_t rgbPixel);
/** 
      @brief retorna el nivel de verde de un pixel 
      @param rgbPixel color actual del pixel en 16 bits.
  */
uint8_t getGreen(uint16_t rgbPixel);
/** 
      @brief retardo en milisegundos 
  */
void delay_ms(const uint32_t delay);
/** 
      @brief retardo en microsegundos 
  */
void delay_us(const uint32_t delay);
/** 
      @brief envia datos al SPI, es decir a la tarjeta ST7735S, internamente pone en alto la 
	bandera DC, para indicar a la tarjeta que se envian datos
  */
void write_data_lcd(uint8_t data);
/** 
      @brief envia datos al SPI, es decir a la tarjeta ST7735S, internamente pone en bajo la 
	bandera DC, para indicar a la tarjeta que se envian comandos de configuracion
  */
void write_cmd_lcd(uint8_t cmd);
/** 
      @brief inicializa la pantalla,aqui se ajusta el formato y bits de color, etc.
  */
void init_lcd(void);
/** 
      @brief Inicializa el periférico SPI a utilizar 
      @param SPI : puede ser SPI1, SPI2 O SPI3
  */
void spi_setup(uint32_t SPI);
/** 
      @brief activa la parte de la pantalla que se va a colorear. En cordenadas X-Y(2D), 
	por defecto X=0-128 y Y=0-160, es posible cambiar la orientación.
      @param XS : x inicial
      @param XE : x final
      @param YS : y inicial
      @param YE : y final
  */
void lcd_setAddrWindow(uint8_t XS, uint8_t YS,uint8_t XE, uint8_t YE);
/** 
      @brief Utilizada para cambiar la orientación de la pantalla
      @param orientation : enum con 1 de los cuatro posibles modos de orientacion
  */
void lcd_orientacion(ScrOrientation_TypeDef orientation);
/** 
      @brief Llenar la pantalla de un solo color
      @param color : color RGB565, alguno de los ya definidos en color.h
  */
void lcd_Clear(uint16_t color);
/** 
      @brief colorear un pixel en específico
      @param X : coordenada en X del pixel
      @param Y : coordenada en y del pixel
      @param color : color del pixel
      \warning : tener cuidado de que las coordenadas XY del pixel esten dentro de los valores actuales de X e Y,
        en la orientacion por defecto lcd_orientacion usando ScrOrientation_TypeDef, src_Normal, x va desde 0 a 128 e Y de 0 a 160
  */
void lcd_Pixel(uint16_t X, uint16_t Y, uint16_t color);
/** 
      @brief Dibuja una linea horizontal que iniciará en el punto xy
      @param w : ancho de la linea
      @param color : color de la linea

  */
void lcd_HLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
/** 
      @brief Dibuja una linea vertical que iniciará en el punto xy
      @param h : alto de la linea
      @param color : color de la linea

  */
void lcd_VLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
/** 
      @brief Dibuja una linea que une los puntos X1Y1 a X2Y2

  */
void lcd_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color);
/** 
      @brief Funcion para dibujar un rectangulo
      @param X1 punto inicial en X
      @param Y1 punto inicial en y
      @param X2 ancho del rectangulo en X a partir de X1
      @param Y2 alto del rectangulo a partir del punto Y2
      @param color color de las lineas con las que se dibuja el rectangulo
      \warning : el alto y ancho del rectangulo no deben sobrepasar las dimensiones de la pantalla

  */
void lcd_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);
/** 
      @brief Rellena un rectangulo con un determinado color
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho del rectangulo en X a partir de X1
      @param h alto del rectangulo a partir del punto Y2
      @param color utilizado para rellenar el rectangulo
      \warning : el alto y ancho del rectangulo no deben sobrepasar las dimensiones de la pantalla

  */
void lcd_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
/** 
      @brief enciende o apaga la luz de fondo de la pantalla
      @param on cero=apaga, on>0, la enciende
  */
void lcd_backLight(uint8_t on);
/** 
      @brief Dibujar un circulo
      @param x0 coordenada en X del centro del circulo
      @param y0 coordenada en ydel centro del circulo
      @param r radio del circulo
      @param color color del circulo
      \warning : cuanto mas grande sea @scale mas tarda en dibujarse en la pantalla
  */
void drawCircle(uint8_t x0, uint8_t y0, uint8_t r,uint16_t color);
/** 
      @brief Funcion auxiliar utilizada por drawCircle y dibujar aun mas rapidamente el circulo
  */
void drawCircleHelper(uint8_t x0, uint8_t y0, uint8_t r, uint8_t cornername, uint16_t color);
/** 
      @brief Dibujar y rellenar un circulo
      @param x0 coordenada en X del centro del circulo
      @param y0 coordenada en ydel centro del circulo
      @param r radio del circulo
      @param color color del relleno
    */
void fillCircle(uint8_t x0, uint8_t y0, uint8_t r,uint16_t color);
/** 
      @brief Funcion auxiliar utilizada por fillCircle y dibujar/rellenar el  circulo
  */
void fillCircleHelper(uint8_t x0, uint8_t y0, uint8_t r,uint8_t cornername, uint8_t delta, uint16_t color);
/** 
      @brief Dibujar un triangulo, dibuja tres lineas que unen los tres vertices del triangulo, cada vertice con coordenadas
            x0y0, x1y1,x2y2.
      @param color color de las lineas
    */
void drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint8_t x2, uint8_t y2, uint16_t color);
/** 
      @brief Dibuja un rectangulo utilizando un algoritmo diferente al usado en lcd_Rect
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho del rectangulo en X a partir de X1
      @param h alto del rectangulo a partir del punto Y2
      @param color utilizado para rellenar el rectangulo
      \warning el alto y ancho del rectangulo no deben sobrepasar las dimensiones de la pantalla

  */
void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
 uint16_t color) ;
 /** 
      @brief Dibuja una linea,utiliza otro algoritmo al empleado por lcd_Line

  */
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
 uint16_t color);
 /** 
      @brief Dibuja una linea horizontal ,utilizando  drawLine()

  */
void drawFastVLine(uint8_t x, uint8_t y,
 uint8_t h, uint16_t color);
 /** 
      @brief Dibuja una linea horizontal ,utilizando  drawLine()

  */
void drawFastHLine(uint8_t x, uint8_t y,
 uint8_t w, uint16_t color);
/** 
      @brief Dibuja/rellena un triangulo, utilizando tres lineas que unen los tres vertices del triangulo, cada vertice con coordenadas
            x0y0, x1y1,x2y2.
      @param color color del relleno
    */
void fillTriangle(uint8_t x0, uint8_t y0,
 uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
/** 
      @brief Rellena un rectangulo con un determinado color, utiliza un algoritmo diferente al utilizado en lcd_FillRect, el cual es utili para dibujo de pequeños rectangulos.
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho del rectangulo en X a partir de X1
      @param h alto del rectangulo a partir del punto Y2
      @param color utilizado para rellenar el rectangulo
      \warning Esta funcion es mucho más lenta que lcd_FillRect, pero más preciso

  */
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
 uint16_t color);
/** 
      @brief Dibuja un Round rectangulo con un determinado color.
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho del rectangulo en X a partir de X1
      @param r radio del circulo interno
      @param color utilizado para rellenar el rectangulo

  */
void drawRoundRect(uint8_t x, uint8_t y, uint8_t w,
 uint8_t, uint8_t r, uint16_t color);
/** 
      @brief Rellena un Round rectangulo con un determinado color.
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho del rectangulo en X a partir de X1
      @param r radio del circulo interno
      @param color utilizado para rellenar el rectangulo

  */
void fillRoundRect(uint8_t x, uint8_t y, uint8_t w,
 uint8_t h, uint8_t r, uint16_t color);
/** 
      @brief escribe un arreglo de chars (strings)
      @param scale tamaño del string
      @param x coordenada en X
      @param y coordenada en y
      @param str string
      @param bgcolor color de fondo del string
      @param color color del string
      \warning scale: cuanto mas grande sea scale mas tarda en dibujarse en la pantalla
  */
void st_PutStr5x7(uint8_t scale, uint8_t X, uint8_t Y, char *str, uint16_t color, uint16_t bgcolor);
/** 
      @brief escribe un solo char en la pantalla, el fond utilizado se encuentra en font5x7.h
      @param x coordenada en X
      @param y coordenada en y
      @param c char a escribir
      @param bg color de fondo del char
      @param color color del char
      @param size tamaño del char en pantalla
      \warning size : cuanto mas grande sea mas tarda en dibujarse en la pantalla
  */
void drawChar(uint8_t x, uint8_t y, unsigned char c,
 uint16_t color, uint16_t bg, uint8_t size);
/** 
      @brief Dibuja un mapa de bits extraido de una imagen, esta función es obsoleta, ya que es posible utilizando funciones como push_color(), lcd_Pixel(), lcd_setAddrWindow() dibujar imagenes a color y en blanco y negro a partir de una tabla de pixeles, esta ultima se obtiene con las funciones encontradas en el archivo makeColorTable.cpp ver el demo en main.c para entender su uso.
      @param x punto inicial en X
      @param y punto inicial en y
      @param w ancho de la imagen
      @param h alto de la imagen
      @param color del pixel en alto
      @param bg del pixel en bajo

  */
void drawBitmap(uint8_t x, uint8_t y,
 uint8_t *bitmap, uint8_t w, uint8_t h, uint16_t color, uint16_t bg);
#endif
