/*
 * historico.c - Gravação e leitura do histórico de partidas (arquivo texto).
 * O formato do arquivo está descrito em historico.h.
 */

#include "historico.h"

#include <stdio.h>
#include <string.h>

typedef struct {                  /* uma linha do arquivo */
    char data[32];
    char nome[TAM_NOME];
    int  pontos;
    int  comidas;
    int  duracao;                 /* em segundos */
} Partida;

bool historico_salvar(const char *nome, time_t quando, int pontos, int comidas, int duracao)
{
    char data[32];
    char limpo[TAM_NOME];
    size_t i;
    FILE *arq;

    /* Copia o nome (no máximo TAM_NOME - 1 caracteres) trocando por espaço o que
       quebraria o formato do arquivo: ';' separa os campos e '\n' separa as linhas. */
    for (i = 0; i < TAM_NOME - 1 && nome[i] != '\0'; i++)
        limpo[i] = (nome[i] == ';' || nome[i] == '\n' || nome[i] == '\r') ? ' ' : nome[i];
    limpo[i] = '\0';

    strftime(data, sizeof data, "%d/%m/%Y %H:%M:%S", localtime(&quando));

    arq = fopen(ARQUIVO_HISTORICO, "a");             /* "a" = adiciona no final */
    if (arq == NULL)
        return false;

    fprintf(arq, "%s;%s;%d;%d;%d\n", data, limpo, pontos, comidas, duracao);
    fclose(arq);
    return true;
}

void historico_mostrar(void)
{
    char linha[256];
    Partida p, melhor;
    int total = 0;
    long soma_pontos = 0;
    long soma_comidas = 0;
    FILE *arq = fopen(ARQUIVO_HISTORICO, "r");

    if (arq == NULL) {
        printf("\nNenhuma partida registrada ainda.\n");
        return;
    }

    memset(&melhor, 0, sizeof melhor);
    melhor.pontos = -1;

    while (fgets(linha, sizeof linha, arq) != NULL) {
        /* 31 = sizeof(p.data) - 1  e  20 = TAM_NOME - 1 */
        if (sscanf(linha, "%31[^;];%20[^;];%d;%d;%d",
                   p.data, p.nome, &p.pontos, &p.comidas, &p.duracao) != 5)
            continue;                                /* ignora linhas inválidas */

        if (total == 0) {
            printf("\n%-4s %-19s %-20s %7s %8s %7s\n",
                   "#", "Data/hora", "Jogador", "Pontos", "Comidas", "Tempo");
            printf("----------------------------------------------------------------------\n");
        }
        total++;
        soma_pontos  += p.pontos;
        soma_comidas += p.comidas;
        printf("%-4d %-19s %-20s %7d %8d %6ds\n",
               total, p.data, p.nome, p.pontos, p.comidas, p.duracao);

        if (p.pontos > melhor.pontos)
            melhor = p;
    }
    fclose(arq);

    if (total == 0) {
        printf("\nNenhuma partida registrada ainda.\n");
        return;
    }

    printf("\nTotal de partidas: %d\n", total);
    printf("Media: %.1f pontos e %.1f comidas por partida\n",
           (double)soma_pontos / total, (double)soma_comidas / total);
    printf("Melhor resultado: %d pontos e %d comidas (%s, em %s)\n",
           melhor.pontos, melhor.comidas, melhor.nome, melhor.data);
}
