#include "jogo.h"


void start(){
	setClk();
    Nokia5110_Init();
    Nokia5110_Clear();
    habPerifericos();
	G_posmenu=0;
    G_modo=0;
    configButtons();
    configTimer();
    habInt();
    while(1){
	    //Nokia5110_Clear();
	    if(G_modo==0){
	        printTelainicio();
	    }else if(G_modo==1){
	        printInstrucoes();
	    }else if (G_modo==2){

	    }
	    Nokia5110_DrawFullImage(Buttons);
	    SysCtlDelay((SysCtlClockGet())/10);
	}
}

void habPerifericos(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
}

void habInt(){
	IntMasterEnable();

	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOIntRegister(GPIO_PORTF_BASE,trataMenu);
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
}

void configTimer(){
	TimerIntDisable(TIMER0_BASE,TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT | TIMER_CFG_B_ONE_SHOT);
    TimerLoadSet(TIMER0_BASE,TIMER_BOTH,(SysCtlClockGet()/20));
    TimerIntRegister(TIMER0_BASE,TIMER_A,trataTimer0A);
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
}

void configButtons(){
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void MUXButtons(){

}

void setClk(){
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_SYSDIV_5);
}

void printTelainicio(){
    Nokia5110_Clear();
    Nokia5110_SetCursor(3,0);
    Nokia5110_OutString("Damas");
    Nokia5110_SetCursor(1,2);
    Nokia5110_OutString("Jogar");
    Nokia5110_SetCursor(1,3);
    Nokia5110_OutString("Instrucoes");
    if(G_posmenu==0){
        Nokia5110_MyDrawing(seta,0,2,1,8);
    }else if(G_posmenu==1){
        Nokia5110_MyDrawing(seta,0,3,1,8);
    }
}

void printInstrucoes(){
    Nokia5110_Clear();
    uint32_t cont,l=0;
    for(cont=G_linhaInst;cont<G_linhaInst+3;cont++){
        Nokia5110_SetCursor(0,l);
        //Nokia5110_OutString(inst[cont]);
        l++;
    }
    Nokia5110_SetCursor(0,5);
    Nokia5110_OutString("SW81 p/ sair");
}


void trataMenu(){
    GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
    if(G_posmenu==0)G_posmenu=1;
    else G_posmenu=0;
    TimerEnable(TIMER0_BASE,TIMER_A);
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
}

void trataTimer0A(){
    TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
}