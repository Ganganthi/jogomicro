#include "jogo.h"

// Para G_modo
#define MODO_MENU 0
#define MODO_JOGO 1
#define MODO_INST 2
#define MODO_VITO 3

// Para limitar os valores de G_posmenu e G_linhaInst
#define MENU_MAX 1
#define INST_MAX 8

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
}

// Habilita interrupt dos botoes da matriz
static void habInt(){
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntRegister(GPIO_PORTF_BASE,trataGPIOF);
    IntPrioritySet(INT_GPIOF,0);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

    IntMasterEnable();
}

//Configure SysTick timer that refreshes the screen
static void configTimer(){
    uint32_t delay = SysCtlClockGet()/10;

    TimerIntDisable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE,TIMER_A,SysCtlClockGet()/7);
    TimerIntRegister(TIMER0_BASE,TIMER_A,trataTimer0);
    IntPrioritySet(INT_TIMER0A,1);
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE,TIMER_A);

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

// Prints the right image at the menu
static void printTelainicio(){
    Nokia5110_Clear();
    if(G_posmenu==0) Nokia5110_DrawFullImage(Menu1_full);
    if(G_posmenu==1) Nokia5110_DrawFullImage(Menu2_full);
}



// Prints intruction pages
static void printInstrucoes(){
    switch(G_linhaInst){
    	case 0:
    		Nokia5110_DrawFullImage(Buttons);
    		break;
		case 1:
			Nokia5110_DrawFullImage(Inst1);
			break;
		case 2:
			Nokia5110_DrawFullImage(Inst2);
			break;
		case 3:
			Nokia5110_DrawFullImage(Inst3);
			break;
		case 4:
			Nokia5110_DrawFullImage(Inst4);
			break;
		case 5:
			Nokia5110_DrawFullImage(Inst5);
			break;
		case 6:
			Nokia5110_DrawFullImage(Inst6);
			break;
		case 7:
			Nokia5110_DrawFullImage(Inst7);
			break;
		case 8:
			Nokia5110_DrawFullImage(Inst8);
			break;
    }
}

// ISR do timer de MUX
void trataTimer0(){
    TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    MUXButtons();
}

// ISR dos botoes da matriz
void trataGPIOF(){
    if(debouncer>0)return;
    const uint8_t tempo=5;

    // left arrow
    if ((GPIOIntStatus(GPIO_PORTF_BASE, true) & GPIO_INT_PIN_0) == GPIO_INT_PIN_0)
    {
        debouncer=tempo;
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
        if(G_mux==2){
            if(G_cursor_x == 0) G_cursor_x = 7;
            else G_cursor_x--;
        }
    }

    // up and down arrows
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
                    if(G_linhaInst<INST_MAX) G_linhaInst++;
                    break;
                case MODO_JOGO:
                    if(G_cursor_y == 7) G_cursor_x = 0;
                    else G_cursor_y++;
                    break;
            }       
        }
    }

    // right arrow
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

    // ok button (select button)
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
                        resetPieces();
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
                    if(G_selec_y==-1 && G_selec_x==-1){
                        if(G_player==checkSpace(G_cursor_x,G_cursor_y)){
                            G_selec_x=G_cursor_x;
                            G_selec_y=G_cursor_y;
                        }
                        break;
                    }else if(G_selec_y==G_cursor_y && G_selec_x==G_cursor_x){
                        G_selec_x=-1;
                        G_selec_y=-1;
                        break;
                    }
                    switch(checkMove()){
                        case 2:
                            G_selec_x=G_cursor_x;
                            G_selec_y=G_cursor_y;
                            break;
                        case 1:
                            if(G_player==1)G_player=2;
                            else G_player=1;
                            G_selec_x=-1;
                            G_selec_y=-1;
                            break;
                        case 0:
                            G_selec_x=-1;
                            G_selec_y=-1;
                            break;
                    }
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


// ISR de refresh de tela
void trataSysTick(){
    switch (G_modo){
        case MODO_MENU:
            printTelainicio();
            break;
        case MODO_INST:
            printInstrucoes();
            break;
        case MODO_JOGO:
            printJogo();
            break;
        case MODO_VITO:
        //coisa pra fazer
            break;
    }
    if(debouncer>0)debouncer--;
}


// Function to reset values from pieces
static void resetPieces(){
    uint8_t i;
    for(i=0;i<12;i++){
        pedras[i].player=1;
        pedras[i].dama=false;
        pedras[i].life=true;
    }
    for(i=12;i<24;i++){
        pedras[i].player=2;
        pedras[i].dama=false;
        pedras[i].life=true;
    }
    for (i=0;i<4;i++){
        pedras[i].x=1+2*i;
        pedras[i].y=7;
    }
    for (i=0;i<4;i++){
        pedras[i+4].x=2*i;
        pedras[i+4].y=6;
    }
    for (i=0;i<4;i++){
        pedras[i+8].x=1+2*i;
        pedras[i+8].y=5;
    }
    for (i=0;i<4;i++){
        pedras[i+12].x=2*i;
        pedras[i+12].y=2;
    }
    for (i=0;i<4;i++){
        pedras[i+16].x=1+2*i;
        pedras[i+16].y=1;
    }
    for (i=0;i<4;i++){
        pedras[i+20].x=2*i;
        pedras[i+20].y=0;
    }
}

//Function to print game Pieces
static void printPieces(){
    uint8_t i;
    for(i=0;i<24;i++){
        if(pedras[i].life==true){
            if(pedras[i].player==1)printP1(pedras[i].x,pedras[i].y);
            if(pedras[i].player==2)printP2(pedras[i].x,pedras[i].y);
        }
    }
}

// Functions to print player 1 and 2 pieces, and ther inverted colors
static void printP1(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6;
    Nokia5110_SetPxl(init_y+1,init_x+1);
    Nokia5110_SetPxl(init_y+1,init_x+4);
    Nokia5110_SetPxl(init_y+2,init_x+2);
    Nokia5110_SetPxl(init_y+2,init_x+3);
    Nokia5110_SetPxl(init_y+3,init_x+2);
    Nokia5110_SetPxl(init_y+3,init_x+3);
    Nokia5110_SetPxl(init_y+4,init_x+1);
    Nokia5110_SetPxl(init_y+4,init_x+4);
}
static void printP1Inv(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6, i,j;
    for(i=init_y;i<init_y+6;i++){
        for(j=init_x;j<init_x+6;j++){
            Nokia5110_SetPxl(i,j);
        }
    }
    Nokia5110_ClrPxl(init_y+1,init_x+1);
    Nokia5110_ClrPxl(init_y+1,init_x+4);
    Nokia5110_ClrPxl(init_y+2,init_x+2);
    Nokia5110_ClrPxl(init_y+2,init_x+3);
    Nokia5110_ClrPxl(init_y+3,init_x+2);
    Nokia5110_ClrPxl(init_y+3,init_x+3);
    Nokia5110_ClrPxl(init_y+4,init_x+1);
    Nokia5110_ClrPxl(init_y+4,init_x+4);
}

static void printP2(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6;
    Nokia5110_SetPxl(init_y+1,init_x+2);
    Nokia5110_SetPxl(init_y+1,init_x+3);
    Nokia5110_SetPxl(init_y+2,init_x+1);
    Nokia5110_SetPxl(init_y+2,init_x+2);
    Nokia5110_SetPxl(init_y+2,init_x+3);
    Nokia5110_SetPxl(init_y+2,init_x+4);
    Nokia5110_SetPxl(init_y+3,init_x+1);
    Nokia5110_SetPxl(init_y+3,init_x+2);
    Nokia5110_SetPxl(init_y+3,init_x+3);
    Nokia5110_SetPxl(init_y+3,init_x+4);
    Nokia5110_SetPxl(init_y+4,init_x+2);
    Nokia5110_SetPxl(init_y+4,init_x+3);
}
static void printP2Inv(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6,i,j;
    for(i=init_y;i<init_y+6;i++){
        for(j=init_x;j<init_x+6;j++){
            Nokia5110_SetPxl(i,j);
        }
    }
    Nokia5110_ClrPxl(init_y+1,init_x+2);
    Nokia5110_ClrPxl(init_y+1,init_x+3);
    Nokia5110_ClrPxl(init_y+2,init_x+1);
    Nokia5110_ClrPxl(init_y+2,init_x+2);
    Nokia5110_ClrPxl(init_y+2,init_x+3);
    Nokia5110_ClrPxl(init_y+2,init_x+4);
    Nokia5110_ClrPxl(init_y+3,init_x+1);
    Nokia5110_ClrPxl(init_y+3,init_x+2);
    Nokia5110_ClrPxl(init_y+3,init_x+3);
    Nokia5110_ClrPxl(init_y+3,init_x+4);
    Nokia5110_ClrPxl(init_y+4,init_x+2);
    Nokia5110_ClrPxl(init_y+4,init_x+3);
}

// Print functions to use with cursor
static void printFull(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6,i,j;
    for(i=init_y;i<init_y+6;i++){
        for(j=init_x;j<init_x+6;j++){
            Nokia5110_SetPxl(i,j);
        }
    }
}
static void printBlank(uint8_t x, uint8_t y){
    uint8_t init_x=36+x*6, init_y=y*6,i,j;
    for(i=init_y;i<init_y+6;i++){
        for(j=init_x;j<init_x+6;j++){
            Nokia5110_ClrPxl(i,j);
        }
    }
}


// Function to print 
static void printJogo(){
    static int vez=0, cont=0;
    Nokia5110_ClearBuffer();
    if (G_player==1){
        Nokia5110_BufferFullImage(JogoPlayer1);
    }else if (G_player==2){
        Nokia5110_BufferFullImage(JogoPlayer2);
    }
    printPieces();
    int i,p1=0,p2=0;
    for(i=0;i<12;i++){
        if(pedras[i].life==false)p1++;
    }
    for(i=12;i<24;i++){
        if(pedras[i].life==false)p2++;
    }
    for(i=0;i<p2;i++){
        if(i<6)Nokia5110_ClrPxl(35,10+(2*i));
        else Nokia5110_ClrPxl(33,10+(2*i-12));
    }
    for(i=0;i<p1;i++){
        if(i<6)Nokia5110_ClrPxl(11,20-(2*i));
        else Nokia5110_ClrPxl(13,20-(2*i-12));
    }
    int aux = checkSpace(G_cursor_x,G_cursor_y);
    switch(aux){
        case 0:
            if(vez==0)printFull(G_cursor_x,G_cursor_y);
            else if(vez==1)printBlank(G_cursor_x,G_cursor_y);
            break;
        case 1:
            if(vez==0)printP1(G_cursor_x,G_cursor_y);
            else if(vez==1)printP1Inv(G_cursor_x,G_cursor_y);
            break;
        case 2:
            if(vez==0)printP2(G_cursor_x,G_cursor_y);
            else if(vez==1)printP2Inv(G_cursor_x,G_cursor_y);
            break;
    }
    if(G_selec_x!=-1 && G_selec_y!=-1){
       switch(G_player){
        case 1:
            if(vez==0)printP1(G_selec_x,G_selec_y);
            else if(vez==1)printP1Inv(G_selec_x,G_selec_y);
            break;
        case 2:
            if(vez==0)printP2(G_selec_x,G_selec_y);
            else if(vez==1)printP2Inv(G_selec_x,G_selec_y);
            break;
        }
    }
    if(cont==0){
        cont=2;
        if(vez==0)vez=1;
        else if(vez==1)vez=0;
    }else cont--;
    Nokia5110_DisplayBuffer();
}


// Function to know if cursor is at a blank or occupied space
// Returns the player number if at a piece, 0 if at a blank space
// Returns -1 if out of limits
static int checkSpace(int x,int y){
    if(x<0 || x>7)return -1;
    if(y<0 || y>7)return -1;
    uint8_t i;
    for(i=0;i<24;i++)if(pedras[i].x==x && pedras[i].y==y)return pedras[i].player;
    return 0;
}

// Function to check if a move is possible or not
// Returns 2 if move was possible, was made and other move is possible
// Returns 1 if move was possible, was made and no other move is possible
// Returns 0 if move was not possible to be made
static int checkMove(){
    if(checkSpace(G_cursor_x,G_cursor_y)!=0)return 0;
    int i,diag, distancia=0;
    for(i=1; i<8;i++){
        if(G_selec_x+i==G_cursor_x && G_selec_y+i==G_cursor_y)diag=1;
        if(G_selec_x-i==G_cursor_x && G_selec_y+i==G_cursor_y)diag=2;
        if(G_selec_x+i==G_cursor_x && G_selec_y-i==G_cursor_y)diag=3;
        if(G_selec_x-i==G_cursor_x && G_selec_y-i==G_cursor_y)diag=4;
        distancia++;
        if(diag!=0)break;
    }
    /*
            4       3
                0
            2       1
    */
    if(diag==0)return 0;
    for(i=0;i<24;i++){
        if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y)break;
    }
    switch(pedras[i].dama){
        case true:

        case false:
            if(distancia>2)return 0;
            if(distancia==1){
                if(G_player==1){
                    if(diag==2 || diag==1)return 0;
                    pedras[i].x=G_cursor_x;
                    pedras[i].y=G_cursor_y;
                    return 1;
                }else{
                    if(diag==4 || diag==3)return 0;
                    pedras[i].x=G_cursor_x;
                    pedras[i].y=G_cursor_y;
                    return 1;
                }
            }
            if(distancia==2){
                if(G_player==1){
                   switch(diag){
                       case 1:
                           if(checkSpace(G_selec_x+1,G_selec_y+1)==2 && checkSpace(G_selec_x+2,G_selec_y+2)==0){
                               for(i=0;i<24;i++){
                                   if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                      pedras[i].x=G_cursor_x;
                                      pedras[i].y=G_cursor_y;
                                  }
                                   if(pedras[i].x==G_selec_x+1 && pedras[i].y==G_selec_y+1){
                                       pedras[i].x=10;
                                       pedras[i].y=10;
                                       pedras[i].life=false;
                                   }
                               }
                               return 1;
                           }
                       case 2:
                          if(checkSpace(G_selec_x-1,G_selec_y+1)==2 && checkSpace(G_selec_x-2,G_selec_y+2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                        pedras[i].x=G_cursor_x;
                                        pedras[i].y=G_cursor_y;
                                    }
                                  if(pedras[i].x==G_selec_x-1 && pedras[i].y==G_selec_y+1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                  }
                              }
                              return 1;
                          }
                       case 3:
                          if(checkSpace(G_selec_x+1,G_selec_y-1)==2 && checkSpace(G_selec_x+2,G_selec_y-2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                    pedras[i].x=G_cursor_x;
                                    pedras[i].y=G_cursor_y;
                                }
                                  if(pedras[i].x==G_selec_x+1 && pedras[i].y==G_selec_y-1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                  }
                              }
                              return 1;
                          }
                       case 4:
                          if(checkSpace(G_selec_x-1,G_selec_y-1)==2 && checkSpace(G_selec_x-2,G_selec_y-2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                        pedras[i].x=G_cursor_x;
                                        pedras[i].y=G_cursor_y;
                                    }
                                  if(pedras[i].x==G_selec_x-1 && pedras[i].y==G_selec_y-1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                  }
                              }
                              return 1;
                          }
                   }
                }else{
                    switch(diag){
                       case 1:
                           if(checkSpace(G_selec_x+1,G_selec_y+1)==1 && checkSpace(G_selec_x+2,G_selec_y+2)==0){
                               for(i=0;i<24;i++){
                                   if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                         pedras[i].x=G_cursor_x;
                                         pedras[i].y=G_cursor_y;
                                     }
                                   if(pedras[i].x==G_selec_x+1 && pedras[i].y==G_selec_y+1){
                                       pedras[i].x=10;
                                       pedras[i].y=10;
                                       pedras[i].life=false;
                                   }
                               }
                               return 1;
                           }
                       case 2:
                          if(checkSpace(G_selec_x-1,G_selec_y+1)==1 && checkSpace(G_selec_x-2,G_selec_y+2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x-1 && pedras[i].y==G_selec_y+1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                      return 1;
                                  }
                              }
                          }
                       case 3:
                          if(checkSpace(G_selec_x+1,G_selec_y-1)==1 && checkSpace(G_selec_x+2,G_selec_y-2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                    pedras[i].x=G_cursor_x;
                                    pedras[i].y=G_cursor_y;
                                }
                                  if(pedras[i].x==G_selec_x+1 && pedras[i].y==G_selec_y-1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                  }
                              }
                              return 1;
                          }
                       case 4:
                          if(checkSpace(G_selec_x-1,G_selec_y-1)==1 && checkSpace(G_selec_x-2,G_selec_y-2)==0){
                              for(i=0;i<24;i++){
                                  if(pedras[i].x==G_selec_x && pedras[i].y==G_selec_y){
                                    pedras[i].x=G_cursor_x;
                                    pedras[i].y=G_cursor_y;
                                }
                                  if(pedras[i].x==G_selec_x-1 && pedras[i].y==G_selec_y-1){
                                      pedras[i].x=10;
                                      pedras[i].y=10;
                                      pedras[i].life=false;
                                      return 1;
                                  }
                              }
                              return 1;
                          }
                   }
                }
            }
    }
    return 0;
}

// Function to check if any piece can capture
// Returns true if possible, false if not possible
static bool canCapture(){
    int i,j;
    if(G_player==1){
        for(i=0;i<12;i++){
            if(pedras[i].life==true){
                switch(pedras[i].dama){
                    case true:
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x+j,pedras[i].y+j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x+j,pedras[i].y+j)==2 && checkSpace(pedras[i].x+j+1,pedras[i].y+j+1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x-j,pedras[i].y-j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x-j,pedras[i].y-j)==2 && checkSpace(pedras[i].x-j-1,pedras[i].y-j-1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x+j,pedras[i].y-j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x+j,pedras[i].y-j)==2 && checkSpace(pedras[i].x+j+1,pedras[i].y-j-1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x-j,pedras[i].y+j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x-j,pedras[i].y+j)==2 && checkSpace(pedras[i].x-j-1,pedras[i].y+j+1)==0)return true;
                    case false:
                        if(checkSpace(pedras[i].x+1,pedras[i].y+1)==2 && checkSpace(pedras[i].x+2,pedras[i].y+2)==0)return true;
                        if(checkSpace(pedras[i].x-1,pedras[i].y-1)==2 && checkSpace(pedras[i].x-2,pedras[i].y-2)==0)return true;
                        if(checkSpace(pedras[i].x+1,pedras[i].y-1)==2 && checkSpace(pedras[i].x+2,pedras[i].y-2)==0)return true;
                        if(checkSpace(pedras[i].x-1,pedras[i].y+1)==2 && checkSpace(pedras[i].x-2,pedras[i].y+2)==0)return true;
                }
            }
        }
    }
    if(G_player==2){
        for(i=12;i<24;i++){
            if(pedras[i].life==true){
                switch(pedras[i].dama){
                    case true:
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x+j,pedras[i].y+j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x+j,pedras[i].y+j)==1 && checkSpace(pedras[i].x+j+1,pedras[i].y+j+1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x-j,pedras[i].y-j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x-j,pedras[i].y-j)==1 && checkSpace(pedras[i].x-j-1,pedras[i].y-j-1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x+j,pedras[i].y-j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x+j,pedras[i].y-j)==1 && checkSpace(pedras[i].x+j+1,pedras[i].y-j-1)==0)return true;
                        for(j=1;j<8;j++){
                            if(checkSpace(pedras[i].x-j,pedras[i].y+j)!=0)break;
                        }
                        if(checkSpace(pedras[i].x-j,pedras[i].y+j)==1 && checkSpace(pedras[i].x-j-1,pedras[i].y+j+1)==0)return true;
                    case false:
                        if(checkSpace(pedras[i].x+1,pedras[i].y+1)==1 && checkSpace(pedras[i].x+2,pedras[i].y+2)==0)return true;
                        if(checkSpace(pedras[i].x-1,pedras[i].y-1)==1 && checkSpace(pedras[i].x-2,pedras[i].y-2)==0)return true;
                        if(checkSpace(pedras[i].x+1,pedras[i].y-1)==1 && checkSpace(pedras[i].x+2,pedras[i].y-2)==0)return true;
                        if(checkSpace(pedras[i].x-1,pedras[i].y+1)==1 && checkSpace(pedras[i].x-2,pedras[i].y+2)==0)return true;
                }
            }
        }
    }
    return false;
}

/*
    To do:

    fazer animacoes de entrada e movimento
    fazer diferenciacao entre dama e pedra normal (printJogo)
    fazer telas e funcoes relacionadas a tela de vitoria


*/
