//******************************************************************************
/*
  * FileName    : main.c
  * Created By  : PaulinoJr.c since  21/04/2019
  * Copyright (c) 2019 - CodByte solutions for embedded systems www.codbyte.com.br
  * Description : Provides functions for control OV7670 camera module
  * Language    : "C" MCUXpresso version 10.0
*/
//******************************************************************************

//--------------------------- *** Includes Library *** -------------------------

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <string.h>

#include "ov7670driver.h"
#include "uartdriver.h"

extern volatile int isCommandReceived;

void InitBoard(void) {

	UartInit(115200);

    if (I2CInit((uint32_t) I2CMASTER) == 0) {
        printf("Fatal i2c error!\n");
        while (1);
    }
}

int main(void) {
	char bufferCommand[20];
	uint8_t lengthCmd = 0;

	SystemInit();

	GpioInit();

	InitBoard();

	InitOv7670();

	printf("Ov7670 Snap Photo\n");
	printf("System clock: [%lu]\n", SystemCoreClock);
	UartSend("Ov7670 Snap Photo\r\n");

	isCommandReceived = 0;

	while (1) {

		if (isCommandReceived) {

			lengthCmd = UartReceive(bufferCommand);

			if (!strncmp(bufferCommand, "snap", lengthCmd)) {
				Ov7670ReadFrame();//Load frame from Ov7670 to memory buffer
				UartSend("Done!\n");
			} else if (!strncmp(bufferCommand, "getimage", lengthCmd)) {
				Ov7670SendFrame();//Send frame from memory buffer to uart
			} else {
				UartSend("Invalid Command!\n");
			}
			isCommandReceived = 0;
		}

	}
	return 0;
}
