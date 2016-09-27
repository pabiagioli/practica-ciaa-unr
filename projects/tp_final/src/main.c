/*==================[inclusions]=============================================*/

#include "main.h"


/*==================[macros and definitions]=================================*/
#define LPC_UART 	LPC_USART2
#define UARTx_IRQn  USART2_IRQn

/* Ring buffer size */
#define UART_RB_SIZE 256

/*==================[internal data declaration]==============================*/

/* Transmit and receive ring buffers */
static RINGBUFF_T txring, rxring;

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RB_SIZE], txbuff[UART_RB_SIZE];

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

//Constante para definir si el anti rebote pasó
const uint8_t BTN_SHOULD_CLICK = 10;
const uint8_t MSG_FTO[] = "Introduzca el valor de entrada en el formato :XY y presione Enter\r\n";

/** @brief used for delay counter */
static uint32_t pausems_count;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
/**
 * @brief	UART interrupt handler sub-routine
 * @return	Nothing
 */
void UART2_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(LPC_UART, &rxring, &txring);
}

void init_hw_UART(void){

	Board_UART_Init(LPC_UART);
	Chip_UART_Init(LPC_UART);

	Chip_UART_SetBaud(LPC_UART, 115200);
	Chip_UART_ConfigData(LPC_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT); /* Default 8-N-1 */

	/* Enable UART Transmit */
	Chip_UART_TXEnable(LPC_UART);

	/* Reset FIFOs, Enable FIFOs and DMA mode in UART */
	Chip_UART_SetupFIFOS(LPC_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0));

	/* Enable UART Rx & line status interrupts */
	/*
	 * Do not enable transmit interrupt here, since it is handled by
	 * UART_Send() function, just to reset Tx Interrupt state for the
	 * first time
	 */
	Chip_UART_IntEnable(LPC_UART, (UART_IER_RBRINT));

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_RB_SIZE);

	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(UARTx_IRQn);

}

void init_hw_PWM(void){
	Chip_SCTPWM_Init(LPC_SCT);

	Chip_SCTPWM_SetRate(LPC_SCT, 10000);

	Chip_SCU_PinMux(2,10,0,FUNC1);
	Chip_SCU_PinMux(2,11,0,FUNC1);
	Chip_SCU_PinMux(2,12,0,FUNC1);

	Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);
	Chip_SCTPWM_SetOutPin(LPC_SCT, 2, 5);
	Chip_SCTPWM_SetOutPin(LPC_SCT, 3, 4);

	Chip_SCTPWM_Start(LPC_SCT);
}

void initHardware(void)
{
		Board_Init();
		Board_Buttons_Init();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
    init_hw_UART ();
		init_hw_PWM ();
}

uint8_t boton_salida(uint8_t current_duty1, uint8_t num_boton){
	uint8_t nuevo_duty = (uint8_t) botones[num_boton].funcion_btn(current_duty1);

	//convierto el duty hexadecimal en un porcentaje
	uint8_t percentage = (nuevo_duty * (uint8_t)0x64)/((uint8_t)0xff);
	//funcion de interpolacion lineal
	uint16_t interpolado = interpolar_num(nuevo_duty);
	//convierto el duty interpolado en un porcentaje
	uint8_t percent_interpolado = ((uint8_t)interpolado * (uint8_t)0x64)/((uint8_t)0xff);

	//Entrada por LED 2
	Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, Chip_SCTPWM_PercentageToTicks(LPC_SCT, percentage));
	//Salida por LED 3
	Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, percent_interpolado));

	/* Devuelvo el valor de duty para poder guardarlo */
	return nuevo_duty;
}

void pausems(uint32_t t)
{
   pausems_count = t;
   while(pausems_count != 0) {
      __WFI();
   }
}

void SysTick_Handler(void)
{
	if(pausems_count > 0) pausems_count--;
}

int main(void)
{
	uint8_t buffer[1];
	int ret = 0;
	int len;
	volatile int antiRebAcum = 0;
	uint8_t salidaUART;
	uint8_t percentageUART;
	uint8_t hexValue;
	uint32_t BUTTON_STATUS_POLLED = 0x00;
	uint8_t current_duty = 0x00;

	InputDTO maquina = {.state=IDLE_MSG, .data={[0]='0', [1]='0', [2]='\0'}};

	initHardware();

	//Enviar mensaje con instrucciones por UART
	Chip_UART_SendRB(LPC_UART,&txring,&MSG_FTO,sizeof(MSG_FTO));

	/* Read some data from the buffer */
	while (1)
	{

		len = Chip_UART_ReadRB(LPC_UART, &rxring, &buffer, sizeof(buffer));

		Chip_UART_SendRB(LPC_UART, &txring, &buffer, len);

		//Si llegó un nuevo char desde UART, hago transicionar la maquina de estados.
		if(len > 0)
			transicionar_dto(&maquina, buffer[0]);

		//Si la maquina procesó una palabra, se hace la interpolacion lineal y se calculan los porcentajes
		if(maquina.state == END_MSG){
			hexValue = string_to_8bit_hex(maquina.data);
			percentageUART = ( hexValue * (uint8_t)0x64)/((uint8_t)0xff);
			salidaUART = (interpolar_string(maquina.data) * (uint8_t)0x64)/((uint8_t)0xff);
			//cambiar intensidad del LED 0 con la entrada
			//LED_Duty_change(maquina.data, LED0);
			Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, Chip_SCTPWM_PercentageToTicks(LPC_SCT, percentageUART));
			//cambiar intensidad del LED 1 con la salida
			//LED_Duty_change(salidaUART, LED1);
			Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, salidaUART));
			maquina.state = IDLE_MSG;
		}

		BUTTON_STATUS_POLLED = Buttons_GetStatus();
		//Si se presionan varios botones al mismo tiempo BUTTON_STATUS_POLLED no va a estar entre 1..4
		if(BUTTON_STATUS_POLLED != 0  && BUTTON_STATUS_POLLED > 0 && BUTTON_STATUS_POLLED < 5 ){
			while(antiRebAcum < BTN_SHOULD_CLICK && Buttons_GetStatus() == BUTTON_STATUS_POLLED){
				antiRebAcum++;
				pausems(10);
			}

			if(antiRebAcum == BTN_SHOULD_CLICK){
				current_duty = boton_salida(current_duty, BUTTON_STATUS_POLLED-1);
			}

			antiRebAcum = 0;
		}

	}

	return ret;
}



/** @} doxygen end group definition */

/*==================[end of file]============================================*/
