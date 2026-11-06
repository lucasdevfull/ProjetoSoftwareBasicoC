/*
 * jogo.h - Regras de uma partida: comida, paredes, pontuação e aceleração.
 * Só lógica, sem janela. A implementação está em jogo.c.
 */
#ifndef JOGO_H
#define JOGO_H

#include <time.h>

#include "cobra.h"
#include "config.h"

#define TAMANHO_INICIAL        3
#define COMIDAS_PARA_ACELERAR  5     /* a cada 5 comidas...               */
#define ACELERACAO_MS          10    /* ...o intervalo diminui 10 ms...   */
#define INTERVALO_MINIMO       50    /* ...até no mínimo 50 ms            */

typedef enum { JOGANDO, PAUSADO, FIM_DE_JOGO, VITORIA } Estado;

typedef struct {
    Cobra   cobra;
    Posicao comida;
    int     grade;               /* casas por lado                        */
    int     pontos_por_comida;
    int     intervalo_inicial;   /* ms entre passos no começo da partida  */
    int     intervalo;           /* ms entre passos agora                 */
    int     pontos;
    int     comidas;             /* quantas comidas a cobra já comeu      */
    Estado  estado;
    time_t  inicio;              /* quando a partida começou              */
    time_t  fim;                 /* quando terminou (0 = em andamento)    */
} Jogo;

/* Guarda os valores da configuração e começa a primeira partida. */
void jogo_iniciar(Jogo *j, const Config *cfg);

/* Começa uma partida nova com a mesma configuração. */
void jogo_reiniciar(Jogo *j);

/* Um passo do jogo: anda a cobra e aplica as regras (só se estiver JOGANDO). */
void jogo_passo(Jogo *j);

/* Repassa a direção pedida para a cobra (só se estiver JOGANDO). */
void jogo_mudar_direcao(Jogo *j, Direcao d);

/* P: pausa ou continua. */
void jogo_alternar_pausa(Jogo *j);

/* Encerra uma partida que ainda não acabou (ESC ou janela fechada). */
void jogo_abandonar(Jogo *j);

/* Retorna true se a partida acabou (fim de jogo ou vitória). */
bool jogo_terminou(const Jogo *j);

/* Duração da partida em segundos. */
int jogo_duracao(const Jogo *j);

#endif /* JOGO_H */
