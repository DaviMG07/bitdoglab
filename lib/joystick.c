/**
 * @file joystick.c
 * @brief Implementação das funções para inicialização e atualização do joystick.
 *
 * Este arquivo contém funções que configuram os pinos ADC para a leitura dos eixos
 * X e Y do joystick, utilizando o ADC do hardware.
 */

 #include "joystick.h"
 #include "hardware/adc.h"
 
 /**
  * @brief Inicializa o joystick.
  *
  * Configura os pinos ADC para os eixos X e Y do joystick.
  *
  * @param pin_x Pino ADC destinado à leitura do eixo X.
  * @param pin_y Pino ADC destinado à leitura do eixo Y.
  */
 void init_joystick(uint pin_x, uint pin_y) {
     adc_init();              // Inicializa o ADC
     adc_gpio_init(pin_x);    // Configura o pino para o eixo X
     adc_gpio_init(pin_y);    // Configura o pino para o eixo Y
 }
 
 /**
  * @brief Atualiza as leituras do joystick.
  *
  * Seleciona as entradas ADC correspondentes e atualiza os valores dos eixos
  * X e Y na estrutura Joystick.
  *
  * @param joy Ponteiro para a estrutura Joystick que armazenará os valores dos eixos.
  */
 void update_joy(Joystick *joy) {
     adc_select_input(0);     // Seleciona o canal ADC para o eixo X
     joy->x = adc_read();     // Lê e armazena o valor do eixo X
 
     adc_select_input(1);     // Seleciona o canal ADC para o eixo Y
     joy->y = adc_read();     // Lê e armazena o valor do eixo Y
 }
 