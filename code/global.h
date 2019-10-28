#include <stdint.h>
#ifdef  MAIN_FILE
volatile uint8_t G_posmenu, G_modo, G_linhaInst, G_cursor_x, G_cursor_y, G_vit, G_player, G_selec_x, G_selec_y;
#else
extern volatile uint8_t G_posmenu, G_modo, G_linhaInst, G_cursor_x, G_cursor_y, G_vit, G_player, G_selec_x, G_selec_y;
#endif
