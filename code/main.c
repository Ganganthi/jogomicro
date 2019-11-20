#define MAIN_FILE
#include "global.h"

#include "Nokia5110.h"
#include "jogo.h"
#include <stdint.h>
#include <string.h>
#include <time.h>

//--------


//----------

int main(void)
{
    start();
    //----
    /*SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);

    IntMasterEnable(); //PASSO 11
    IntEnable(INT_UART0); // PASSO 10
    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); // PASSO 9
    //UARTIntRegister(UART0_BASE, trataUART0);
    //---*/
    while(1);
	return 0;
}
