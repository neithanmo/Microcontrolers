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
#include <math.h>
#include "usb.h"
#include "st7735.h"
#include "tabla.h"
#include "tabla2.h"



#define PWM_PERIOD 2999
#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12
//usbd_ep_write_packet(usbd_dev, 0x82, buf, len)
uint16_t image_buffer[128*160];
bool new_image;
usbd_device *usbd_dev;

uint16_t frequency_sequence[1] = {
	1
};

uint8_t frequency_sel;
uint16_t compare_time;
uint16_t new_time;
uint16_t frequency;
uint16_t buff[1];//es necesario declarar un arreglo debido al cont void * ptr
uint16_t count;


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
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPCEN);
        gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN | GPIO15 | GPIO12 | GPIO13 | GPIO14 | GPIO3 | GPIO4 | GPIO5 | GPIO1);
	gpio_clear(LED_DISCO_GREEN_PORT, GPIO14);

	/* D/C =  conectado al pin 3 del puerto c
          D/C = 1; data
          D/C = 0; commando de control
	*/
}

static void usb_setup(void)
{

        rcc_periph_clock_enable(RCC_OTGFS);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);
}


static void tim_setup(void)
{
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	timer_reset(TIM2);

	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		       TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
        /* 12MHz frecuencia del reloj
         * esto asegura junto al periodo
         * una buena tasa de bits en la
         * transmision de datos usb*/
        timer_set_prescaler(TIM2, 1);
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

        timer_set_period(TIM2, 65536);
	timer_disable_oc_output(TIM2, TIM_OC1);
	timer_disable_oc_output(TIM2, TIM_OC2);
	timer_disable_oc_output(TIM2, TIM_OC3);
	timer_disable_oc_output(TIM2, TIM_OC4);

	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);
	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_FROZEN);
	timer_set_oc_value(TIM2, TIM_OC1, frequency_sequence[0]);
        timer_enable_counter(TIM2);
}
void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
		timer_clear_flag(TIM2, TIM_SR_CC1IF);
		compare_time = timer_get_counter(TIM2);
		frequency = frequency_sequence[frequency_sel++];
		new_time = compare_time + frequency;
		timer_set_oc_value(TIM2, TIM_OC1, new_time);

		if(frequency_sel == 1){
			frequency_sel = 0;				
		}
	}
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
	//int len = usbd_ep_read_packet(usbd_read, 0x01, image_buffer, 40960);
	int len = usbd_ep_read_packet(usbd_read, 0x01, buff, sizeof(uint16_t));
	gpio_toggle(GPIOD,GPIO14);
	if(count==20480) count=0;
	image_buffer[count]=buff[0];
	//push_color(image_buffer[count]);
	count=count+1;
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
	spi_setup(SPI1);
	gpio_setup();
        usb_setup();
	lcd_backLight(1);
	init_lcd();
	new_image = false;

	usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
			usb_strings, 3,
			usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);
	/*uint8_t dx,dy;
	dx = 0;
        dy = 0;*/
	uint16_t j,i;
        lcd_setAddrWindow(0,128,0,159);
	while (1) {
		if(gpio_get(GPIOA, GPIO0)){
			count=0;
			gpio_toggle(GPIOD, GPIO12);
        		/*lcd_setAddrWindow(0,128,0,159);
			push_color(buff[0]);
			new_image=false;*/
		}
		if(count==20480){
			count=0;
			gpio_toggle(GPIOD, GPIO15);
			for(j=0;j<160;j++){
				for(i=0;i<128;i++){
				lcd_Pixel(i,j,image_buffer[i+(j*128)]);
				usbd_poll(usbd_dev);
				}
			}
		}
		usbd_poll(usbd_dev);
		//push_color(image_buffer[0]);		
	}
	return 0;
}
