/* *
 *
 * Made by Pablo Biagioli
 *
 * */
#ifndef _MAIN_H_
#define _MAIN_H_

/** \addtogroup blink Bare-metal blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "inttypes.h"
#include "chip.h"

#include "btn_dto.h"
#include "serial_dto.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/* init hardware functions */
void initHardware(void);
void init_hw_PWM(void);
void init_hw_UART(void);

/* Clock and UART Handlers */
void SysTick_Handler(void);
void UART2_IRQHandler(void);

/* Pause function in ms */
void pausems(uint32_t t);
/* Mandar el valor de current_duty1 hexadecimal al num_boton */
uint8_t boton_salida(uint8_t current_duty1, uint8_t num_boton);

/** @brief main function
 * @return main function should never return
 */
int main(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
