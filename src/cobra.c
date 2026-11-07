#include "cobra.h"
#include <string.h>

static Direcao direcao_oposta(Direcao d)
{
    switch (d) {
        case CIMA:     return BAIXO;
        case BAIXO:    return CIMA;
        case ESQUERDA: return DIREITA;
        default:       return ESQUERDA;
    }
}

void cobra_iniciar(Cobra *c, Posicao cabeca, int tamanho)
{
    int i;

    for (i = 0; i < tamanho; i++) {
        c->corpo[i].x = cabeca.x - i;
        c->corpo[i].y = cabeca.y;
    }
    c->tamanho = tamanho;
    c->direcao = DIREITA;
    c->proxima = DIREITA;
}

void cobra_pedir_direcao(Cobra *c, Direcao nova)
{
    /* Compara com a direção do último movimento feito, e não com "proxima",
       para que dois toques rápidos dentro do mesmo passo não burlem a regra. */
    if (nova != direcao_oposta(c->direcao))
        c->proxima = nova;
}

Posicao cobra_proxima_cabeca(const Cobra *c)
{
    static const int DX[] = { 0,  0, -1, 1 };    /* CIMA, BAIXO, ESQUERDA, DIREITA */
    static const int DY[] = { 1, -1,  0, 0 };    /* no OpenGL o eixo Y cresce para cima */
    Posicao p;

    p.x = c->corpo[0].x + DX[c->proxima];
    p.y = c->corpo[0].y + DY[c->proxima];
    return p;
}

bool cobra_ocupa(const Cobra *c, Posicao p)
{
    int i;

    for (i = 0; i < c->tamanho; i++)
        if (c->corpo[i].x == p.x && c->corpo[i].y == p.y)
            return true;
    return false;
}

bool cobra_bate_no_corpo(const Cobra *c, Posicao nova, bool cresce)
{
    int i;
    int limite = cresce ? c->tamanho : c->tamanho - 1;

    for (i = 0; i < limite; i++)
        if (c->corpo[i].x == nova.x && c->corpo[i].y == nova.y)
            return true;
    return false;
}

void cobra_move(Cobra *c, bool cresce)
{
    Posicao nova = cobra_proxima_cabeca(c);

    if (cresce && c->tamanho < COBRA_MAX_SEGMENTOS)
        c->tamanho++;                             /* ganha mais um segmento no final */

    /* empurra o corpo uma posição para trás e coloca a nova cabeça na frente */
    memmove(&c->corpo[1], &c->corpo[0], (size_t)(c->tamanho - 1) * sizeof(Posicao));
    c->corpo[0] = nova;
    c->direcao  = c->proxima;
}
