/*
 * cobra.h - Jogo da cobrinha (lógica, desenho OpenGL e janela freeglut).
 * A implementação está em cobra.c.
 */
#ifndef COBRA_H
#define COBRA_H

#include <time.h>

/* Dados da partida que acabou de terminar. */
typedef struct {
    int    pontos;
    int    tamanho;    /* tamanho final da cobra, em quadrados */
    int    duracao;    /* em segundos                          */
    time_t fim;        /* hora em que a partida terminou       */
} ResultadoPartida;

/* Abre a janela do jogo (600 x 600) e roda uma partida.
   Só retorna quando a partida acabar: ENTER na tela de fim de jogo,
   ESC durante o jogo ou janela fechada. */
ResultadoPartida cobra_jogar(void);

#endif /* COBRA_H */
