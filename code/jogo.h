#ifndef JOGO_H
#define JOGO_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "global.h"
#include "bitmaps.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "Nokia5110.h"
#include "driverlib/hibernate.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"


void start();

void habPerifericos();

void habInt();

void configTimer();

void configButtons();

void MUXButtons();

void printTelainicio();

void printInstrucoes();

void trataMenu();

void trataTimer0A();

#endif
