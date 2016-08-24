#include "setup.h"

void leds_init(void)
{

    /// habilitamos el reloj para alimentar los perifericos conectados a puerto
    rcc_periph_clock_enable(LED_RCC);

    /// aqui inicializo el puerto y le indico al controlador como tratar los pines de leds
    
    gpio_mode_setup(LED_PORTA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 
			LED_PIN0 | LED_PIN1 | LED_PIN2 | LED_PIN3 | LED_PIN4 | LED_PIN5 | LED_PIN6 | LED_PIN7);
    gpio_mode_setup(LED_PORTD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, 
			LED_PIN0 | LED_PIN1 | LED_PIN2 | LED_PIN3 | LED_PIN4 | LED_PIN5 | LED_PIN6 | LED_PIN7);

    ///ahora onfiguramos el boton "user" y se habilita
   rcc_periph_clock_enable(RCC_GPIOA);
   gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);	
	
}
