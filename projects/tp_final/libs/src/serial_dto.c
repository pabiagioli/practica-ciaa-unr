#include "serial_dto.h"

/* utility function to convert hex character representation to their nibble (4 bit) values */
static uint8_t nibbleFromChar(uint8_t c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 255;
}

uint8_t string_to_8bit_hex(char *str){
	return (nibbleFromChar(str[0]) * 16) + nibbleFromChar(str[1]);
}

uint16_t interpolar_num(uint8_t hexValue){
	if(hexValue >= 0 && hexValue < 0x40)
	    return funcion_interpolacion_lineal(hexValue, 0x0, 0x0, 0x40,0x40);
	  else if(hexValue >= 0x40 && hexValue < 0x80)
	    return funcion_interpolacion_lineal(hexValue, 0x40,0x40, 0x80,0xFF);
	  else if(hexValue >= 0x80 && hexValue < 0xC0)
	    return funcion_interpolacion_lineal(hexValue, 0x80, 0xFF, 0xC0,0x40);
	  else if(hexValue >= 0xC0 && hexValue <= 0xFF)
	    return funcion_interpolacion_lineal(hexValue, 0xC0, 0x40, 0xFF,0xFF);
	  return 0;
}

uint16_t interpolar_string(char *data) {
  uint8_t hexValue = 0x0;
  if(*data != 0x0)
	hexValue = (nibbleFromChar(data[0]) * 16) + nibbleFromChar(data[1]);
 // printf("hexValue = %x\n", hexValue);
  return interpolar_num(hexValue);
}


uint16_t funcion_interpolacion_lineal (uint16_t x, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  return ((x - x1) * (y2 - y1) / (x2 - x1)) + y1;
}

void transicionar_dto (InputDTO *dto, uint8_t input){
  if(dto->state == INVALID_MSG && input == ':'){
    dto->state = START_MSG;
    //dto->data = "";
  } else if(dto->state == IDLE_MSG && input == ':'){
    dto->state = START_MSG;
    //dto->data = "";
  } else if (dto->state == IDLE_MSG && input != ':') {
    dto->state = INVALID_MSG;
  } else if (dto->state == START_MSG && ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F') )) {
    dto->state = MSG_DIGIT1;
    dto->data[0] = input;
    //asprintf((dto->data), "%s%s", dto->data, &input);
  } else if (dto->state == START_MSG && !((input >= '0' && input <= '9') || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F') )) {
    dto->state = INVALID_MSG;
  } else if (dto->state == MSG_DIGIT1 && ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F') )) {
    dto->state = MSG_DIGIT2;
    dto->data[1] = input;
    //asprintf((dto->data), "%s%s", dto->data, &input);
  } else if (dto->state == MSG_DIGIT1 && !((input >= '0' && input <= '9') || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F') )) {
    dto->state = INVALID_MSG;
  } else if (dto->state == MSG_DIGIT2 && (input == 0x0D || input == '\n')) {
    //0x0D is CR and '\n' is CRLF or LF depending on the platform
    dto->state = END_MSG;
  } else if (dto->state == MSG_DIGIT2 && !(input == 0x0D || input == '\n')) {
    //0x0D is CR and '\n' is CRLF or LF depending on the platform
    dto->state = INVALID_MSG;
  }
}


/*int main(void) {
  InputDTO maquina = {.state=IDLE, .data=":00"};
  transicionar_dto(&maquina, 'a');
  transicionar_dto(&maquina, ':');
  transicionar_dto(&maquina, '8');
  transicionar_dto(&maquina, 'a');
  transicionar_dto(&maquina, '\n');
  if(maquina.state == END_MSG)
    printf("maquina.data interpolada = %x\n", interpolar(maquina.data));
  printf("datos de la maquina: state = %d , data= %s\n", maquina.state, maquina.data);
}*/
