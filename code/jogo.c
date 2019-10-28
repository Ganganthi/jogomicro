#include "jogo.h"

#define MODO_MENU 0
#define MODO_JOGO 1
#define MODO_INST 2
#define MODO_VITO 3

#define MENU_MAX
#define INST_MAX

void start(){
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_SYSDIV_5);
    Nokia5110_Init();
    Nokia5110_Clear();
    habPerifericos();
	G_posmenu=0;
    G_modo=MODO_MENU;
    G_linhaInst=0;
    G_cursor_x=0;
    G_cursor_y=0;
    G_vit=0;
    configButtons();
    configTimer();
    habInt();
}

// Habilita os perifericos necessarios
void habPerifericos(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4));
}

// Habilita interrupt dos botoes da matriz
void habInt(){
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntRegister(GPIO_PORTF_BASE,trataGPIOF);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

    IntMasterEnable();
}

//Configura os timer de debouce (0-4) e o timer periodico (5)
void configTimer(){
    uint32_t delay = SysCtlClockGet()/20;
	TimerIntDisable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    TimerLoadSet(TIMER0_BASE, TIMER_A, delay);
    TimerIntRegister(TIMER0_BASE, TIMER_A, trataTimer0);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);
    TimerLoadSet(TIMER1_BASE, TIMER_A, delay);
    TimerIntRegister(TIMER1_BASE, TIMER_A, trataTimer1);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT);
    TimerLoadSet(TIMER2_BASE, TIMER_A, delay);
    TimerIntRegister(TIMER2_BASE, TIMER_A, trataTimer2);
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER3_BASE, TIMER_CFG_ONE_SHOT);
    TimerLoadSet(TIMER3_BASE, TIMER_A, delay);
    TimerIntRegister(TIMER3_BASE, TIMER_A, trataTimer3);
    TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER4_BASE, TIMER_A, delay);
    TimerIntRegister(TIMER4_BASE, TIMER_A, trataTimerP);
    TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
}

// Configura os botoes da matriz
void configButtons(){
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    limpaMatriz();
}

// Set bits da matriz
void limpaMatriz(){
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

// Multiplexa os botoes da matriz de botoes. PF4 nao eh multiplexado pois nao eh utilizado.
void MUXButtons(){
    limpaMatriz();
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0x00);
    limpaMatriz();
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0x00);
    limpaMatriz();
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0x00);
    limpaMatriz();
}

//
void printTelainicio(){
    // Nokia5110_Clear();
    // Nokia5110_SetCursor(3,0);
    // Nokia5110_OutString("Damas");
    // Nokia5110_SetCursor(1,2);
    // Nokia5110_OutString("Jogar");
    // Nokia5110_SetCursor(1,3);
    // Nokia5110_OutString("Instrucoes");
    // if(G_posmenu==0){
    //     Nokia5110_MyDrawing(seta,0,2,1,8);
    // }else if(G_posmenu==1){
    //     Nokia5110_MyDrawing(seta,0,3,1,8);
    // }
}

//
void printInstrucoes(){
    // Nokia5110_Clear();
    // volatile uint32_t cont,l=0;
    // for(cont=G_linhaInst;cont<G_linhaInst+3;cont++){
    //     Nokia5110_SetCursor(0,l);
    //     //Nokia5110_OutString(inst[cont]);
    //     l++;
    // }
    // Nokia5110_SetCursor(0,5);
    // Nokia5110_OutString("SW81 p/ sair");
}

// ISR dos botoes da matriz
void trataGPIOF(){
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_0) == GPIO_INT_PIN_0)
    {
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
        if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1) == 0x00){
            if(G_modo == MODO_JOGO){
                if(G_cursor_x == 0) G_cursor_x = 7;
                else G_cursor_x--;
                TimerEnable(TIMER0_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
            }
        }
    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_1) == GPIO_INT_PIN_1)
    {
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
        if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0) == 0x00){
            if(G_modo == MODO_JOGO){
                if(G_cursor_y == 0) G_cursor_x = 7;
                else G_cursor_y--;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }else if(G_modo == MODO_MENU){
                if(G_posmenu>0) G_posmenu--;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }else if(G_modo == MODO_INST){
                if(G_linhaInst>0) G_linhaInst--;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }
        }else if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5) == 0x00){
            if(G_modo == MODO_JOGO){
                if(G_cursor_y == 7) G_cursor_x = 0;
                else G_cursor_y++;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }else if(G_modo == MODO_MENU){
                if(G_posmenu<MENU_MAX) G_posmenu++;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }else if(G_modo == MODO_INST){
                if(G_linhaInst<INST_MAX) G_linhaInst--;
                TimerEnable(TIMER1_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
            }
        }
    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_2) == GPIO_INT_PIN_2)
    {
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_2);
        if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1) == 0x00){
            if(G_modo == MODO_JOGO){
                if(G_cursor_x == 7) G_cursor_x = 0;
                else G_cursor_x++;
                TimerEnable(TIMER2_BASE,TIMER_A);
                GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_2);
            }
        }

    }
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_3) == GPIO_INT_PIN_3)
    {
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        if(G_modo == MODO_JOGO){
            seleciona();
            TimerEnable(TIMER3_BASE,TIMER_A);
            GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        }else if(G_modo == MODO_MENU){
            if(G_posmenu==0) G_modo=MODO_JOGO;
            if(G_posmenu==1) G_modo=MODO_INST;
            G_posmenu=0;
            TimerEnable(TIMER3_BASE,TIMER_A);
            GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        }else if(G_modo == MODO_INST){
            G_modo=MODO_MENU;
            G_linhaInst=0;
            TimerEnable(TIMER3_BASE,TIMER_A);
            GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        }else if(G_modo == MODO_VITO){
            G_modo=MODO_MENU;
            G_vit=0;
            TimerEnable(TIMER3_BASE,TIMER_A);
            GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
        }
    }   
}

// ISR de deboucer
void trataTimer0(){
    TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
}

void trataTimer1(){
    TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_1);
}

void trataTimer2(){
    TimerIntClear(TIMER2_BASE,TIMER_TIMA_TIMEOUT);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_2);
}

void trataTimer3(){
    TimerIntClear(TIMER3_BASE,TIMER_TIMA_TIMEOUT);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_3);
}

// ISR de MUX e refresh de tela
void trataTimerP(){
    MUXButtons();
    // if(G_modo==0){
    //     printTelainicio();
    // }else if(G_modo==1){
    //     printInstrucoes();
    // }else if (G_modo==2){

    // }
    Nokia5110_DrawFullImage(Buttons);
}
