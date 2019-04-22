//******************************************************************************
/*
  * FileName    : uartdriver.c
  * Created By  : PaulinoJr.c since  21/04/2019
  * Description : Contém a implementação dos métodos da classe UsartDriver
  * Objective   : Construção de um driver para o periférico USART0 do ARM CROTEX-M3
  * Language    : "C/C++"  Ambiente Keil uVision versão 4.71.2.0
*/
//******************************************************************************

//----------------------- *** Inclusão das Bibliotecas *** ---------------------
#include "uartdriver.h"

//--------------------- <> Desenvolvimento dos métodos <> --------------------//

/** Buffer for received bytes */
uint8_t rxBuffer[20];
/** Receive byte counter */
volatile uint8_t rxCounter;
/** Flag command received */
volatile int isCommandReceived;

/*
 *	método que configura a UART0
 * 8 Bit de Dados, 1 Stop Bit, Paridade N, Baud rate 9600
 * Pino 98 TX, pino 99 RX
 */
void UartInit(int baud) {
	float res_f;
	unsigned int res_i;

	LPC_SC->PCONP |= PCUART0_POWERON;

	//PCLK_UART0 = C_CLK/4.
	LPC_SC->PCLKSEL0 &= ~(0b11 << 6);

	// Atualiza o valor da frequência de clock do sistema.
	SystemCoreClockUpdate();

	res_f = (float) SystemCoreClock / (16 * 4 * (float) baud);
	res_i = SystemCoreClock / (16 * 4 * baud);

	if (res_i > res_f)
		res_i--;

	LPC_UART0->LCR = 0x80;	             //DLAB = 1
	LPC_UART0->DLL = (res_i & 0xFF);	     //U0DLL = 81
	LPC_UART0->DLM = ((res_i >> 8) & 0xFF);  //U0DLM = 0
	LPC_UART0->FDR = 0 | 1 << 4;           //DIVADDVAL = 0 e MULVAL = 1.

	//Configura os pinos P0.2 e P0.3 para a função TXD0 e RXD0 respectivamente.
	LPC_PINCON->PINSEL0 = ((LPC_PINCON->PINSEL0 & ~(0b11 << 4 | 0b11 << 6))
			| (0b01 << 4 | 0b01 << 6));

	//UART0 – 8bits de dados, sem paridade, 1bit de parada e DLAB = 0.
	LPC_UART0->LCR = 0x03;
	//limpa o registro de transmissão
	//LPC_UART0->THR = 0x00;

	LPC_UART0->FCR = 0x07; //Ativa e reinicia as FIFOs da UART0.

	/*  RBR Interrupt Enable, THRE Interrupt Enable, RX Line Status Interrupt Enable */
	LPC_UART0->IER = 0x01;

//	habilita interrupção da UART0
//	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);

	isCommandReceived = 0;
	rxCounter = 0;
}		//fim do método init
//------------------------------------------------------------------------------

/*
 * Send byte to serial port
 *
 */
void UartTxChar(char ch) {

	while (!(LPC_UART0->LSR & (1 << 5))) {
		__NOP();
	}

	LPC_UART0->THR = ch;
}		//fim do método txChar
//------------------------------------------------------------------------------

/**
 * Método que recebe um caracter da UART0
 * @return: o caracter lido da UART0
 */
void UartRxChar(char data)
{
    rxBuffer[rxCounter] = data;
    rxCounter = (rxCounter + 1) % 20;

}//fim do método RxChar
//-----------------------------------------------------------------------------

uint8_t UartReceive(char* data) {
    uint8_t i = 0;

    while (rxCounter > 0){
        data[i] = rxBuffer[i];
        rxCounter--;
        i++;
    }

    data[i] = '\0';
    //UartSend(data);
    return i;
}

/*
 *		método que envia uma string para porta serial
 *
 */
void UartSend(const char *data) {

	while (*data != '\0') {
		UartTxChar(*data);		//envia a string byte a byte
		data++;
	}

}		//fim do método send
//------------------------------------------------------------------------------

/**
 * ISR Uart0
 */
void UART0_IRQHandler(void) {

	volatile char tmp;

	if (LPC_UART0->LSR & (1 << 0)) {
		tmp = LPC_UART0->RBR;
		if ((tmp == '\r') || (tmp == '\n')) {
			isCommandReceived = 1;
			return;
		}
		UartRxChar(tmp);
		//UartTxChar(tmp); //TODO: ECHO test
	}

} //-----------------------------------------------------------------------------
