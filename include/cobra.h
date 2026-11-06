/*
 * cobra.h - A cobra: struct e funções que trabalham com ela.
 * Só lógica, sem nada de janela ou OpenGL (dá para testar sem abrir a janela).
 * A implementação está em cobra.c.
 */
#ifndef COBRA_H
#define COBRA_H

#define GRADE_MAX            40                        /* maior grade aceita (casas por lado) */
#define COBRA_MAX_SEGMENTOS  (GRADE_MAX * GRADE_MAX)   /* cobra do tamanho do tabuleiro todo  */

typedef struct { int x, y; } Posicao;                  /* posição na grade (não em pixels) */

typedef enum { CIMA, BAIXO, ESQUERDA, DIREITA } Direcao;

typedef struct {
    Posicao corpo[COBRA_MAX_SEGMENTOS];   /* corpo[0] é a cabeça                   */
    int     tamanho;                      /* quantos segmentos estão em uso        */
    Direcao direcao;                      /* direção do último movimento realizado */
    Direcao proxima;                      /* direção pedida pelo jogador           */
} Cobra;

/* Coloca a cobra com `tamanho` segmentos na horizontal, com a cabeça em
   `cabeca`, andando para a direita. */
void cobra_iniciar(Cobra *c, Posicao cabeca, int tamanho);

/* Pede uma nova direção. Meia-volta (para dentro do próprio corpo) é ignorada. */
void cobra_pedir_direcao(Cobra *c, Direcao nova);

/* Onde a cabeça vai estar depois do próximo passo. */
Posicao cobra_proxima_cabeca(const Cobra *c);

/* Retorna true se algum segmento da cobra está na casa `p`. */
bool cobra_ocupa(const Cobra *c, Posicao p);

/* Retorna true se a cabeça em `nova` bate no próprio corpo.
   Se a cobra não vai crescer, a ponta da cauda sai do lugar neste passo
   e por isso não conta como obstáculo. */
bool cobra_bate_no_corpo(const Cobra *c, Posicao nova, bool cresce);

/* Anda uma casa na direção pedida. Se `cresce` for true, ganha um segmento. */
void cobra_move(Cobra *c, bool cresce);

#endif /* COBRA_H */
