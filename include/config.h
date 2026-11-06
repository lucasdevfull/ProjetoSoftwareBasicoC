/*
 * config.h - Configuração do jogo lida de um arquivo texto.
 * A implementação está em config.c.
 *
 * Formato do arquivo (uma opção por linha; linhas com # são comentários):
 *     grade=14
 *     velocidade=180
 *     cor=roxo
 *     pontos=15
 *
 * Se o arquivo não existir, ou se um valor for inválido, vale o valor padrão
 * (calculado a partir da matrícula, veja config.c).
 */
#ifndef CONFIG_H
#define CONFIG_H

#define ARQUIVO_CONFIG  "config.txt"      /* procurado na pasta onde o programa roda */

/* Limites aceitos para cada valor do arquivo */
#define GRADE_MIN        10               /* GRADE_MAX está em cobra.h */
#define VELOCIDADE_MIN   50               /* ms entre passos */
#define VELOCIDADE_MAX   1000
#define PONTOS_MIN       1
#define PONTOS_MAX       1000

typedef enum { COR_VERDE, COR_AZUL, COR_LARANJA, COR_ROXO, COR_AMARELO } CorCobra;

typedef struct {
    int      grade;               /* casas por lado (a grade é grade x grade) */
    int      velocidade;          /* ms entre passos no começo da partida     */
    CorCobra cor;
    int      pontos_por_comida;
} Config;

/* Preenche `cfg` com os valores padrão (os da matrícula). */
void config_padrao(Config *cfg);

/* Começa com os valores padrão e troca pelos do arquivo os que forem válidos.
   Avisa no console sobre linhas inválidas.
   Retorna true se conseguiu abrir o arquivo e false se ele não existe. */
bool config_carregar(Config *cfg, const char *arquivo);

/* Nome da cor, para mostrar no console ("verde", "azul"...). */
const char *config_nome_cor(CorCobra cor);

/* Mostra a configuração em uso no console. */
void config_mostrar(const Config *cfg);

#endif /* CONFIG_H */
