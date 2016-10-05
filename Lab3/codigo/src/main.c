#include "usb.h"
#include <stdlib.h>
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

#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12
//usbd_ep_write_packet(usbd_dev, 0x82, buf, len)

uint16_t frequency_sequence[1] = {
	1
};

uint16_t frequency_sel;

uint16_t compare_time;
uint16_t new_time;
uint16_t frequency;
uint8_t frecuency_sel;
uint16_t buff[1];//es necesario declarar un arrglo debido al cont void * ptr
uint8_t channel_array[16];
uint32_t prueba;
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
    	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
   	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
        gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN | GPIO15 | GPIO13 | GPIO14);
}

static void adc_setup(void)
{
	//habilitar el reloj en el periferico
	rcc_periph_clock_enable(RCC_ADC1);

	//configurar el convertidor A/D
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	channel_array[0] = 0;
	//adc_set_regular_sequence(adc, length, channel[])
	adc_set_regular_sequence(ADC1, 1, channel_array);
        adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_set_right_aligned(ADC1);
	adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);//2 byte
        adc_set_dma_continue(ADC1);
        //adc_enable_eoc_interrupt(ADC1);
	adc_power_on(ADC1);
/*
ADC_SMPR_SMP_3CYC   0x0
 
ADC_SMPR_SMP_15CYC   0x1
 
ADC_SMPR_SMP_28CYC   0x2
 
ADC_SMPR_SMP_56CYC   0x3
 
ADC_SMPR_SMP_84CYC   0x4
 
ADC_SMPR_SMP_112CYC   0x5
 
ADC_SMPR_SMP_144CYC   0x6
 
ADC_SMPR_SMP_480CYC   0x7*/
}

static void usb_setup(void){

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
        timer_set_prescaler(TIM2, 10);
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
	timer_disable_preload(TIM2);
        timer_enable_counter(TIM2);

}

static void read_adc_send(void)
{
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
                buff[0] = adc_read_regular(ADC1);
        //usbd_ep_write_packet(usbd_dev, 0x82, buff, sizeof(buff));
}

void adc_isr()
{
    usbd_ep_write_packet(usbd_dev, 0x82, buff, sizeof(buff));
    gpio_toggle(LED_DISCO_GREEN_PORT, GPIO12);
}

void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		timer_clear_flag(TIM2, TIM_SR_CC1IF);
                //read_adc_send();
                usbd_ep_write_packet(usbd_dev, 0x82, buff, sizeof(buff));
                prueba++;
		//gpio_toggle(LED_DISCO_GREEN_PORT, GPIO13);
		compare_time = timer_get_counter(TIM2);
		frequency = frequency_sequence[frequency_sel++];
		new_time = compare_time + frequency;
		timer_set_oc_value(TIM2, TIM_OC1, new_time);	
		if(frequency_sel == 1){
			frequency_sel = 0;				
		}
                if(prueba == 1520){
                        prueba = 0;
                        gpio_toggle(LED_DISCO_GREEN_PORT, GPIO13);
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



static void cdcacm_set_config(usbd_device *usbd, uint16_t wValue)
{
	(void)wValue;

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
	adc_setup();
        tim_setup();
        usb_setup();
	frequency_sel = 0;
        prueba = 0;
	usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
			usb_strings, 3,
			usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);

	while (1) {
                        read_adc_send();
			usbd_poll(usbd_dev);
		}
	return 0;
}
