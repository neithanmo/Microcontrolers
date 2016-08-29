/** @file utils.h
 *  @brief General functions
 *
 *  @details This file contains the definition of general functions for
 *  STM32F4xx programs.
 */

#ifndef UC_UTILS_HPP
#define UC_UTILS_HPP

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

/**
 * @brief Delay for a short period. This is totally uncalibrated and
 * should not be used for accurate timing.
 * @param delay Number of timing units to delay.
 * @return void.
 */
void _delay_ms(const uint32_t delay);

#endif
