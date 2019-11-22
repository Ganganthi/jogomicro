#ifndef JOGO_H
#define JOGO_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "global.h"
#include "bitmaps.h"

#include "inc/hw_uart.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "Nokia5110.h"
#include "driverlib/hibernate.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

typedef struct pedra
{
	uint8_t x;
	uint8_t y;
	uint8_t player;
	bool dama;
	bool life;

}pedra_s;


void start();

static void habPerifericos();

static void habInt();

static void configTimer();

static void configButtons();

static void limpaMatriz();

static void MUXButtons();

static void printTelainicio();

static void printInstrucoes();

void trataGPIOF();

void trataSysTick();

static void resetPieces();

static void printPieces();

static void printP1(uint8_t x, uint8_t y);
static void printP1Inv(uint8_t x, uint8_t y);

static void printP2(uint8_t x, uint8_t y);
static void printP2Inv(uint8_t x, uint8_t y);

#endif
