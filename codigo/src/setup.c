#include "setup.h"

void leds_init(void)
{

    /* Clock the GPIOC peripheral */
    rcc_periph_clock_enable(LED_RCC);

    /* Configure the pin as an output */
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);	
	
}
