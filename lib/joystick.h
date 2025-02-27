#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "pico/stdlib.h"

#define JOY_X 26
#define JOY_Y 27

typedef struct {
    uint16_t x;
    uint16_t y;
} Joystick;

void init_joystick(uint pin_x, uint pin_y);
void update_joy(Joystick *joy);

#endif