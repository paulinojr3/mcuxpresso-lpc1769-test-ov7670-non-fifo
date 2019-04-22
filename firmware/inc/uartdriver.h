/*
 * uartdriver.h
 *
 *  Created on: 21 de abr de 2019
 *      Author: codByte
 */

#pragma once

#include <LPC17xx.h>

// PCUART0
#define PCUART0_POWERON (1 << 3)

#define PCLK_UART0 6
#define PCLK_UART0_MASK (3 << 6)

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

void UartInit(int baud);

void UartTxChar(char ch);

void UartRxChar(char data);

uint8_t UartReceive(char* data);

void UartSend(const char* data);
