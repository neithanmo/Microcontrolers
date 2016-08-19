#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <time.h>
#include <stdlib.h>
#include "setup.h"
#include "utils.h"
#include "defs.h"



int main(void)
{

/* Init the hardware components */

  leds_init();///< llamado para configurar los puertos y gpios a usar
  srand(time(NULL));
  int max = 4;
  int min = 1;

  while(1){

    if(gpio_get(GPIOA, GPIO0)){///< FUE PRESIONADO EL BOTON???
      gpio_port_write(GPIOD, 0);///<apagamos todos los leds
      _delay_ms(100);
	/*llamar aqui a la funcion para encender los leds como un 
        efecto, simulando el calculo*/
      int x = rand() % max + min; 	//Se genera un número aleatorio del 1 al 4
      
	  switch(x){
        
		case 1:		//El numero es 1, 1 led encendido
	   gpio_set(GPIOD, LED_PIN1); 
	   break;
        
		case 2:		//El numero es 2, 2 leds encendidos
	   gpio_set(GPIOD, LED_PIN1); 
	   gpio_set(GPIOD, LED_PIN2);
	   break;
        
		case 3:		//El numero es 3, 3 leds encendidos
	   gpio_set(GPIOD, LED_PIN1);
	   gpio_set(GPIOD, LED_PIN2);
	   gpio_set(GPIOD, LED_PIN3);
	   break;
        
		case 4:	//El numero es 4, 4 leds encendidos
	   gpio_set(GPIOD, LED_PIN1);
	   gpio_set(GPIOD, LED_PIN2);
	   gpio_set(GPIOD, LED_PIN3);
	   gpio_set(GPIOD, LED_PIN4);
	   break;
	
	default:
	   break;			 
      }
    }
  }
  return 0;
}
