#include "usb.h"
#include "pid.h"
#include "st7735.h"



#define PWM_PERIOD 2999
#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12
//usbd_ep_write_packet(usbd_dev, 0x82, buf, len)

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
	//rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPCEN);
        gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN | GPIO15 | GPIO12 | GPIO13 | GPIO14 | GPIO3 | GPIO4 | GPIO5 | GPIO1);
	gpio_set(LED_DISCO_GREEN_PORT, GPIO12);
	gpio_clear(LED_DISCO_GREEN_PORT, GPIO14);

	/* D/C =  conectado al pin 3 del puerto c
          D/C = 1; data
          D/C = 0; commando de control
	*/
}



int main(void)
{	
	clock_setup();
	gpio_setup();
	spi_setup(SPI1);
	lcd_backLight(1);
	init_lcd();
	delay_ms(500);
	//lcd_FillRect(0,0,128,160, COLOR565_DARK_CYAN);
	//delay_ms(500);
	//lcd_VLine(110,0,155,COLOR565_MAROON);
	//delay_ms(500);
	//lcd_HLine(0,128,75, COLOR565_FUCHSIA);
	//lcd_FillRect(0, 0, 120, 100, COLOR565_RED);
	uint8_t dx,dy;
	dx = 0;
        dy = 0;
	while (1) {
		 //lcd_HLine(0, 50,128,COLOR565_CHOCOLATE);
                 //lcd_VLine(50, 0, 128, COLOR565_BLACK);
		 
                 for(dy=0;dy<160;dy++){
		     for(dx=0;dx<128;dx++){
			lcd_Pixel(dx,dy,COLOR565_DARK_RED);
		     }
		 }
                 for(dy=160;dy>0;dy--){
		     for(dx=128;dx>0;dx--){
			lcd_Pixel(dx,dy,COLOR565_DARK_BLUE);
		     }
		 }
		 //dy>160 ? dy=0 : dy++;
		 //dx>128 ? dx=0 : dx++;
		 //lcd_FillRect(25, 25, 100, 100, COLOR565_BLACK);
		 /*if(gpio_get(GPIOA, GPIO0)){
		 	//lcd_FillRect(25, 0, 50, 50, COLOR565_YELLOW);
			lcd_HLine(0,dx,100,COLOR565_CHOCOLATE);
			delay_ms(500);
		 }*/
		 //delay_ms(500);
		  
		}
	return 0;
}
