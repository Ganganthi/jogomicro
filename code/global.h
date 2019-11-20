#include <stdint.h>
#ifdef  MAIN_FILE
volatile uint8_t G_posmenu, G_modo, G_linhaInst, G_cursor_x, G_cursor_y, G_vit, G_player, G_mux, debouncer;
volatile int G_selec_x, G_selec_y;
#else
extern volatile uint8_t G_posmenu, G_modo, G_linhaInst, G_cursor_x, G_cursor_y, G_vit, G_player, G_mux, debouncer;
extern volatile int G_selec_x, G_selec_y;
#endif
/*

G_posmenu		posicao do cursor no menu
(0 aponta para jogo, 1 aponta para instrucao)

G_modo			modo de jogo operante
(MODO_MENU,MODO_JOGO,MODO_INST,MODO_VITO)

G_linhaInst		qual tela de instrucao esta aparecendo
(minimo de 0, maximo de INST_MAX)

G_cursor_x		posicao x do cursor durante o jogo
(minimo de 0, maximo de 7)

G_cursor_y		posicao y do cursor durante o jogo
(minimo de 0, maximo de 7)

G_vit			armazena qual jogador venceu para imprimir em tela	
(0 sem vitoria, 1 para vitoria do jogador 1, 2 para vitoria do jogador 2)

G_player		armazena qual jogador está jogando
(1 para player 1, 2 para player 2)

G_selec_x		posicao da casa em x selecionada	
(-1 para nao selecionada, valor de 0-7 para a casa selecionada)

G_selec_y		posicao da casa em y selecionada
(-1 para nao selecionada, valor de 0-7 para a casa selecionada)

G_mux			armazena qual linha da matriz esta sendo multiplexada
(1-3 para numero da linha)



*/
