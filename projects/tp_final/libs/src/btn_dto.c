#include "btn_dto.h"

btn_dto botones[] = {
		{ /*.state = BTN_OFF, */ .funcion_btn = funcion_btn1 },
		{ /*.state = BTN_OFF, */ .funcion_btn = funcion_btn2 },
		{ /*.state = BTN_OFF, */ .funcion_btn = funcion_btn3 },
		{ /*.state = BTN_OFF, */ .funcion_btn = funcion_btn4 }
};

uint8_t funcion_btn1 (uint8_t current_duty) {
  return 0x00;
}

uint8_t funcion_btn2 (uint8_t current_duty) {
  return 0xFF;
}

uint8_t funcion_btn3 (uint8_t current_duty) {
  return current_duty + 0x10;
}

uint8_t funcion_btn4 (uint8_t current_duty) {
  return current_duty - 0x10;
}

/*
int main(void){
  uint32_t current_duty = 0;
  int i = 0;
  for(i = 0; i < 4; i++){
    printf("%d\n", current_duty = botones[i].funcion_btn(current_duty));
  }
}
*/
