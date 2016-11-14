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
	uint8_t dx,dy;
	dx = 0;
        dy = 0;
	while (1) {

                 for(dy=0;dy<160;dy++){
  		     lcd_HLine(0, dy,128,COLOR565_CHOCOLATE);
		 }
		 for(dy=0;dy<80;dy++){
		     for(dx=0;dx<64;dx++){
                         lcd_Pixel(64+dx, 80+dy, COLOR565_GOLD);
			 lcd_Pixel(64-dx, 80-dy, COLOR565_GOLD);
 		     }
                  }
                 for(dx=0;dx<128;dx++){
		     lcd_VLine(dx, 0, 160,RGB565(dx,255-dx,127+dx));
		 }
                 for(dy=160;dy>0;dy--){
		     lcd_HLine(0, dy,128,RGB565(94+dy,dy,255-dy));
		 }
                 for(dx=128;dx>0;dx--){
		     lcd_VLine(dx, 0, 160,RGB565(255-dx,127+dx,dx));
		 }
                 for(dy=0;dy<160;dy++){
  		     lcd_HLine(0, dy,128,RGB565(dy,94+dy,255-dy));
		 }
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
			drawCircle(64, 80, dx, COLOR565_GOLD+dx);
			dx++;
		  }
		  delay_ms(5000);
		 lcd_FillRect(0, 0, 128, 160, COLOR565_BLACK);
	         drawRect(20,40,75,95, COLOR565_MEDIUM_TURQUOISE);
		 delay_ms(20000);
		 fillRect(20,40,75,95, COLOR565_TEAL);
		 delay_ms(9000);
		 lcd_Clear(COLOR565_DEEP_PINK);
		 delay_ms(5000);
		 fillTriangle(45, 40, 105, 40, 75, 150, COLOR565_DARK_OLIVE_GREEN);
		 delay_ms(10000);
		 fillRect(0, 0, 128, 160,COLOR565_TEAL);
		 drawRoundRect(10, 20, 110,110, 50, COLOR565_MEDIUM_TURQUOISE);
		 delay_ms(10000);
		 fillRoundRect(10, 20, 110,110, 50, COLOR565_SANDY_BROWN);
		 delay_ms(50000);

		}
	return 0;
}
