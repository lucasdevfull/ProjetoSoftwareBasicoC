/*
 * render.h - Janela (freeglut), desenho com quadrados (OpenGL) e eventos
 *            de teclado e mouse. A implementação está em render.c.
 *
 * Na janela:  setas = mover | P = pausar | ESC = sair
 *             clique esquerdo depois do fim = jogar de novo
 */
#ifndef RENDER_H
#define RENDER_H

#include "config.h"
#include "jogo.h"

/* Função chamada sempre que uma partida termina (inclusive por ESC ou
   janela fechada). `contexto` é o mesmo ponteiro passado a render_jogar. */
typedef void (*AoTerminarPartida)(const Jogo *jogo, void *contexto);

/* Abre a janela de 600 x 600 e roda partidas com o `jogo` já iniciado
   (jogo_iniciar). Só retorna quando o jogador aperta ESC ou fecha a janela. */
void render_jogar(Jogo *jogo, CorCobra cor, AoTerminarPartida ao_terminar, void *contexto);

#endif /* RENDER_H */
