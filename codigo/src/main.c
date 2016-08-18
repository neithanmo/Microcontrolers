#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "setup.h"
#include "utils.h"
#include "defs.h"


int main(void)
{

	/* Init the hardware components */
	leds_init();

    /* Flash the LED forever */
    while(1)
    {
        gpio_set(LED_PORT, LED_PIN);
        _delay_ms(500);
        gpio_clear(LED_PORT, LED_PIN);
        _delay_ms(500);
    }
    
    return 0;
}
