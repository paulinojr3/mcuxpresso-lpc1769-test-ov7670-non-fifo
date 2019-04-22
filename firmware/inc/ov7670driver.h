//******************************************************************************
/*
  * FileName    : ov7670driver.h
  * Created By  : PaulinoJr.c since  07/08/16
  * Copyright (c) 2016 - CodByte soluções em sistemas embarcados www.codbyte.com.br
  * Description : Contém os cabeçalhos das funções de controle do módulo OV7670
  * Objective   : Educacional
  * Language    : "C/C++"  MPLAB XC32++ version 1.4
*/
//******************************************************************************

#pragma once

//--------------------------- *** Includes Library *** -------------------------

/* LPC17xx Peripheral Registers */
#include "LPC17xx.h"
/* Definições dos registros do módulo ov7670 */
#include "ov7670regs.h"
/* Definições do driver i2c <NXP> */
#include "i2cdriver.h"


//------------------------- <> Constants Definitions <> ------------------------

/** Define endereço do módulo ov7670 no barramento i2c */
#define    OV7670_ADDR     0x42
/** Define a largura do frame em 160 pixels, resolução QQVGA */
#define    QQVGA_WIDTH     160
/** Define a altura do frame em 120 pixels, resolução QQVGA */
#define    QQVGA_HEIGHT    120


/** Define que o módulo irá gerar o padrão de barras coloridas */
//#define    TEST_PATTERN


//------------------------- <> I/O Definitions pins <> -------------------------

/** Define o pino do sincronismo vertical VSYNC no pino P2[8] pino J6-50 */
#define    PIN_VSYNC    (LPC_GPIO2->FIOPIN & (1 << 8))
/** Define o pino do sincronismo horizontal HREF no pino P2[11] pino J6-52 */
#define    PIN_HREF     (LPC_GPIO2->FIOPIN & (1 << 11))
/** Define o pino do pixel clock PCLK no pino P2[12] pino J6-53 */
#define    PIN_PCLK     (LPC_GPIO2->FIOPIN & (1 << 12))


/**
 * Apenas para mapear os pinos do barramento de dados, a leitura dos dados
 * da câmera será feita direto do registrador de 8 bits com acesso a esses pinos
 */
/** Define o pino D0 no pino P2[0] pino J6-42 */
#define    PIN_D0    (LPC_GPIO2->FIOPIN & (1 << 0))
/** Define o pino D1 no pino P2[0] pino J6-43 */
#define    PIN_D1    (LPC_GPIO2->FIOPIN & (1 << 1))
#define    PIN_D2    (LPC_GPIO2->FIOPIN & (1 << 2))
#define    PIN_D3    (LPC_GPIO2->FIOPIN & (1 << 3))
#define    PIN_D4    (LPC_GPIO2->FIOPIN & (1 << 4))
#define    PIN_D5    (LPC_GPIO2->FIOPIN & (1 << 5))

#define    PIN_D6    (LPC_GPIO2->FIOPIN & (1 << 6))
/** Define o pino D7 no pino P2[0] pino J6-49 */
#define    PIN_D7    (LPC_GPIO2->FIOPIN & (1 << 7))


//----------------------- <> Declaração de Variáveis <> -----------------------

extern uint8_t qqvgaframe1[QQVGA_HEIGHT * QQVGA_WIDTH];
extern __DATA(RAM2) uint8_t qqvgaframe2[QQVGA_HEIGHT * QQVGA_WIDTH];

//------------------------- <> Protótipo das Funções <> -----------------------

/** Configura os pinos de I/O do microcontrolador */
void GpioInit(void);
/** Delay para rotina de tempo */
void delay(int n);


/** Initializa o módulo ov7670 */
void InitOv7670(void);
/** Captura um frame (foto) do módulo ov7670 */
void Ov7670ReadFrame(void);

void Ov7670SendFrame(void);
/** Escreve em um registro do módulo ov7670 */
uint32_t setRegOv7670(eRegsOv7670 regName, uint8_t value);
/** Lê um registro do módulo ov7670 */
uint8_t getRegOv7670(eRegsOv7670 regName);
