#include <utils.h>


void _delay_ms(const uint32_t delay)
{
    uint32_t i, j;

    for( i = 0; i < delay; i++ )
        for( j = 0; j < 1000; j++)
            __asm__("nop");
}
