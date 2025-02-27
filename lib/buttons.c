/**
 * @file buttons.c
 * @brief Implementação de funções para manipulação de botões.
 *
 * Este arquivo contém a implementação da função para criar e inicializar
 * um botão, configurando o pino de GPIO correspondente como entrada com
 * pull-up ativado.
 */

 #include "buttons.h"
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/clocks.h"
 
 /**
  * @brief Cria e inicializa um botão.
  *
  * Aloca dinamicamente uma estrutura Button, inicializa o pino associado ao botão
  * como entrada, habilita o resistor pull-up interno e define o estado inicial como
  * não pressionado.
  *
  * @param pin Pino de GPIO ao qual o botão está conectado.
  * @return Button* Ponteiro para a estrutura Button alocada ou NULL em caso de falha.
  */
 Button *create_button(uint pin) {
     Button *button = malloc(sizeof(Button));
     if (!button) {
         // Em caso de falha na alocação, retorna NULL.
         return NULL;
     }
     button->pin = pin;
     button->pressed = false;
 
     // Inicializa o pino do botão como entrada com resistor pull-up ativado.
     gpio_init(pin);
     gpio_set_dir(pin, GPIO_IN);
     gpio_pull_up(pin);
     
     return button;
 }
 