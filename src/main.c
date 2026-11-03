/*
 * main.c - Menu no terminal. Liga o jogo (cobra.c) ao histórico (historico.c).
 *
 * Os dois módulos não dependem um do outro: quem conversa com os dois é este
 * arquivo, que pega o resultado de cobra_jogar() e entrega ao historico_salvar().
 *
 * Compilar sem CMake:
 *   Linux:   gcc main.c cobra.c historico.c -o cobra -Wall -Wextra -lglut -lGL
 *   Windows: gcc main.c cobra.c historico.c -o cobra.exe -Wall -Wextra -lfreeglut -lopengl32
 */

#include <stdio.h>
#include <string.h>

#include "cobra.h"
#include "historico.h"

/* ====================== ENTRADA DO TERMINAL ====================== */

/* Lê uma linha do teclado sem o '\n' e descarta o que passar do tamanho do buffer.
   Retorna 0 se a entrada acabou (EOF). */
static int ler_linha(char *destino, size_t tamanho)
{
    size_t n;

    if (fgets(destino, (int)tamanho, stdin) == NULL)
        return 0;

    n = strlen(destino);
    if (n > 0 && destino[n - 1] == '\n') {
        destino[n - 1] = '\0';
    } else {
        int c;                                       /* linha maior que o buffer: joga o resto fora */
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    return 1;
}

static int ler_opcao(void)
{
    char linha[32];
    int opcao;

    fflush(stdout);
    if (!ler_linha(linha, sizeof linha))
        return 0;                                    /* fim da entrada: sai do programa */
    if (sscanf(linha, "%d", &opcao) != 1)
        return -1;                                   /* não digitou um número */
    return opcao;
}

static void ler_nome(char *destino, size_t tamanho)
{
    char linha[128];
    char *ini = linha;
    size_t n;

    printf("Digite seu nome (ate %d caracteres): ", TAM_NOME - 1);
    fflush(stdout);
    if (!ler_linha(linha, sizeof linha))
        linha[0] = '\0';

    while (*ini == ' ')                              /* tira os espaços das pontas */
        ini++;
    n = strlen(ini);
    while (n > 0 && ini[n - 1] == ' ')
        ini[--n] = '\0';

    strncpy(destino, (n > 0) ? ini : "Anonimo", tamanho - 1);
    destino[tamanho - 1] = '\0';
}

static void exibir_menu(const char *jogador)
{
    printf("\n==============================\n");
    printf("      JOGO DA COBRINHA\n");
    printf("==============================\n");
    printf(" Jogador: %s\n", jogador);
    printf("------------------------------\n");
    printf(" 1 - Jogar\n");
    printf(" 2 - Ver historico de partidas\n");
    printf(" 3 - Trocar de jogador\n");
    printf(" 0 - Sair\n");
    printf("==============================\n");
    printf("Escolha uma opcao: ");
}

/* ====================== PARTIDA ====================== */

/* Roda uma partida e grava o resultado no histórico. */
static void jogar(const char *jogador)
{
    ResultadoPartida r;

    printf("\nBoa sorte, %s! Abrindo a janela do jogo...\n", jogador);
    r = cobra_jogar();                               /* só volta quando a partida terminar */

    printf("\n--- Fim da partida ---\n");
    printf("Jogador: %s | Pontos: %d | Tamanho: %d | Tempo: %ds\n",
           jogador, r.pontos, r.tamanho, r.duracao);

    if (historico_salvar(jogador, r.fim, r.pontos, r.tamanho, r.duracao))
        printf("Partida registrada em %s\n", ARQUIVO_HISTORICO);
    else
        printf("Nao foi possivel gravar o arquivo %s\n", ARQUIVO_HISTORICO);
}

/* ====================== PROGRAMA PRINCIPAL ====================== */

int main(void)
{
    char jogador[TAM_NOME];
    int opcao;

    printf("Bem-vindo ao Jogo da Cobrinha!\n");
    ler_nome(jogador, sizeof jogador);

    do {
        exibir_menu(jogador);
        opcao = ler_opcao();

        switch (opcao) {
            case 1:  jogar(jogador);                    break;
            case 2:  historico_mostrar();               break;
            case 3:  ler_nome(jogador, sizeof jogador); break;
            case 0:  printf("\nAte a proxima!\n");      break;
            default: printf("\nOpcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
