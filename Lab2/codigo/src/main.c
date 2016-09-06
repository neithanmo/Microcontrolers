
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencmsis/core_cm3.h>

uint16_t frequency_sequence[16] = {
	15000,// primeros 10 segundos del ciclo de espera, en los cuales se acepta solicitud
	750,//toggle del led3 indicando que se apagara y dara paso a los peatones
	750,
	750,
	750,
	750,
	750,
	1500,
	15000,//peatones tienen 10 segundos para pasar
	750,//toggle del led4 indicando que se apagara y dara paso a los vehiculos
	750,
	750,
	750,
	750,
	750,
	1500,
};

uint16_t frequency_sel = 0;

uint16_t compare_time;
uint16_t new_time;
uint16_t frequency;
bool solicitud;
bool fin_ciclo; 

static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_120MHZ]);

}

static void gpio_setup(void)
{

	/* Enable GPIO clock for leds. */
	rcc_periph_clock_enable(RCC_GPIOD);

	/* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
		      GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);

   	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); 

}

static void tim_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral. */
	timer_reset(TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		       TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value.
	 * Running the clock at 5kHz.
	 */
	/*
	 * On STM32F4 the timers are not running directly from pure APB1 or
	 * APB2 clock busses.  The APB1 and APB2 clocks used for timers might
	 * be the double of the APB1 and APB2 clocks.  This depends on the
	 * setting in DCKCFGR register. By default the behaviour is the
	 * following: If the Prescaler APBx is greater than 1 the derived timer
	 * APBx clocks will be double of the original APBx frequencies. Only if
	 * the APBx prescaler is set to 1 the derived timer APBx will equal the
	 * original APBx frequencies.
	 *
	 * In our case here the APB1 is devided by 4 system frequency and APB2
	 * divided by 2. This means APB1 timer will be 2 x APB1 and APB2 will
	 * be 2 x APB2. So when we try to calculate the prescaler value we have
	 * to use rcc_apb1_freqency * 2!!! 
	 *
	 * For additional information see reference manual for the stm32f4
	 * familiy of chips. Page 204 and 213
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 10));

	/* Enable preload. */
	timer_disable_preload(TIM2);

	/* Continous mode. */
	timer_continuous_mode(TIM2);

	/* Period (36kHz). */
	timer_set_period(TIM2, 65535);

	/* Disable outputs. */
	timer_disable_oc_output(TIM2, TIM_OC1);
	timer_disable_oc_output(TIM2, TIM_OC2);
	timer_disable_oc_output(TIM2, TIM_OC3);
	timer_disable_oc_output(TIM2, TIM_OC4);

	/* -- OC1 configuration -- */

	/* Configure global mode of line 1. */
	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);
	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_FROZEN);

	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC1, frequency_sequence[0]);

	/* ---- */

	/* ARR reload enable. */
	timer_disable_preload(TIM2);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable commutation interrupt. */
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);
		/* activación de las luces del semaforo*/
		if(solicitud){
			fin_ciclo = false; //rechazamos solicitudes de paso
			if(frequency_sel == 0){	
				gpio_set(GPIOD, GPIO12 | GPIO15); //paso Vehiculos/alto Peatones
				gpio_clear(GPIOD, GPIO13 | GPIO14);
			}
			else if((frequency_sel >= 1)&&(frequency_sel < 6)){
				gpio_toggle(GPIOD, GPIO12);//alerta vehiculos/cambio a roja

			}
			else if(frequency_sel == 6){
				gpio_clear(GPIOD, GPIO12);
				gpio_set(GPIOD, GPIO14);//alto vehiculos
			}
			else if(frequency_sel == 7){
				gpio_set(GPIOD, GPIO13);//luz verde para peatones
				gpio_clear(GPIOD, GPIO12 | GPIO15);
			}
			else if(frequency_sel == 8){
				gpio_set(GPIOD, GPIO13);
			}
			else if((frequency_sel >= 9)&&(frequency_sel < 14)){
				gpio_toggle(GPIOD, GPIO13);//alerta peatones/cambio a roja
			}
			else if(frequency_sel == 14){
				gpio_clear(GPIOD, GPIO13);//espera de un segundo antes de dar
				gpio_set(GPIOD, GPIO15);//luz verde a vehiculos
			}
			else(frequency_sel == 15){
				gpio_set(GPIOD, GPIO12 | GPIO15);//paso a vehiculos
				gpio_clear(GPIOD, GPIO14 | GPIO13);
				fin_ciclo = true;
				solicitud = false;
			}
		}
		else{
			gpio_set(GPIOD, GPIO12 | GPIO15);//paso a vehiculos
			gpio_clear(GPIOD, GPIO14 | GPIO13);
		}
		/*
		 * Get current timer value to calculate next
		 * compare register value.
			*/
		compare_time = timer_get_counter(TIM2);

		/* Calculate and set the next compare value. */
		frequency = frequency_sequence[frequency_sel++];
		new_time = compare_time + frequency;
		timer_set_oc_value(TIM2, TIM_OC1, new_time);
		if(frequency_sel == 16){
			frequency_sel = 0;
		}
	}
}

int main(void)
{	
	clock_setup();
	gpio_setup();
	tim_setup();
	solicitud = false;
	fin_ciclo = true;

	/* Loop calling Wait For Interrupt. In older pre cortex ARM this is
	 * just equivalent to nop. On cortex it puts the cpu to sleep until
	 * one of the three occurs:
	 *
	 * a non-masked interrupt occurs and is taken
	 * an interrupt masked by PRIMASK becomes pending
	 * a Debug Entry request
	 */
	while (1){
		if(gpio_get(GPIOA, GPIO0) && fin_ciclo){
			solicitud = true;
			frequency_sel = 0;
		}
		//__WFI(); /* Wait For Interrupt. */
	}
	return 0;
}
