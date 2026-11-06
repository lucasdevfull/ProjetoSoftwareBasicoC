/*
 * ranking.c - Ranking dos 5 melhores (veja ranking.h).
 */

#include "ranking.h"

#include <stdio.h>
#include <string.h>

/* Copia o nome (no máximo TAM_NOME - 1 caracteres) trocando por espaço o que
   quebraria o formato do arquivo: ';' separa os campos e '\n' separa as linhas. */
static void copiar_nome(char *destino, const char *nome)
{
    size_t i;

    for (i = 0; i < TAM_NOME - 1 && nome[i] != '\0'; i++)
        destino[i] = (nome[i] == ';' || nome[i] == '\n' || nome[i] == '\r') ? ' ' : nome[i];
    destino[i] = '\0';
}

bool ranking_carregar(Ranking *r, const char *arquivo)
{
    char  linha[128];
    char  nome[TAM_NOME];
    int   pontos;
    FILE *arq;

    r->quantidade = 0;

    arq = fopen(arquivo, "r");
    if (arq == NULL)
        return false;                             /* ainda não existe: ranking vazio */

    while (fgets(linha, sizeof linha, arq) != NULL) {
        /* 20 = TAM_NOME - 1 */
        if (sscanf(linha, "%20[^;];%d", nome, &pontos) != 2 || pontos < 0)
            continue;                             /* ignora linhas inválidas */
        ranking_inserir(r, nome, pontos);         /* garante a ordem mesmo se o arquivo foi editado */
    }
    fclose(arq);
    return true;
}

int ranking_inserir(Ranking *r, const char *nome, int pontos)
{
    int pos = 0;
    int i;

    /* procura a primeira posição com pontuação menor (empate fica atrás de quem já estava) */
    while (pos < r->quantidade && r->itens[pos].pontos >= pontos)
        pos++;
    if (pos >= RANKING_MAX)
        return 0;                                 /* não entrou */

    if (r->quantidade < RANKING_MAX)
        r->quantidade++;

    /* empurra para baixo quem está da posição em diante (o último sai se estiver cheio) */
    for (i = r->quantidade - 1; i > pos; i--)
        r->itens[i] = r->itens[i - 1];

    copiar_nome(r->itens[pos].nome, nome);
    r->itens[pos].pontos = pontos;
    return pos + 1;
}

bool ranking_salvar(const Ranking *r, const char *arquivo)
{
    int   i;
    FILE *arq = fopen(arquivo, "w");              /* "w" = reescreve o arquivo inteiro */

    if (arq == NULL)
        return false;

    for (i = 0; i < r->quantidade; i++)
        fprintf(arq, "%s;%d\n", r->itens[i].nome, r->itens[i].pontos);
    fclose(arq);
    return true;
}

void ranking_mostrar(const Ranking *r)
{
    int i;

    printf("\n=========== RANKING ===========\n");
    if (r->quantidade == 0) {
        printf(" Nenhuma partida no ranking ainda.\n");
    } else {
        printf(" %-3s %-20s %6s\n", "#", "Jogador", "Pontos");
        for (i = 0; i < r->quantidade; i++)
            printf(" %-3d %-20s %6d\n", i + 1, r->itens[i].nome, r->itens[i].pontos);
    }
    printf("===============================\n");
}
