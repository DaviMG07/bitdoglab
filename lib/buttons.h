#ifndef BUTTONS_H
#define BUTTONS_h 

#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_J 22
#define DEBOUCE_DELAY 200000

#include "pico/stdlib.h"

typedef struct {
    bool pressed;
    uint32_t last_time;
    uint pin;
} Button;

Button *create_button(uint pin);

#endif 