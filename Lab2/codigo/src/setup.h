/** @file setup.h
 *  @brief Function for setting up the hardware.
 *
 *  @details This file contains the definition of functions that initialize
 *  the diferent hardware components of a STM32F4xx using libopencm3.
 */

#ifndef UC_SETUP_HPP
#define UC_SETUP_HPP

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "defs.h"
#include <stdlib.h>
/**
 * @brief Initialize the internal LEDS for a STM32F Discovery board.
 * @param void.
 * @return void.
 */
 void leds_init(void);

#endif
