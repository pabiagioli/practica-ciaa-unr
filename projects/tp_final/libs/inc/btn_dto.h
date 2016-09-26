#include <stdint.h>

/*typedef enum btn_state {
	BTN_ON, BTN_OFF
} btn_state;
*/
typedef struct btn_dto {
	//btn_state state;
	uint8_t (*funcion_btn)(uint8_t current_duty);
} btn_dto;

uint8_t funcion_btn1(uint8_t current_duty);
uint8_t funcion_btn2(uint8_t current_duty);
uint8_t funcion_btn3(uint8_t current_duty);
uint8_t funcion_btn4(uint8_t current_duty);

extern btn_dto botones[];
