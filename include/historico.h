/*
 * historico.h - Histórico de partidas gravado em arquivo de texto (extensão C).
 * A implementação está em historico.c.
 *
 * Formato do arquivo (uma partida por linha):
 *     data;jogador;pontos;comidas;duracao
 *     03/11/2026 15:30:12;Ana;120;8;48
 */
#ifndef HISTORICO_H
#define HISTORICO_H

#include <time.h>

#include "ranking.h"                         /* TAM_NOME */

#define ARQUIVO_HISTORICO  "historico.txt"   /* criado na pasta onde o programa roda */

/* Acrescenta uma partida ao final do arquivo.
   `quando` é a hora em que a partida terminou e `duracao` está em segundos.
   Retorna true se gravou e false se não conseguiu abrir o arquivo. */
bool historico_salvar(const char *nome, time_t quando, int pontos, int comidas, int duracao);

/* Lê o arquivo e mostra no console a tabela de partidas, o total,
   a média de pontos e de comidas, e o melhor resultado. */
void historico_mostrar(void);

#endif /* HISTORICO_H */
