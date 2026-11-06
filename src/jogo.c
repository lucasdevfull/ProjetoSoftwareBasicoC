/*
 * jogo.c - Regras de uma partida (veja jogo.h).
 */

#include "jogo.h"

#include <stdlib.h>

/* Sorteia uma casa que a cobra não ocupa. */
static Posicao sortear_casa_livre(const Jogo *j)
{
    Posicao p;

    do {
        p.x = rand() % j->grade;
        p.y = rand() % j->grade;
    } while (cobra_ocupa(&j->cobra, p));
    return p;
}

static bool fora_da_grade(const Jogo *j, Posicao p)
{
    return p.x < 0 || p.x >= j->grade || p.y < 0 || p.y >= j->grade;
}

static void encerrar(Jogo *j, Estado novo_estado)
{
    j->estado = novo_estado;
    j->fim    = time(NULL);
}

/* A cada COMIDAS_PARA_ACELERAR comidas o intervalo diminui, até o mínimo. */
static void acelerar(Jogo *j)
{
    if (j->comidas % COMIDAS_PARA_ACELERAR != 0)
        return;
    j->intervalo -= ACELERACAO_MS;
    if (j->intervalo < INTERVALO_MINIMO)
        j->intervalo = INTERVALO_MINIMO;
}

void jogo_iniciar(Jogo *j, const Config *cfg)
{
    j->grade             = cfg->grade;
    j->pontos_por_comida = cfg->pontos_por_comida;
    j->intervalo_inicial = cfg->velocidade;
    jogo_reiniciar(j);
}

void jogo_reiniciar(Jogo *j)
{
    Posicao centro;

    centro.x = j->grade / 2;                      /* cobra no centro, andando para a direita */
    centro.y = j->grade / 2;
    cobra_iniciar(&j->cobra, centro, TAMANHO_INICIAL);

    j->intervalo = j->intervalo_inicial;
    j->pontos    = 0;
    j->comidas   = 0;
    j->estado    = JOGANDO;
    j->inicio    = time(NULL);
    j->fim       = 0;
    j->comida    = sortear_casa_livre(j);
}

void jogo_passo(Jogo *j)
{
    Posicao nova;
    bool    comeu;

    if (j->estado != JOGANDO)
        return;

    nova = cobra_proxima_cabeca(&j->cobra);

    /* 1) bateu na parede? */
    if (fora_da_grade(j, nova)) {
        encerrar(j, FIM_DE_JOGO);
        return;
    }

    /* 2) bateu no próprio corpo? */
    comeu = (nova.x == j->comida.x && nova.y == j->comida.y);
    if (cobra_bate_no_corpo(&j->cobra, nova, comeu)) {
        encerrar(j, FIM_DE_JOGO);
        return;
    }

    /* 3) anda (e cresce, se comeu) */
    cobra_move(&j->cobra, comeu);
    if (!comeu)
        return;

    /* 4) pontuação, aceleração e nova comida */
    j->pontos += j->pontos_por_comida;
    j->comidas++;
    acelerar(j);

    if (j->cobra.tamanho == j->grade * j->grade)
        encerrar(j, VITORIA);                     /* a cobra ocupou o tabuleiro inteiro */
    else
        j->comida = sortear_casa_livre(j);
}

void jogo_mudar_direcao(Jogo *j, Direcao d)
{
    if (j->estado == JOGANDO)
        cobra_pedir_direcao(&j->cobra, d);
}

void jogo_alternar_pausa(Jogo *j)
{
    if (j->estado == JOGANDO)
        j->estado = PAUSADO;
    else if (j->estado == PAUSADO)
        j->estado = JOGANDO;
}

void jogo_abandonar(Jogo *j)
{
    if (!jogo_terminou(j))
        encerrar(j, FIM_DE_JOGO);
}

bool jogo_terminou(const Jogo *j)
{
    return j->estado == FIM_DE_JOGO || j->estado == VITORIA;
}

int jogo_duracao(const Jogo *j)
{
    time_t fim = (j->fim != 0) ? j->fim : time(NULL);
    return (int)difftime(fim, j->inicio);
}
