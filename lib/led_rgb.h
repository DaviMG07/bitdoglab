#ifndef LED_RGB_H
#define LED_RGB_H

#include "pico/stdlib.h"

#define RED 13
#define GREEN 11
#define BLUE 12

void init_led(uint pin);
void set_led_color(uint8_t r, uint8_t g, uint8_t b);

#endif