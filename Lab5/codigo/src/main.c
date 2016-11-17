#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "usb.h"
#include "st7735.h"



#define PWM_PERIOD 2999
#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12
//usbd_ep_write_packet(usbd_dev, 0x82, buf, len)
uint16_t image_buffer[128*160];
bool new_image;
usbd_device *usbd_dev;


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
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); //para usar el boton
	gpio_clear(LED_DISCO_GREEN_PORT, GPIO14);

	/* D/C =  conectado al pin 3 del puerto c
          D/C = 1; data
          D/C = 0; commando de control
	*/
}

static void usb_setup(void){

        rcc_periph_clock_enable(RCC_OTGFS);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);
}

static int cdcacm_control_request(usbd_device *usbd,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
        void (**complete)(usbd_device *usbd_dev1, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
        (void)usbd;
	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		gpio_toggle(LED_DISCO_GREEN_PORT, GPIO15);
                timer_enable_irq(TIM2, TIM_DIER_CC1IE);
		return 1;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding)) {
			return 0;
		}

		return 1;
	}
	return 0;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_read, uint8_t ep)///leer el setpoint enviado por el usuario
{
	(void)ep;
	int len = usbd_ep_read_packet(usbd_read, 0x01, image_buffer, sizeof(image_buffer));
	gpio_toggle(GPIOD,GPIO14);
	//(buf[0]='i') ? (new_image=true) : (new_image=false);
	/*if (len) {
		if(buf[0] != )
		//while (usbd_ep_write_packet(usbd_dev, 0x82, buf, len) == 0);
                image_buffer[i++] = buf[0];;
                gpio_toggle(LED_DISCO_GREEN_PORT, GPIO14);
	}*/
}



static void cdcacm_set_config(usbd_device *usbd, uint16_t wValue)
{
	(void)wValue;
	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);///lectura
        usbd_ep_setup(usbd, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
        usbd_ep_setup(usbd, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(
                                usbd,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}



int main(void)
{	
	clock_setup();
	gpio_setup();
	spi_setup(SPI1);
	lcd_backLight(1);
	init_lcd();
	delay_ms(500);
	new_image=false;
	usb_setup();

	usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
			usb_strings, 3,
			usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);

	uint8_t dx,dy;
	dx = 0;
        dy = 0;
        lcd_setAddrWindow(0,128,0,159);
	usbd_poll(usbd_dev);
	while (1) {
		usbd_poll(usbd_dev);
        	lcd_setAddrWindow(0,128,0,159);
		if(gpio_get(GPIOA,GPIO0)){
			new_image=true;
			gpio_toggle(GPIOD,GPIO14);
		}
                 /*for(dy=0;dy<160;dy++){
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
		 lcd_Clear(COLOR565_DEEP_PINK);
		 delay_ms(5000);
		 fillTriangle(45, 40, 105, 40, 75, 150, COLOR565_DARK_OLIVE_GREEN);
		 delay_ms(10000);
		 lcd_Clear(COLOR565_TEAL);
		 drawRoundRect(10, 20, 110,110, 50, COLOR565_MEDIUM_TURQUOISE);
		 delay_ms(10000);
		 fillRoundRect(10, 20, 110,110, 50, COLOR565_SANDY_BROWN);
		 delay_ms(50000);
		 st_PutStr5x7(1, 5, 10, "NATANAEL", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 5, 20, "MOJICA", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 5, 30, "JIMENEZ", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 60, 40, "LAB", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 20, 50, "MICROCONTROLADORES", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 5, 60, "<------------------>", COLOR565_RED, COLOR565_LIME);
	         st_PutStr5x7(1, 2, 75, "##$$?¡[]{}°!ñññÑÑ", COLOR565_RED, COLOR565_LIME);
	         st_PutStr5x7(1, 2, 85, "123456789", COLOR565_RED, COLOR565_LIME);
		 st_PutStr5x7(1, 2, 95, "~~~~~~~~~~~~", COLOR565_RED, COLOR565_LIME);	 
		 delay_ms(10000);
		 lcd_Clear(COLOR565_BLACK);  
		 drawBitmap(0, 0,linux_bits, 128, 160, COLOR565_GRAY, COLOR565_BLACK);
		 delay_ms(100000);*/
		 lcd_Clear(COLOR565_BLACK);
	         uint16_t i,j;
		 for(j=0;j<20480;j++){
			push_color(imagen_tabla[j]);
		 }
		 delay_ms(50000);
		 for(j=0;j<20480;j++){
			push_color(imagen2_tabla[j]);
		 }
		 delay_ms(50000);
		 for(j=20480;j>0;j--){
			push_color(imagen2_tabla[j]);
		 }
		 delay_ms(50000);
		 for(j=0;j<20480;j++){
			push_color(imagen_tabla[j]);
		 }
		 delay_ms(50000);
		usbd_poll(usbd_dev);
		 for(j=0;j<20480;j++){
			push_color(RGB565(getBlue(imagen_tabla[j]), getGreen(imagen_tabla[j]), getRed(imagen_tabla[j])));
		 }
		 for(j=0;j<20480;j++){
			push_color(RGB565(getGreen(imagen_tabla[j]), getBlue(imagen_tabla[j]), getRed(imagen_tabla[j])));
		 }
		 for(j=0;j<20480;j++){
			push_color(RGB565(getGreen(imagen_tabla[j]), getRed(imagen_tabla[j]), getBlue(imagen_tabla[j])));
		 }
		 delay_ms(50000);
		gpio_toggle(GPIOD,GPIO14);
		usbd_poll(usbd_dev);
	}
	return 0;
}
