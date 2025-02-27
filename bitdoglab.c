/**
 * @file main.c
 * @brief Sistema de controle para processo de plantio, irrigação e colheita.
 *
 * Este programa configura e gerencia os seguintes componentes:
 * - Botões com debounce.
 * - Joystick.
 * - LEDs RGB.
 * - Matriz de LEDs.
 * - Display SSD1306 via I2C.
 *
 * O fluxo do sistema é controlado por uma máquina de estados:
 *   POUSIO    - Estado inicial com exibição da grama.
 *   PLANTIO   - Simulação do plantio com mudança de cor.
 *   IRRIGACAO - Exibição dos modos de irrigação e atualização do display.
 *   COLHEITA  - Simulação da colheita e retorno ao estado inicial.
 */

 #include "lib/includes.h"
 #include "pico/time.h"
 #include "lib/buttons.h"
 #include "lib/joystick.h"
 #include "lib/led_rgb.h"
 #include "lib/matrix.h"
 #include "lib/ssd1306.h"
 
 /* ============================================
    Macros e Constantes de Configuração
    ============================================ */
 #define I2C_PORT       i2c1     /**< Porta I2C para o display */
 #define DISPLAY_SDA    14       /**< Pino SDA para o display */
 #define DISPLAY_SCL    15       /**< Pino SCL para o display */
 #define ADRESS         0x3C     /**< Endereço do display SSD1306 */
 
 /* ============================================
    Declaração de Botões
    ============================================ */
 volatile Button* button_a;   /**< Botão A */
 volatile Button* button_b;   /**< Botão B */
 volatile Button* button_j;   /**< Botão do joystick */
 
 /* ============================================
    Configuração da Matriz de LEDs
    ============================================ */
 Matrix matrix = {
     .buffer = {0},
     .pio = pio0,
     .sm = 0,
     .reference = {
         {24, 23, 22, 21, 20},
         {15, 16, 17, 18, 19},
         {14, 13, 12, 11, 10},
         {5, 6, 7, 8, 9},
         {4, 3, 2, 1, 0}
     }
 };
 
 /* ============================================
    Objeto para o Display SSD1306
    ============================================ */
 ssd1306_t ssd;
 
 /* ============================================
    Máquina de Estados para o Processo de Plantio
    ============================================ */
 enum mefplant {
     POUSIO,       /**< Estado de repouso: exibe grama inicial */
     PLANTIO,      /**< Estado de plantio: simula o crescimento */
     IRRIGACAO,    /**< Estado de irrigação: exibe modo de irrigação */
     COLHEITA      /**< Estado de colheita: simula a colheita */
 } state = POUSIO;
 
 /* ============================================
    Modos de Irrigação
    ============================================ */
 enum irrig {
     NENHUM,       /**< Sem irrigação */
     ASPERSAO,     /**< Irrigação por aspersão */
     GOTEJAMENTO   /**< Irrigação por gotejamento */
 } irrigacao = NENHUM;
 
 /* ============================================
    Variáveis Globais de Umidade
    ============================================ */
 uint8_t umidade_solo = 0; /**< Nível de umidade do solo */
 uint8_t umidade_ar = 0;   /**< Nível de umidade do ar */
 
 /* ============================================
    Função: buttons_handler
    ============================================ */
 /**
  * @brief Handler de interrupção para os botões (com debounce).
  *
  * Esta função é chamada quando ocorre uma interrupção nos pinos dos botões.
  * Atualiza os estados dos botões e ajusta os níveis de umidade de solo e ar.
  *
  * @param gpio Pino que gerou a interrupção.
  * @param events Eventos associados à interrupção.
  */
 void buttons_handler(uint gpio, uint32_t events) {
     uint32_t now = to_us_since_boot(get_absolute_time());
 
     // Botão A: Incrementa o nível de umidade do solo
     if (gpio == button_a->pin) {
         if (button_a->last_time > 200000) {  // Verifica debounce
             umidade_solo = (umidade_solo + 1) % 3;
             printf("umidade do solo: %d\n", umidade_solo);
             button_a->pressed = true;
             button_a->last_time = now;
         }
     }
 
     // Botão B: Incrementa o nível de umidade do ar
     if (gpio == button_b->pin) {
         if (button_b->last_time > 200000) {
             umidade_ar = (umidade_ar + 1) % 3;
             printf("umidade do ar: %d\n", umidade_ar);
             button_b->pressed = true;
             button_b->last_time = now;
         }
     }
 
     // Botão do Joystick: Apenas registra o pressionamento
     if (gpio == button_j->pin) {
         if (button_j->last_time > 200000) {
             button_j->pressed = true;
             button_j->last_time = now;
         }
     }
 }
 
 /* ============================================
    Função: setup
    ============================================ */
 /**
  * @brief Inicializa o sistema e os periféricos.
  *
  * Esta função configura:
  * - Joystick e I/O padrão.
  * - Botões com interrupção.
  * - LEDs RGB.
  * - Matriz de LEDs.
  * - Interface I2C e display SSD1306.
  */
 void setup() {
     // Inicializa joystick e I/O padrão
     init_joystick(JOY_X, JOY_Y);
     stdio_init_all();
 
     // Cria os botões
     button_a = create_button(BUTTON_A);
     button_b = create_button(BUTTON_B);
     button_j = create_button(BUTTON_J);
 
     // Configura as interrupções para os botões
     gpio_set_irq_enabled_with_callback(button_a->pin, GPIO_IRQ_EDGE_FALL, true, &buttons_handler);
     gpio_set_irq_enabled(button_b->pin, GPIO_IRQ_EDGE_FALL, true);
     gpio_set_irq_enabled(button_j->pin, GPIO_IRQ_EDGE_FALL, true);
 
     // Inicializa os LEDs RGB
     init_led(RED);
     init_led(GREEN);
     init_led(BLUE);
 
     // Inicializa a matriz de LEDs
     init_matrix(&matrix);
 
     // Configura a interface I2C para o display
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(DISPLAY_SDA, GPIO_FUNC_I2C);
     gpio_set_function(DISPLAY_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(DISPLAY_SDA);
     gpio_pull_up(DISPLAY_SCL);
 
     // Inicializa e configura o display SSD1306
     ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
     ssd1306_config(&ssd);
     ssd1306_send_data(&ssd);
     ssd1306_fill(&ssd, false);
     ssd1306_send_data(&ssd);
 
     // Define a cor inicial do LED (verde)
     set_led_color(0, 255, 0);
 }
 
 /* ============================================
    Função: main
    ============================================ */
 /**
  * @brief Função principal do programa.
  *
  * Executa o loop principal, onde é gerenciada a máquina de estados que
  * simula o processo de plantio, irrigação e colheita, atualizando a matriz
  * de LEDs e o display conforme necessário.
  *
  * @return int Retorna 0 (nunca alcançado, pois o loop é infinito).
  */
 int main() {
     setup();
 
     // Criação do frame inicial ("grama")
     Frame grama = {
         .color = urgb_u32(0, 3, 0),
         .draw = {31, 31, 31, 31, 31}
     };
 
     // Loop principal do sistema
     while (true) {
         // Máquina de estados para o processo de plantio
         switch (state) {
             case POUSIO:
                 // Estado POUSIO: exibe grama de cor verde e aguarda
                 grama.color = urgb_u32(0, 3, 0);
                 sleep_ms(3000);
                 state = PLANTIO;
                 break;
 
             case PLANTIO:
                 // Estado PLANTIO: simula o crescimento da grama
                 grama.color = urgb_u32(0, 7, 0);
                 sleep_ms(2000);
                 break;
 
             case IRRIGACAO: {
                 // Estado IRRIGACAO: simula a irrigação
                 grama.color = urgb_u32(0, 3, 7);
                 // Seleciona o modo de irrigação com base nos níveis de umidade
                 if (umidade_solo < 1) {
                     irrigacao = (umidade_ar < 2) ? ASPERSAO : GOTEJAMENTO;
                 }
                 // Define a mensagem a ser exibida no display
                 const char* irrigMsg = (irrigacao == NENHUM)   ? "Modo de Irrigacao: Nenhum" :
                                          (irrigacao == ASPERSAO) ? "Modo de Irrigacao: Aspersao" :
                                                                  "Modo de Irrigacao: Gotejamento";
                 ssd1306_draw_string(&ssd, irrigMsg, 0, 0);
                 ssd1306_send_data(&ssd);
                 sleep_ms(2000);
                 state = COLHEITA;
                 irrigacao = NENHUM;
                 break;
             }
 
             case COLHEITA:
                 // Estado COLHEITA: simula a colheita e retorna ao estado inicial
                 grama.color = urgb_u32(0, 15, 0);
                 sleep_ms(2000);
                 state = POUSIO;
                 break;
         }
 
         // Atualiza o display e a matriz de LEDs:
         // Limpa o display, desenha o frame atual e renderiza a matriz.
         ssd1306_fill(&ssd, false);
         put_frame(grama, &matrix);
         render(&matrix);
     }
     return 0;
 }
 