#include "jogo.h"

// Para G_modo
#define MODO_MENU 0
#define MODO_JOGO 1
#define MODO_INST 2
#define MODO_VITO 3

// Para limitar os valores de G_posmenu e G_linhaInst
#define MENU_MAX 1
#define INST_MAX 1

pedra_s pedras[24];

void start(){
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_SYSDIV_2_5);
    Nokia5110_Init();
    Nokia5110_Clear();
    habPerifericos();
	G_posmenu=0;
    G_modo=MODO_MENU;
    G_linhaInst=0;
    G_cursor_x=0;
    G_cursor_y=0;
    G_vit=0;
    G_mux=1;
    configButtons();
    configTimer();
    habInt();
}

// Habilita os perifericos necessarios
static void habPerifericos(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    
}

// Habilita interrupt dos botoes da matriz
static void habInt(){
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntRegister(GPIO_PORTF_BASE,trataGPIOF);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

    IntMasterEnable();
}


static void configTimer(){
    uint32_t delay = SysCtlClockGet()/10;

    SysTickIntDisable();
    SysTickPeriodSet(delay);
    SysTickIntRegister(trataSysTick);
    SysTickIntEnable();
    SysTickEnable();
}

// Configura os botoes da matriz
static void configButtons(){
    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    limpaMatriz();
}

// Set bits da matriz
static void limpaMatriz(){
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

// Multiplexa os botoes da matriz de botoes. PF4 nao eh multiplexado pois nao eh utilizado.
static void MUXButtons(){
    limpaMatriz();
    switch (G_mux){
    case 1:
        limpaMatriz();
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0x00);
        break;
    case 2:
        limpaMatriz();
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0x00);
        break;
    case 3:
        limpaMatriz();
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0x00);
        G_mux=0;
        break;
    }
    G_mux++;
}

//
static void printTelainicio(){
    Nokia5110_Clear();
    if(G_posmenu==0) Nokia5110_DrawFullImage(Menu1_full);
    if(G_posmenu==1) Nokia5110_DrawFullImage(Menu2_full);
}

//
static void printInstrucoes(){
    Nokia5110_Clear();
    Nokia5110_DrawFullImage(Buttons);
}

// ISR dos botoes da matriz
void trataGPIOF(){
    if(debouncer>0)return;
    const uint8_t tempo=5;
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_0) == GPIO_INT_PIN_0)
    {
        debouncer=tempo;
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
        if(G_mux==2){
            if(G_cursor_x == 0) G_cursor_x = 7;
            else G_cursor_x--;
            TimerEnable(TIMER0_BASE,TIMER_A);
            GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
        }
    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_1) == GPIO_INT_PIN_1)
    {
        debouncer=tempo;
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
        if(G_mux==1){
            switch (G_modo){
                case MODO_MENU:
                    if(G_posmenu>0) G_posmenu--;
                    break;
                case MODO_INST:
                    if(G_linhaInst>0)G_linhaInst--;
                    break;
                case MODO_JOGO:
                    if(G_cursor_y == 0) G_cursor_x = 7;
                    else G_cursor_y--;
                    break;
            }            
        }else if(G_mux==3){
            switch (G_modo){
                case MODO_MENU:
                    if(G_posmenu<MENU_MAX) G_posmenu++;
                    break;
                case MODO_INST:
                    if(G_linhaInst<INST_MAX) G_linhaInst--;
                    break;
                case MODO_JOGO:
                    if(G_cursor_y == 7) G_cursor_x = 0;
                    else G_cursor_y++;
                    break;
            }       
        }
    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_2) == GPIO_INT_PIN_2)
    {
        debouncer=tempo;
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_2);
        if(G_mux==2){
            if(G_modo == MODO_JOGO){
                if(G_cursor_x == 7) G_cursor_x = 0;
                else G_cursor_x++;
            }
        }

    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_3) == GPIO_INT_PIN_3)
    {
        debouncer=tempo;
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        if(G_mux==2){
            switch (G_modo){
                case MODO_MENU:
                    if(G_posmenu==0){
                        G_modo=MODO_JOGO;
                        G_cursor_x=0;
                        G_cursor_y=0;
                        G_selec_x=-1;
                        G_selec_y=-1;
                        G_player=1;
                        G_vit=0;
                    }else if(G_posmenu==1){
                        G_modo=MODO_INST;
                        G_linhaInst=0;
                    }
                    G_posmenu=0;
                    break;
                case MODO_INST:
                    G_modo=MODO_MENU;
                    G_linhaInst=0;
                    G_posmenu=0;
                    break;
                case MODO_JOGO:
                //coisa pra fazer
                    break;
                case MODO_VITO:
                    G_modo=MODO_MENU;
                    G_vit=0;
                    G_posmenu=0;
                    break;
            }        
        }
    }
}


// ISR de MUX e refresh de tela
void trataSysTick(){
    MUXButtons();
    switch (G_modo){
        case MODO_MENU:
            printTelainicio();
            break;
        case MODO_INST:
            printInstrucoes();
            break;
        case MODO_JOGO:
        //coisa pra fazer
            break;
        case MODO_VITO:
        //coisa pra fazer
            break;
    }
    if(debouncer>0)debouncer--;
}
