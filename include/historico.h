/*
 * historico.h - Histórico de partidas gravado em arquivo de texto.
 * A implementação está em historico.c.
 *
 * Formato do arquivo (uma partida por linha):
 *     data;jogador;pontos;tamanho;duracao
 *     03/11/2026 15:30:12;Ana;120;15;48
 */
#ifndef HISTORICO_H
#define HISTORICO_H

#include <time.h>

#define ARQUIVO_HISTORICO  "historico.txt"   /* criado na pasta onde o programa roda */
#define TAM_NOME           21                /* nome do jogador: 20 caracteres + '\0' */

/* Acrescenta uma partida ao final do arquivo.
   `quando` é a hora em que a partida terminou e `duracao` está em segundos.
   Retorna 1 se gravou e 0 se não conseguiu abrir o arquivo. */
int historico_salvar(const char *nome, time_t quando, int pontos, int tamanho, int duracao);

/* Lê o arquivo e mostra no terminal a tabela de partidas,
   o total de partidas e a melhor pontuação. */
void historico_mostrar(void);

#endif /* HISTORICO_H */
