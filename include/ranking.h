/*
 * ranking.h - Ranking dos 5 melhores, gravado em arquivo de texto.
 * A implementação está em ranking.c.
 *
 * Formato do arquivo (uma linha por posição, do 1º ao 5º):
 *     nome;pontos
 *     Ana;120
 */
#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_RANKING  "ranking.txt"   /* criado na pasta onde o programa roda */
#define RANKING_MAX      5
#define TAM_NOME         21              /* nome do jogador: 20 caracteres + '\0' */

typedef struct {
    char nome[TAM_NOME];
    int  pontos;
} ItemRanking;

typedef struct {
    ItemRanking itens[RANKING_MAX];      /* em ordem: itens[0] é o 1º lugar */
    int         quantidade;              /* quantos itens estão preenchidos */
} Ranking;

/* Lê o arquivo. Se ele não existir, o ranking começa vazio.
   Retorna true se conseguiu abrir o arquivo e false se não. */
bool ranking_carregar(Ranking *r, const char *arquivo);

/* Insere o resultado na posição certa (maior pontuação primeiro).
   Retorna a posição (1 a RANKING_MAX) ou 0 se não entrou no ranking. */
int ranking_inserir(Ranking *r, const char *nome, int pontos);

/* Grava o ranking no arquivo. Retorna true se gravou e false se não conseguiu. */
bool ranking_salvar(const Ranking *r, const char *arquivo);

/* Mostra o ranking no console. */
void ranking_mostrar(const Ranking *r);

#endif /* RANKING_H */
