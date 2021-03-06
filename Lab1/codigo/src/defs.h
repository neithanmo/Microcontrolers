/** @file defs.h
 *  @brief General definitions and macros.
 *
 *  @details This file contains the definition of general macros for
 *  STM32F4xx programs.
 */

#ifndef UC_DEFS_HPP
#define UC_DEFS_HPP

#ifdef STM32F4
#define LED_PORTD GPIOD
#define LED_PIN0 GPIO0
#define LED_PIN1 GPIO1
#define LED_PIN2 GPIO2
#define LED_PIN3 GPIO3
#define LED_PIN4 GPIO4
#define LED_PIN5 GPIO5
#define LED_PIN6 GPIO6
#define LED_RCC RCC_GPIOD
#endif

//PWM stuff
#define SYSFREQ                              168000000 //168MHz
#define PWMFREQ                                  2000//58000 //32000
#define PWMFREQ_F                  ((float )(PWMFREQ)) //32000.0f
#define PWMFREQ_CENTER_ALIGNED_F   (PWMFREQ_F/2.0f)     

#define PRESCALE                                                   1 //freq_CK_CNT=freq_CK_PSC/(PSC[15:0]+1)
#define PWM_PERIOD_ARR               SYSFREQ/( PWMFREQ*(PRESCALE+1))
#define INIT_DUTY 

#endif
