#ifndef MATRIX_H
#define MATRIX_H

#include "pico/stdlib.h"
#include "ws2812.pio.h"

#define MATRIX_PIN 7
#define IS_RGBW false
#define PIXELS 25

typedef struct {
    uint8_t draw[5];
    uint32_t color;
} Frame;

typedef struct {
    PIO pio;
    uint sm;
    uint32_t buffer[PIXELS];
    uint8_t reference[5][5];
} Matrix;

void init_matrix(Matrix *matrix);
void put_pixel(Matrix *matrix, uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
bool *int_to_array(uint8_t num);
void render(Matrix *matrix);
void put_frame(Frame frame, Matrix *matrix);

#endif