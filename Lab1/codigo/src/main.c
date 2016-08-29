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
  int max = 6;
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
	   	gpio_port_write(GPIOD,0x01);
	   	break;
        
	   case 2:		//El numero es 2, 2 leds encendidos
	   	gpio_port_write(GPIOD,0x42);
	   	break;
        
	   case 3:		//El numero es 3, 3 leds encendidos
	   	gpio_port_write(GPIOD,0x43);
	   	break;
        
	   case 4:		//El numero es 4, 4 leds encendidos
	   	gpio_port_write(GPIOD,0x78);
	   	break;
	
	   case 5:		//El numero es 5, 5 leds encendidos
	   	gpio_port_write(GPIOD,0x5A);
	   	break;
	   
	   case 6:		//El numero es 6, 6 leds encendidos
	   	gpio_port_write(GPIOD,0x7E);
	   	break;
	   
	   default:
	   break;			 
        }
     }
  }
  return 0;
}
