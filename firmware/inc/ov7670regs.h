/*
 * ov7670regs.h
 *
 *  Created on: 07/08/2016
 *      Author: PaulinoJr.c
 */

#pragma once

typedef enum {

/** ClocK control */
	REG_CLKRC = 0x11,
/** Registro de controle 7 */
	REG_COM7 = 0x12,
/** lots of stuff */
	REG_TSLB = 0x3A,
/** Control 11 */
	REG_COM11 = 0x3B,
/** Control 13 */
	REG_COM13 = 0x3D,
/** Control 15 */
	REG_COM15 = 0x40,
/** Horizontal start high bits */
	REG_HSTART = 0x17,
/** Horizontal stop high bits */
	REG_HSTOP = 0x18,
/** HREF pieces */
	REG_HREF = 0x32,
/** Vertical start high bits */
	REG_VSTART = 0x19,
/** Vertical stop high bits */
	REG_VSTOP = 0x1A,
/** Pieces of GAIN, VSTART, VSTOP */
	REG_VREF = 0x03,
/** Control 10 */
	REG_COM10 = 0x15,
/** Control 3 */
	REG_COM3 = 0x0C,
/** Control 14 */
	REG_COM14 = 0x3E,
/** Control 17 */
	REG_COM17 = 0x42,
/** Mirror / vflip */
	REG_MVFP = 0x1E,

/** */

/** DBLV */
	REG_DBLV = 0x6B,
/** SCALING_XSC */
	SCALING_XSC = 0x70,
/** SCALING_YSC */
	SCALING_YSC = 0x71,
/** DCW Control */
	SCALING_DCWCTR = 0x72,
/** Scaling Pixel clock div */
	SCALING_PCLK_DIV = 0x73,
/**  */
	SCALING_PCLK_DELAY = 0xA2,
/** Product ID MSB, apenas leitura */
	REG_PID = 0x0A,
/** Product ID LSB, apenas leitura */
	REG_VER = 0x0B,

	REG_BRIGHT = 0x55,
	REG_CONTRAS = 0x56,

}eRegsOv7670;

#define COM15_RGB565	0x10 /* RGB565 output */
#define	COM13_UVSAT		0x40 /* UV saturation auto adjustment */
#define COM15_R00FF		0xc0 /*	00 to FF */
#define COM7_RGB		0x04 /* bits 0 and 2 - RGB format */
#define COM10_PCLK_HB	0x20 /* Suppress PCLK on horiz blank */
#define COM3_DCWEN		0x04 /* Enable downsamp/crop/window */
