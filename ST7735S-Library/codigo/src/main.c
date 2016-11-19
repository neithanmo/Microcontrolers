#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "st7735.h"
#include "tabla.h"
#include "tabla2.h"


#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12



static void clock_setup(void)///
{
        rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_120MHZ]);
  /* apb1 = 30MHz
     apb2 = 60MHz usado por adc
     ahb = 120MHz USB, DMA, usb solo puede trabajar a un maximo de 120MHz*/

}

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
        gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN | GPIO15 | GPIO12 | GPIO13 | GPIO14 | GPIO3 | GPIO4 | GPIO5 | GPIO1);
}



int main(void)
{	
	clock_setup();
	spi_setup(SPI1);
	gpio_setup();
	lcd_backLight(1);
	init_lcd();
	delay_ms(500);
	uint8_t dx,dy;
	dx = 0;
        dy = 0;
        lcd_setAddrWindow(0,128,0,159);
	while (1) {
		 lcd_FillRect(0, 0, 128, 160, COLOR565_BLACK);
		  dy=0;
                  while((80-dy)){
			lcd_Pixel(64, 79+dy, COLOR565_GOLD);
			dy++;
		  }
		  dx=0;
                  while((64-dx)){
			lcd_Pixel(63+dx, 80, COLOR565_GOLD);
			dx++;
		  }
		  dy=0;
                  while((80-dy)){
			lcd_Pixel(64, 80-dy, COLOR565_GOLD);
			dy++;
		  }
		  dx=0;
                  while((64-dx)){
			lcd_Pixel(64-dx, 80, COLOR565_GOLD);
			dx++;
		  }
		  dx=0;
                  while((64-dx)){
			drawCircle(64, 80, dx+15, COLOR565_GOLD+dx);
			dx++;
		  }
		  delay_ms(5000);
		 
		 lcd_FillRect(0, 0, 128, 160, COLOR565_BLACK);
		 fillTriangle(45, 40, 105, 40, 75, 150, COLOR565_SANDY_BROWN);
		 delay_ms(5000);
		 lcd_Clear(COLOR565_TEAL);
		 drawRoundRect(10, 20, 110,110, 50, COLOR565_MEDIUM_TURQUOISE);
		 delay_ms(5000);
		 fillRoundRect(10, 20, 110,110, 50, COLOR565_SANDY_BROWN);	 
		 delay_ms(5000);
		 lcd_setAddrWindow(0,128,0,159);
	         uint16_t i,j;
		 for(j=0;j<20480;j++){
			push_color(imagen_tabla[j]);
		 }
		 delay_ms(10000);
		 for(j=0;j<20480;j++){
			push_color(imagen2_tabla[j]);
		 }
		 delay_ms(10000);
		 for(j=20480;j>0;j--){
			push_color(imagen2_tabla[j]);
		 }

//################################# circulos2 #####################################
		 delay_ms(5000);
		 drawCircle(16, 32, 16, imagen_tabla[512]);
		 delay_ms(1000);
		 drawCircle(48, 32, 16, imagen_tabla[1536]);
		 delay_ms(1000);
		 drawCircle(80, 32, 16, imagen_tabla[2560]);
		 delay_ms(1000);
		 drawCircle(112, 32, 16, imagen_tabla[3584]);
//################################# circulos3 #####################################
		 delay_ms(5000);
		 drawCircle(16, 64, 16, imagen2_tabla[1024]);
		 delay_ms(1000);
		 drawCircle(48, 64, 16, imagen2_tabla[3072]);
		 delay_ms(1000);
		 drawCircle(80, 64, 16, imagen2_tabla[5120]);
		 delay_ms(1000);
		 drawCircle(112, 64, 16, imagen2_tabla[7168]);
//################################# circulos4 #####################################
		 delay_ms(5000);
		 drawCircle(16, 96, 16, imagen_tabla[1535]);
		 delay_ms(1000);
		 drawCircle(48, 96, 16, imagen_tabla[4608]);
		 delay_ms(1000);
		 drawCircle(80, 96, 16, imagen_tabla[7680]);
		 delay_ms(1000);
		 drawCircle(112, 96, 16, imagen_tabla[10752]);
//################################# circulos5 #####################################
		 delay_ms(5000);
		 drawCircle(16, 128, 16, imagen2_tabla[16*128]);
		 delay_ms(1000);
		 drawCircle(48, 128, 16, imagen2_tabla[48*128]);
		 delay_ms(1000);
		 drawCircle(80, 128, 16, imagen2_tabla[80*128]);
		 delay_ms(1000);
		 drawCircle(112, 128, 16, imagen2_tabla[112*128]);
//################################# fin circulos ###############################
                 lcd_setAddrWindow(0,128,0,159);
		 for(j=0;j<20480;j++){
			push_color(imagen_tabla[j]);
		 }
		 delay_ms(10000);

		 for(j=0;j<20480;j++){
			push_color(RGB565(getBlue(imagen_tabla[j]), getGreen(imagen_tabla[j]), getRed(imagen_tabla[j])));
		 }
		 for(j=0;j<20480;j++){
			push_color(RGB565(getGreen(imagen_tabla[j]), getBlue(imagen_tabla[j]), getRed(imagen_tabla[j])));
		 }
		 for(j=0;j<20480;j++){
			push_color(RGB565(getGreen(imagen_tabla[j]), getRed(imagen_tabla[j]), getBlue(imagen_tabla[j])));
		 }
		 for(j=0;j<20480;j++){
			push_color(swapcolor(imagen_tabla[j]));
		 }
		st_PutStr5x7(1, 5, 10, "NATANAEL", COLOR565_RED, COLOR565_WHITE);
		 st_PutStr5x7(1, 5, 20, "MOJICA", COLOR565_RED, COLOR565_WHITE);
		 st_PutStr5x7(1, 5, 30, "JIMENEZ", COLOR565_RED, COLOR565_WHITE);
		 st_PutStr5x7(1, 60, 40, "LAB", COLOR565_RED, COLOR565_WHITE);
		 st_PutStr5x7(1, 10, 50, "MICROCONTROLADORES", COLOR565_RED, COLOR565_WHITE);
		 delay_ms(10000);
        	lcd_setAddrWindow(0,128,0,159);

		 for(j=0;j<10240;j++){
			push_color(imagen2_tabla[j]);
		 }
		 delay_ms(10000);
		 for(j=10240;j<20480;j++){
			push_color(imagen_tabla[j]);
		 }
		 delay_ms(10000);
		st_PutStr5x7(1, 10, 50, "ST7735S", COLOR565_BLACK, COLOR565_WHITE);
		 delay_ms(10000);
		st_PutStr5x7(1, 1, 70, "Library for STM32F411", COLOR565_STEEL_BLUE, COLOR565_WHITE);

		lcd_setAddrWindow(0,128,80,160);
		 delay_ms(10000);
		 for(j=0;j<10240;j++){
			push_color(imagen2_tabla[j]);
		 }
		 delay_ms(10000);
        	lcd_setAddrWindow(0,128,0,80);
		 for(j=20480;j>10420;j--){
			push_color(imagen2_tabla[j]);
		 }


        	lcd_setAddrWindow(64,128,0,160);
		 delay_ms(10000);
		for(j=0;j<160;j++){
			for(i=64;i<128;i++){
				lcd_Pixel(i, j, imagen2_tabla[i+(j*128)]);
			}
		}
        	lcd_setAddrWindow(0,64,0,160);
		for(j=0;j<160;j++){
			for(i=0;i<64;i++){
				lcd_Pixel(i, j, imagen2_tabla[i+(j*128)]);
			}
		}
                 for(dy=160;dy>0;dy--){
  		     lcd_HLine(0, dy,128,RGB565(dy,94+dy,255-dy));
		 }
        	lcd_setAddrWindow(0,128,0,160);
		for(j=0;j<160;j++){
			for(i=0;i<128;i++){
				if((j>20)&(j<74))
					lcd_Pixel(i, j, imagen2_tabla[i+(j*128)]);
				else
					lcd_Pixel(i, j, imagen_tabla[i+(j*128)]);
			}
		}
		 delay_ms(8000);
		for(j=0;j<160;j++){
			for(i=0;i<128;i++){
				if((j>20)&(j<74))
					lcd_Pixel(i, j, imagen_tabla[i+(j*128)]);
			}
		}
		 delay_ms(10000);				

	}
	return 0;
}
