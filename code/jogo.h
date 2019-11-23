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
	uint8_t x;			// 0-7 if alive, 10 if dead
	uint8_t y;			// 0-7 if alive, 10 if dead
	uint8_t player;		// 1 if player 1, 2 if player 2
	bool dama;			// false if not, true if is
	bool life;			// false if dead, true if alive

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

void trataTimer0();

void trataGPIOF();

void trataSysTick();

static void resetPieces();

static void printPieces();

static void printP1(uint8_t x, uint8_t y);
static void printP1Inv(uint8_t x, uint8_t y);

static void printP2(uint8_t x, uint8_t y);
static void printP2Inv(uint8_t x, uint8_t y);

static void printFull(uint8_t x, uint8_t y);
static void printBlank(uint8_t x, uint8_t y);

static void printJogo();

static int checkSpace(int x,int y);

static int checkMove();

static bool canCapture();

#endif
