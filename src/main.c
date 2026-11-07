#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "historico.h"
#include "jogo.h"
#include "ranking.h"
#include "render.h"

/* Dados do programa que o menu e o fim de partida usam. */
typedef struct {
    char    jogador[TAM_NOME];
    int     dificuldade;          /* 1 fácil, 2 normal, 3 difícil (ainda não muda nada no jogo) */
    Config  config;
    Ranking ranking;
} Sessao;

/* ====================== ENTRADA DO CONSOLE ====================== */

/* Lê uma linha do teclado sem o '\n' e descarta o que passar do tamanho do buffer.
   Retorna false se a entrada acabou (EOF). */
static bool ler_linha(char *destino, size_t tamanho)
{
    size_t n;

    if (fgets(destino, (int)tamanho, stdin) == NULL)
        return false;

    n = strlen(destino);
    if (n > 0 && destino[n - 1] == '\n') {
        destino[n - 1] = '\0';
    } else {
        int c;                                       /* linha maior que o buffer: joga o resto fora */
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    return true;
}

/* Lê um número inteiro sozinho na linha.
   Retorna 1 se leu, 0 se não era um número e -1 se a entrada acabou (EOF). */
static int ler_numero(int *valor)
{
    char linha[32];
    char sobra;

    fflush(stdout);
    if (!ler_linha(linha, sizeof linha))
        return -1;
    /* " %c" pega qualquer coisa depois do número, como em "2abc" */
    if (sscanf(linha, "%d %c", valor, &sobra) != 1)
        return 0;
    return 1;
}

static void ler_nome(char *destino, size_t tamanho)
{
    char linha[128];
    char *ini;
    size_t n;

    for (;;) {
        printf("Digite seu nome (ate %d caracteres): ", TAM_NOME - 1);
        fflush(stdout);
        if (!ler_linha(linha, sizeof linha))
            linha[0] = '\0';                         
        ini = linha;
        while (*ini == ' ')                          
            ini++;
        n = strlen(ini);
        while (n > 0 && ini[n - 1] == ' ')
            ini[--n] = '\0';

        if (n <= TAM_NOME - 1)
            break;                                   

        printf("Aviso: o nome nao pode exceder %d caracteres (voce digitou %zu).\n",
               TAM_NOME - 1, n);
    }

    strncpy(destino, (n > 0) ? ini : "Anonimo", tamanho - 1);
    destino[tamanho - 1] = '\0';
}

/* Pergunta até receber 1, 2 ou 3. Se a entrada acabar, fica no normal. */
static int ler_dificuldade(void)
{
    int valor;
    int lido;

    for (;;) {
        printf("Dificuldade (1 - facil, 2 - normal, 3 - dificil): ");
        lido = ler_numero(&valor);
        if (lido == -1)
            return 2;
        if (lido == 1 && valor >= 1 && valor <= 3)
            return valor;
        printf("Valor invalido. Digite 1, 2 ou 3.\n");
    }
}

static const char *nome_dificuldade(int dificuldade)
{
    switch (dificuldade) {
        case 1:  return "facil";
        case 3:  return "dificil";
        default: return "normal";
    }
}

static void exibir_menu(const Sessao *s)
{
    printf("\n==============================\n");
    printf("      JOGO DA COBRINHA\n");
    printf("==============================\n");
    printf(" Jogador: %s\n", s->jogador);
    printf(" Dificuldade: %s\n", nome_dificuldade(s->dificuldade));
    printf("------------------------------\n");
    printf(" 1 - Jogar\n");
    printf(" 2 - Ver ranking\n");
    printf(" 3 - Ver historico de partidas\n");
    printf(" 4 - Trocar de jogador\n");
    printf(" 5 - Trocar dificuldade\n");
    printf(" 0 - Sair\n");
    printf("==============================\n");
    printf("Escolha uma opcao: ");
}

/* ====================== PARTIDA ====================== */

/* Chamada pelo render.c ao fim de cada partida: grava ranking e histórico. */
static void registrar_partida(const Jogo *j, void *contexto)
{
    Sessao *s = contexto;
    int posicao;

    printf("\n--- Fim da partida ---\n");
    printf("Jogador: %s | Pontos: %d | Comidas: %d | Tempo: %ds\n",
           s->jogador, j->pontos, j->comidas, jogo_duracao(j));

    posicao = ranking_inserir(&s->ranking, s->jogador, j->pontos);
    if (posicao > 0)
        printf("Voce entrou no ranking em %do lugar!\n", posicao);
    if (!ranking_salvar(&s->ranking, ARQUIVO_RANKING))
        printf("Nao foi possivel gravar o arquivo %s\n", ARQUIVO_RANKING);

    if (!historico_salvar(s->jogador, j->fim, j->pontos, j->comidas, jogo_duracao(j)))
        printf("Nao foi possivel gravar o arquivo %s\n", ARQUIVO_HISTORICO);

    ranking_mostrar(&s->ranking);
    printf("Clique na janela para jogar de novo ou aperte ESC para voltar ao menu.\n");
    fflush(stdout);
}

static void jogar(Sessao *s)
{
    Jogo jogo;

    printf("\nBoa sorte, %s! Abrindo a janela do jogo...\n", s->jogador);
    printf("Setas: mover | P: pausar | ESC: sair | clique: jogar de novo apos o fim\n");
    fflush(stdout);

    jogo_iniciar(&jogo, &s->config);
    render_jogar(&jogo, s->config.cor, registrar_partida, s);   /* só volta com ESC */
}

/* ====================== PROGRAMA PRINCIPAL ====================== */

int main(void)
{
    static Sessao s;                                 /* static: não ocupa a pilha */
    int opcao;
    int lido;

    srand((unsigned)time(NULL));                     /* semente do sorteio da comida */

    printf("Bem-vindo ao Jogo da Cobrinha!\n");

    if (!config_carregar(&s.config, ARQUIVO_CONFIG))
        printf("Arquivo %s nao encontrado; usando os valores padrao.\n", ARQUIVO_CONFIG);
    config_mostrar(&s.config);

    ranking_carregar(&s.ranking, ARQUIVO_RANKING);   /* sem arquivo: começa vazio */

    ler_nome(s.jogador, sizeof s.jogador);
    s.dificuldade = ler_dificuldade();

    do {
        exibir_menu(&s);
        lido = ler_numero(&opcao);
        if (lido == -1)
            opcao = 0;                               /* fim da entrada: sai do programa */
        else if (lido == 0)
            opcao = -1;                              /* não digitou um número */

        switch (opcao) {
            case 1:  jogar(&s);                                  break;
            case 2:  ranking_mostrar(&s.ranking);                break;
            case 3:  historico_mostrar();                        break;
            case 4:  ler_nome(s.jogador, sizeof s.jogador);      break;
            case 5:  s.dificuldade = ler_dificuldade();          break;
            case 0:  printf("\nAte a proxima!\n");               break;
            default: printf("\nOpcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
