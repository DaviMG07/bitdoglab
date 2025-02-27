/**
 * @file matrix_functions.c
 * @brief Funções para manipulação da matriz de LEDs.
 *
 * Este arquivo contém funções para inicializar a matriz de LEDs, enviar pixels,
 * converter componentes de cor e renderizar frames na matriz.
 */

 #include "pico/stdlib.h"
 #include <stdlib.h>
 #include "matrix.h"
 #include "ws2812.pio.h"
 
 /**
  * @brief Inicializa a matriz de LEDs.
  *
  * Adiciona o programa WS2812 ao PIO e inicializa a máquina de estados
  * para controle dos LEDs na matriz.
  *
  * @param matrix Ponteiro para a estrutura Matrix a ser inicializada.
  */
 void init_matrix(Matrix *matrix) {
     uint offset = pio_add_program(matrix->pio, &ws2812_program);
     ws2812_program_init(matrix->pio, matrix->sm, offset, MATRIX_PIN, 800000, IS_RGBW);
 }
 
 /**
  * @brief Envia um pixel para a matriz de LEDs.
  *
  * Transfere o valor da cor para o estado da máquina do PIO, deslocando
  * a cor 8 bits à esquerda conforme exigido pelo protocolo WS2812.
  *
  * @param matrix Ponteiro para a estrutura Matrix.
  * @param color Valor da cor em formato de 32 bits.
  */
 void put_pixel(Matrix *matrix, uint32_t color) {
     pio_sm_put_blocking(matrix->pio, matrix->sm, color << 8u);
 }
 
 /**
  * @brief Converte componentes RGB em um valor de cor de 32 bits.
  *
  * Combina os valores de vermelho, verde e azul em um único valor, 
  * posicionado nos bytes correspondentes.
  *
  * @param r Componente de vermelho.
  * @param g Componente de verde.
  * @param b Componente de azul.
  * @return uint32_t Valor da cor combinada.
  */
 uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
     return ((uint32_t)r << 8) | ((uint32_t)g << 16) | (uint32_t)b;
 }
 
 /**
  * @brief Converte um número inteiro em um array de valores booleanos.
  *
  * Extrai os 5 bits menos significativos de um número e os armazena em um
  * array de 5 booleanos. Cada posição do array indica se o respectivo bit
  * está ativo (true) ou não (false).
  *
  * @param num Número inteiro a ser convertido.
  * @return bool* Ponteiro para o array de booleanos alocado dinamicamente.
  *               É responsabilidade do chamador liberar a memória alocada.
  */
 bool *int_to_array(uint8_t num) {
     bool *array = malloc(5 * sizeof(bool));
     for (int i = 0; i < 5; ++i) {
         array[i] = num & (1 << i);
     }
     return array;
 }
 
 /**
  * @brief Renderiza o conteúdo do buffer da matriz de LEDs.
  *
  * Percorre o buffer da matriz e envia cada valor de cor para a função put_pixel,
  * atualizando a exibição dos LEDs.
  *
  * @param matrix Ponteiro para a estrutura Matrix.
  */
 void render(Matrix *matrix) {
     for (uint i = 0; i < PIXELS; ++i) {
         put_pixel(matrix, matrix->buffer[i]);
     }
 }
 
 /**
  * @brief Atualiza o buffer da matriz com os dados de um frame.
  *
  * Para cada linha do frame, converte o valor inteiro em um array de bits e
  * atualiza o buffer da matriz de acordo com a posição referenciada.
  *
  * @param frame Estrutura Frame contendo a cor e os dados do desenho.
  * @param matrix Ponteiro para a estrutura Matrix.
  */
 void put_frame(Frame frame, Matrix *matrix) {
     for (uint row = 0; row < 5; ++row) {
         bool *row_array = int_to_array(frame.draw[row]);
         for (uint col = 0; col < 5; ++col) {
             // Atualiza o pixel somente se o bit correspondente estiver ativo
             matrix->buffer[matrix->reference[row][col]] = (row_array[col])
                 ? frame.color
                 : matrix->buffer[matrix->reference[row][col]];
         }
         // É aconselhável liberar a memória alocada para row_array após seu uso
         free(row_array);
     }
 }
 