//******************************************************************************
/*
  * FileName    : ov7670driver.c
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

#include "ov7670driver.h"
#include "ov7670regs.h"
#include "i2cdriver.h"
#include "uartdriver.h"

//----------------- <> DeclaraÃ§Ã£o de VariÃ¡veis Globais <> ---------------------

uint8_t qqvgaframe1[QQVGA_HEIGHT * QQVGA_WIDTH];
__DATA(RAM2) uint8_t qqvgaframe2[QQVGA_HEIGHT * QQVGA_WIDTH];

//---------------------- <> Implementation of Functions <> -----------------------

/**
 * Configura os pinos de I/O do microcontrolador
 */
void GpioInit(void)
{
    /* 0.22 used to reset the camera */
    LPC_PINCON->PINSEL1 &= ~(3 << 12); /* set as gpio */
    LPC_GPIO0->FIODIR |= (1 << 22); /* set as output */
    LPC_PINCON->PINMODE1 &= ~(1 << 12); /* no pulldown/up */
    LPC_PINCON->PINMODE1 |= (1 << 13); /* no pulldown/up */

    /* ports D0..D7 go to P2.0..P2.7 */
    LPC_PINCON->PINSEL4 &= ~(0xffff); /* function = gpio */
    LPC_GPIO2->FIODIR &= ~(0xff); /* direction = input */

    /* port 2.8 vsync */
    LPC_PINCON->PINSEL4 &= ~(3 << 16); /* function = gpio */
    LPC_GPIO2->FIODIR &= ~(1 << 8); /* direction = input */

    /* port 2.11 href */
    LPC_PINCON->PINSEL4 &= ~(3 << 22); /* function = gpio */
    LPC_GPIO2->FIODIR &= ~(1 << 11); /* direction = input */

    /* port 2.12 pclk */
    LPC_PINCON->PINSEL4 &= ~(3 << 24); /* function = gpio */
    LPC_GPIO2->FIODIR &= ~(1 << 12); /* direction = input */

//    /* clkout of 12mhz on 1.27, MAIN_CLK como fonte de clock sem divisor FOUT = 12Mhz */
//    LPC_PINCON->PINSEL3 &=~(3<<22);
//    LPC_PINCON->PINSEL3 |= (1<<22);
//    LPC_SC->CLKOUTCFG = ((LPC_SC->CLKOUTCFG &~(0b1111 << 4)) | (1<<8 | 0b0001));//enable and divide by 1

    /* clkout of 10mhz on 1.27, C_CLK como fonte de clock sem divisor FOUT = 10Mhz */
	LPC_PINCON->PINSEL3 &= ~(3 << 22);
	LPC_PINCON->PINSEL3 |= (1 << 22);
	LPC_SC->CLKOUTCFG = (0b0000 | 9 << 4 | 1 << 8); //enable and divide by 10

    printf("Reset do módulo\r\n");
    LPC_GPIO0->FIOCLR |= (1 << 22); //low
    delay(100);
    LPC_GPIO0->FIOSET |= (1 << 22); //high
    delay(100);

}//fim da funÃ§Ã£o GpioInit
//-----------------------------------------------------------------------------


/**
 * Initializa o módulo ov7670, configura os registros básicos de funcionamento
 */
void InitOv7670(void)
{
    printf("Inicializando o módulo ov7670\r\n");

/* Como forma de testar a comunicaÃ§Ã£o faz a leitura do valor MSB do PID do mÃ³dulo, deve ser 0x76 para o OV7670 */
    if (getRegOv7670(REG_PID) != 0x76)
    {
        printf("Erro ao ler o product ID Msb != 0x76!\r\n");
        while (1);
    }

/* Reset to default values */
    setRegOv7670(REG_COM7, 0x80);
    delay(100);

/* Configura o clock externo 24MHz diretamente */

   	setRegOv7670(REG_DBLV,  0x01);//0b11000000);//Multiplica o PLL x4 TODO: Multiplicador nÃ£o estÃ¡ funcionando
   	setRegOv7670(REG_CLKRC, 0x01);//0b01000000);//bit 6 = 0 Habilita o prescaller e divide por 2


   	//XCLK = 10MHz, imagem Preta - NÃƒO
   	//DBLV = 0x40
   	//CLKRC = 0x80
   	//PCLK = 10 MHz

   	//XCLK = 10MHz, imagem OK
   	//DBLV = 0x01
   	//CLKRC = 0x81
   	//PCLK = 1,25 MHz

   	//XCLK = 10MHz, imagem um pouco melhor OK
   	//DBLV = 0x01
   	//CLKRC = 0x01
   	//PCLK = 625 KHz


   	//XCLK = 10MHz, imagem um pouco melhor a noite OK
   	//DBLV = 0x01
   	//CLKRC = 0x84
   	//PCLK = 500 KHz

    setRegOv7670(REG_COM11, 0x0A);//0x4A);
    setRegOv7670(REG_TSLB, 0x0D);
    setRegOv7670(REG_COM13, 0x80);//Gamma enable [7]
    setRegOv7670(REG_COM7, 0x04); /* output format: rgb QQVGA */

/* Set RGB565 */
    setRegOv7670(REG_COM15, 0xD0);

/* Define dimensões do quadro QQVGA */
    setRegOv7670(REG_HSTART, 0x16);
    setRegOv7670(REG_HSTOP, 0x04);
    setRegOv7670(REG_HREF, 0x24);
    setRegOv7670(REG_VSTART, 0x02);
    setRegOv7670(REG_VSTOP, 0x7a);
    setRegOv7670(REG_VREF, 0x0a);
    setRegOv7670(REG_COM10, 0x02);

    setRegOv7670(REG_COM3, 0x04);
    setRegOv7670(REG_COM14, 0x1A); // divide by 4  0x1b divide by 8

    setRegOv7670(SCALING_XSC, 0x3A);
    setRegOv7670(SCALING_YSC, 0x35);
    setRegOv7670(SCALING_DCWCTR, 0x22); // downsample by 4 0x33 downsample by 8
    setRegOv7670(SCALING_PCLK_DIV, 0xF2); // divide by 4     0xf3 divide by 8
    setRegOv7670(SCALING_PCLK_DELAY, 0x02);

    setRegOv7670(REG_MVFP, 0x27);

    //setRegOv7670(REG_BRIGHT,0x18);  // 0x00(Brightness 0) - 0x18(Brightness +1) - 0x98(Brightness -1)
    //setRegOv7670(REG_CONTRAS,0x38); // 0x40(Contrast 0) - 0x50(Contrast +1) - 0x38(Contrast -1)

/* test pattern Gera o padrÃ£o de barras coloridas 8 barras verticais */
#ifdef TEST_PATTERN
    setRegOv7670(SCALING_XSC, 0x3A);
    setRegOv7670(SCALING_YSC, 0xB5);
#endif

    // COLOR SETTING
    setRegOv7670(0x4f, 0x80);
    setRegOv7670(0x50, 0x80);
    setRegOv7670(0x51, 0x00);
    setRegOv7670(0x52, 0x22);
    setRegOv7670(0x53, 0x5e);
    setRegOv7670(0x54, 0x80);
    setRegOv7670(0x56, 0x40);
    setRegOv7670(0x58, 0x9e);
    setRegOv7670(0x59, 0x88);
    setRegOv7670(0x5a, 0x88);
    setRegOv7670(0x5b, 0x44);
    setRegOv7670(0x5c, 0x67);
    setRegOv7670(0x5d, 0x49);
    setRegOv7670(0x5e, 0x0e);
    setRegOv7670(0x69, 0x00);
    setRegOv7670(0x6a, 0x40);
    setRegOv7670(0x6c, 0x0a);
    setRegOv7670(0x6d, 0x55);
    setRegOv7670(0x6e, 0x11);
    setRegOv7670(0x6f, 0x9f);

    setRegOv7670(0xb0, 0x84);//Corrige as cores de forma automática

    printf("Ov7670Init - OK\r\n");

}//fim da funÃ§Ã£o Ov7670Init
//-----------------------------------------------------------------------------

/**
 * Read a frame from Ov7670 and load on memory buffer
 */
void Ov7670ReadFrame(void) {
	while (PIN_VSYNC); /* wait for the old frame to end */
	while (!PIN_VSYNC); /* wait for a new frame to start */

	uint32_t i = 0;
	while (PIN_VSYNC) {

		while (PIN_VSYNC && !PIN_HREF); /* wait for a line to start */
		if (!PIN_VSYNC)
			break; /* line didn't start, but frame ended */
		while (PIN_HREF)/* wait for a line to end */
		{
			/* first byte */
			while (!PIN_PCLK); /* wait for clock to go high */
			qqvgaframe1[i] = LPC_GPIO2->FIOPIN;
			while (PIN_PCLK); /* wait for clock to go back low */

			/* second byte */
			while (!PIN_PCLK); /* wait for clock to go high */
			qqvgaframe2[i] = LPC_GPIO2->FIOPIN;
			while (PIN_PCLK); /* wait for clock to go back low */
			i++;
		}
	}

}//end of function Ov7670ReadFrame
//-----------------------------------------------------------------------------


/**
 * Send frame raw rgb565 from memory buffer to uart
 */
void Ov7670SendFrame(void) {
	int row = 0;
	int column = 0;
	for (row = 0; row < QQVGA_HEIGHT; row++) {
		for (column = 0; column < QQVGA_WIDTH; column++) {
			UartTxChar(qqvgaframe1[column + (row * QQVGA_WIDTH)]);
			UartTxChar(qqvgaframe2[column + (row * QQVGA_WIDTH)]);
		}
	}
}//end of function Ov7670SendFrame
//-----------------------------------------------------------------------------


/**
 * Escreve em um registro do mÃ³dulo ov7670
 * @regName: O nome do registrador a ser escrito, definido em ov7670regs.h
 * @value: o valor a ser escrito registrador, do tipo 8 bits unsigned
 * @retorna: nada
 */
uint32_t setRegOv7670(eRegsOv7670 regName, uint8_t value)
{
    i2c_clearbuffers();

    I2CWriteLength = 3;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = OV7670_ADDR;   /* endereÃ§o do ov7670 no barramento i2c */
    I2CMasterBuffer[1] = regName;       /* seleciona o registrador */
    I2CMasterBuffer[2] = value;         /* escreve o valor */

    return I2CEngine();

}//end of function setRegOv7670
//-----------------------------------------------------------------------------


/**
 * LÃª um registro do mÃ³dulo ov7670
 * @regName: O nome do registrador a ser lido, definido em ov7670regs.h
 * @retorna: o valor de 8 bits unsigned lido do registrador
 */
uint8_t getRegOv7670(eRegsOv7670 regName)
{
    i2c_clearbuffers();
    I2CWriteLength = 2;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = OV7670_ADDR;   /* endereÃ§o do ov7670 no barramento i2c */
    I2CMasterBuffer[1] = regName;       /* seleciona o registrador */

    I2CEngine();

    delay(1);

    i2c_clearbuffers();
    I2CWriteLength = 0;
    I2CReadLength = 1;
    I2CMasterBuffer[0] = OV7670_ADDR | RD_BIT;

    while (I2CEngine() == I2CSTATE_SLA_NACK);

    return I2CSlaveBuffer[0];

}//fim da funÃ§Ã£o getRegOv7670
//-----------------------------------------------------------------------------


void delay(int n) {
	volatile int d;
	for (d = 0; d < n * 3000; d++) {
	}
}
