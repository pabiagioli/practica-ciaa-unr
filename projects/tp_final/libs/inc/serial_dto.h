//#define _GNU_SOURCE
#include <stdint.h>
//#include <stdio.h> //printf, asprintf

typedef enum input_states {INVALID_MSG, IDLE_MSG, START_MSG, MSG_DIGIT1, MSG_DIGIT2, END_MSG} input_states;

typedef struct {
  input_states state;
  char data[3];
} InputDTO;

uint8_t string_to_8bit_hex(char *str);
uint16_t interpolar_num(uint8_t hexValue);
uint16_t interpolar_string(char *data);
uint16_t funcion_interpolacion_lineal(uint16_t x, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void transicionar_dto (InputDTO *dto, uint8_t input);
