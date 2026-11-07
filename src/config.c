#include "config.h"
#include "cobra.h"
#include <stdio.h>
#include <string.h>

/* ====================== PERSONALIZAÇÃO ======================
   Matrícula: 1 3 1  ->  A = 1, B = 3, C = 1            */
#define MAT_A  1
#define MAT_B  3
#define MAT_C  1

#define GRADE_PADRAO       (12 + 2 * MAT_C)              /* 12 + 2 x 1     = 14       */
#define VELOCIDADE_PADRAO  (120 + 20 * (MAT_B % 5))      /* 120 + 20 x 3   = 180 ms   */
#define COR_PADRAO         ((CorCobra)(MAT_B % 5))       /* 3 mod 5 = 3    -> roxo    */
#define PONTOS_PADRAO      (10 + 5 * (MAT_A % 4))        /* 10 + 5 x 1     = 15       */

static const char *NOMES_CORES[] = { "verde", "azul", "laranja", "roxo", "amarelo" };
#define TOTAL_CORES  ((int)(sizeof NOMES_CORES / sizeof NOMES_CORES[0]))

void config_padrao(Config *cfg)
{
    cfg->grade             = GRADE_PADRAO;
    cfg->velocidade        = VELOCIDADE_PADRAO;
    cfg->cor               = COR_PADRAO;
    cfg->pontos_por_comida = PONTOS_PADRAO;
}

const char *config_nome_cor(CorCobra cor)
{
    if ((int)cor < 0 || (int)cor >= TOTAL_CORES)
        return "?";
    return NOMES_CORES[cor];
}

/* Tira espaços, '\n' e '\r' do começo e do fim do texto (altera o próprio texto). */
static char *aparar(char *texto)
{
    size_t n;

    while (*texto == ' ' || *texto == '\t')
        texto++;
    n = strlen(texto);
    while (n > 0 && (texto[n - 1] == ' ' || texto[n - 1] == '\t' ||
                     texto[n - 1] == '\n' || texto[n - 1] == '\r'))
        texto[--n] = '\0';
    return texto;
}

/* Converte o texto em inteiro dentro de [minimo, maximo].
   Retorna true se deu certo; false se não é um número ou está fora do limite. */
static bool ler_inteiro(const char *texto, int minimo, int maximo, int *destino)
{
    int  valor;
    char sobra;

    /* " %c" pega qualquer coisa que tenha sobrado depois do número, como em "20abc" */
    if (sscanf(texto, "%d %c", &valor, &sobra) != 1)
        return false;
    if (valor < minimo || valor > maximo)
        return false;
    *destino = valor;
    return true;
}

static bool ler_cor(const char *texto, CorCobra *destino)
{
    int i;

    for (i = 0; i < TOTAL_CORES; i++) {
        if (strcmp(texto, NOMES_CORES[i]) == 0) {
            *destino = (CorCobra)i;
            return true;
        }
    }
    return false;
}

/* Aplica uma linha "chave=valor". Retorna true se a linha era válida. */
static bool aplicar_opcao(Config *cfg, const char *chave, const char *valor)
{
    if (strcmp(chave, "grade") == 0)
        return ler_inteiro(valor, GRADE_MIN, GRADE_MAX, &cfg->grade);
    if (strcmp(chave, "velocidade") == 0)
        return ler_inteiro(valor, VELOCIDADE_MIN, VELOCIDADE_MAX, &cfg->velocidade);
    if (strcmp(chave, "pontos") == 0)
        return ler_inteiro(valor, PONTOS_MIN, PONTOS_MAX, &cfg->pontos_por_comida);
    if (strcmp(chave, "cor") == 0)
        return ler_cor(valor, &cfg->cor);
    return false;                                     /* chave desconhecida */
}

bool config_carregar(Config *cfg, const char *arquivo)
{
    char  linha[128];
    int   numero = 0;
    FILE *arq;

    config_padrao(cfg);

    arq = fopen(arquivo, "r");
    if (arq == NULL)
        return false;                                 /* sem arquivo: fica tudo no padrão */

    while (fgets(linha, sizeof linha, arq) != NULL) {
        char *texto = aparar(linha);
        char *igual;

        numero++;
        if (texto[0] == '\0' || texto[0] == '#')     /* linha vazia ou comentário */
            continue;

        igual = strchr(texto, '=');
        if (igual != NULL) {
            *igual = '\0';                            /* separa "chave" de "valor" */
            if (aplicar_opcao(cfg, aparar(texto), aparar(igual + 1)))
                continue;
        }
        printf("Aviso: %s, linha %d invalida; usando o valor padrao.\n", arquivo, numero);
    }
    fclose(arq);
    return true;
}

void config_mostrar(const Config *cfg)
{
    printf("Configuracao: grade %dx%d | velocidade inicial %d ms | cobra %s | %d pontos por comida\n",
           cfg->grade, cfg->grade, cfg->velocidade, config_nome_cor(cfg->cor),
           cfg->pontos_por_comida);
}
