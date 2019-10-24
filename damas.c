#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct jogo_peca
{
	unsigned int cor;
	unsigned int linha;
	unsigned int coluna;
}peca;

peca *peca_cria(unsigned int c, unsigned int lin, unsigned int col){
	peca *ret=(peca*)malloc(sizeof(peca));
	if(!ret) return NULL;
	ret->cor=c;
	ret->linha=lin;
	ret->coluna=col;
	return ret;
}

void peca_destroi(peca **certo){
	if(!(*certo)) return;
	if(!certo) return;
	free(*certo);
	*certo=NULL;
	return;
}


int main(int argc, char const *argv[])
{
	
	return 0;
}
