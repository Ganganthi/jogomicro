#include "bitmaps.h"
#include "Buttons.h"
#include "Nokia5110.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
//------------Initial config------------
    Nokia5110_Init();
    Nokia5110_Clear();
    SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ConfigureButtons();
    Nokia5110_Clear();
    while(1){
        Nokia5110_Clear();
        SysCtlDelay((SysCtlClockGet())/200);
        Nokia5110_DrawFullImageInv(load_screen);//tela toda
        
        //Nokia5110_MyDrawing(doublee,3,1,2,16);// bonequinho
        
        //Nokia5110_SetCursor(0,0);//posicona o curso para onde vai comecar a escrever a string
        //Nokia5110_OutString("bla");//string
    }
}
