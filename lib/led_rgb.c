#include "led_rgb.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

void init_led(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num, &config, true);
}
void set_led_color(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set_gpio_level(RED, r * r);
    pwm_set_gpio_level(GREEN, g * g);
    pwm_set_gpio_level(BLUE, b * b);
}