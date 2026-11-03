/*
 * cobra.c - Jogo da cobrinha: lógica, desenho com quadrados (OpenGL) e
 *           janela/eventos (freeglut).
 *
 * Interface pública (veja cobra.h):  cobra_jogar()
 *
 * Durante o jogo:  setas = mover | P = pausar | ESC = sair da partida
 */

#include "cobra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GL/freeglut.h>

/* ====================== CONFIGURAÇÕES ====================== */

/* Janela do jogo: 600 x 600 pixels */
#define LARGURA_JANELA     600
#define ALTURA_JANELA      600

#define TAM_CELULA         24    /* lado de cada quadrado, em pixels        */
#define ALTURA_PLACAR      48    /* faixa no topo da janela com a pontuação */

/* A grade é calculada a partir do tamanho da janela:
   25 colunas x 23 linhas de 24 px (600 x 552) + placar de 48 px = 600 x 600 */
#define GRADE_LARGURA      (LARGURA_JANELA / TAM_CELULA)
#define GRADE_ALTURA       ((ALTURA_JANELA - ALTURA_PLACAR) / TAM_CELULA)
#define ALTURA_TABULEIRO   (GRADE_ALTURA * TAM_CELULA)
#define MAX_CELULAS        (GRADE_LARGURA * GRADE_ALTURA)

/* linha de base do texto do placar, centralizado na faixa do topo */
#define BASE_TEXTO_PLACAR  (ALTURA_TABULEIRO + (ALTURA_JANELA - ALTURA_TABULEIRO) / 2 - 6)

#if (LARGURA_JANELA % TAM_CELULA) != 0
#error "LARGURA_JANELA precisa ser multiplo de TAM_CELULA"
#endif

#define TAMANHO_INICIAL    3
#define INTERVALO_MS       120   /* tempo entre movimentos (menor = mais rápido) */
#define PONTOS_POR_COMIDA  10

/* ====================== TIPOS E ESTADO ====================== */

typedef struct { int x, y; } Posicao;            /* posição na grade (não em pixels) */

typedef enum { CIMA, BAIXO, ESQUERDA, DIREITA } Direcao;
typedef enum { JOGANDO, PAUSADO, FIM_DE_JOGO, VITORIA } Estado;

typedef struct {
    Posicao corpo[MAX_CELULAS];   /* corpo[0] é a cabeça                      */
    int     tamanho;
    Direcao direcao;              /* direção do último movimento realizado    */
    Direcao proxima;              /* direção pedida pelo jogador              */
    Posicao comida;
    int     pontos;
    Estado  estado;
    time_t  inicio;               /* quando a partida começou                 */
    time_t  fim;                  /* quando terminou (0 = ainda em andamento) */
} Jogo;

/* Os callbacks do GLUT não recebem parâmetros, então o jogo fica em uma variável global. */
static Jogo jogo;

/* ====================== LÓGICA DO JOGO ====================== */

/* Retorna 1 se algum segmento da cobra está na célula (x, y). */
static int celula_ocupada(int x, int y)
{
    for (int i = 0; i < jogo.tamanho; i++)
        if (jogo.corpo[i].x == x && jogo.corpo[i].y == y)
            return 1;
    return 0;
}

/* Sorteia uma célula livre para a comida. */
static void gerar_comida(void)
{
    int x, y;

    do {
        x = rand() % GRADE_LARGURA;
        y = rand() % GRADE_ALTURA;
    } while (celula_ocupada(x, y));

    jogo.comida.x = x;
    jogo.comida.y = y;
}

static Direcao direcao_oposta(Direcao d)
{
    switch (d) {
        case CIMA:     return BAIXO;
        case BAIXO:    return CIMA;
        case ESQUERDA: return DIREITA;
        default:       return ESQUERDA;
    }
}

static void encerrar_partida(Estado novo_estado)
{
    jogo.estado = novo_estado;
    jogo.fim = time(NULL);
}

static void reiniciar_jogo(void)
{
    /* cobra no centro do tabuleiro, andando para a direita */
    for (int i = 0; i < TAMANHO_INICIAL; i++) {
        jogo.corpo[i].x = GRADE_LARGURA / 2 - i;
        jogo.corpo[i].y = GRADE_ALTURA / 2;
    }
    jogo.tamanho = TAMANHO_INICIAL;
    jogo.direcao = DIREITA;
    jogo.proxima = DIREITA;
    jogo.pontos  = 0;
    jogo.estado  = JOGANDO;
    jogo.inicio  = time(NULL);
    jogo.fim     = 0;
    gerar_comida();
}

/* Avança a cobra uma célula na direção atual. */
static void mover_cobra(void)
{
    static const int DX[] = { 0,  0, -1, 1 };    /* CIMA, BAIXO, ESQUERDA, DIREITA */
    static const int DY[] = { 1, -1,  0, 0 };    /* no OpenGL o eixo Y cresce para cima */

    jogo.direcao = jogo.proxima;

    Posicao nova = { jogo.corpo[0].x + DX[jogo.direcao],
                     jogo.corpo[0].y + DY[jogo.direcao] };

    /* 1) bateu na parede? */
    if (nova.x < 0 || nova.x >= GRADE_LARGURA || nova.y < 0 || nova.y >= GRADE_ALTURA) {
        encerrar_partida(FIM_DE_JOGO);
        return;
    }

    int comeu = (nova.x == jogo.comida.x && nova.y == jogo.comida.y);

    /* 2) bateu no próprio corpo?
          Se a cobra NÃO vai comer, a ponta da cauda sai do lugar neste passo,
          então ela não conta como obstáculo. */
    int limite = comeu ? jogo.tamanho : jogo.tamanho - 1;
    for (int i = 0; i < limite; i++) {
        if (jogo.corpo[i].x == nova.x && jogo.corpo[i].y == nova.y) {
            encerrar_partida(FIM_DE_JOGO);
            return;
        }
    }

    /* 3) comeu: a cobra cresce (ganha mais um segmento no final) */
    if (comeu)
        jogo.tamanho++;

    /* 4) anda: empurra o corpo uma posição para trás e coloca a nova cabeça na frente */
    memmove(&jogo.corpo[1], &jogo.corpo[0], (jogo.tamanho - 1) * sizeof(Posicao));
    jogo.corpo[0] = nova;

    /* 5) pontuação e nova comida */
    if (comeu) {
        jogo.pontos += PONTOS_POR_COMIDA;

        if (jogo.tamanho == MAX_CELULAS)
            encerrar_partida(VITORIA);       /* a cobra ocupou o tabuleiro inteiro */
        else
            gerar_comida();
    }
}

/* ====================== DESENHO (OpenGL) ====================== */

static void desenhar_retangulo(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

/* Desenha um quadrado na célula da grade, deixando 1 pixel de folga
   para dar para enxergar a separação entre os quadrados. */
static void desenhar_celula(Posicao p, float r, float g, float b)
{
    float x = (float)(p.x * TAM_CELULA);
    float y = (float)(p.y * TAM_CELULA);

    glColor3f(r, g, b);
    desenhar_retangulo(x + 1, y + 1, x + TAM_CELULA - 1, y + TAM_CELULA - 1);
}

static void desenhar_texto(float x, float y, void *fonte, const char *texto)
{
    glRasterPos2f(x, y);
    for (const char *c = texto; *c != '\0'; c++)
        glutBitmapCharacter(fonte, *c);
}

static void desenhar_texto_centralizado(float y, void *fonte, const char *texto)
{
    float largura = (float)glutBitmapLength(fonte, (const unsigned char *)texto);
    desenhar_texto((LARGURA_JANELA - largura) / 2.0f, y, fonte, texto);
}

/* Escurece o tabuleiro e escreve uma mensagem no meio (pausa, fim de jogo...). */
static void desenhar_mensagem(const char *titulo, const char *subtitulo)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.70f);
    desenhar_retangulo(0, 0, LARGURA_JANELA, ALTURA_TABULEIRO);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 1.0f, 1.0f);
    desenhar_texto_centralizado(ALTURA_TABULEIRO / 2 + 10, GLUT_BITMAP_TIMES_ROMAN_24, titulo);
    desenhar_texto_centralizado(ALTURA_TABULEIRO / 2 - 25, GLUT_BITMAP_HELVETICA_18, subtitulo);
}

static void desenhar(void)
{
    char texto[64];

    glClearColor(0.07f, 0.07f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* fundo do tabuleiro */
    glColor3f(0.13f, 0.14f, 0.17f);
    desenhar_retangulo(0, 0, LARGURA_JANELA, ALTURA_TABULEIRO);

    /* comida: quadrado vermelho */
    desenhar_celula(jogo.comida, 0.90f, 0.20f, 0.20f);

    /* cobra: cabeça em verde claro, corpo em verde escuro */
    for (int i = 0; i < jogo.tamanho; i++) {
        if (i == 0) desenhar_celula(jogo.corpo[i], 0.55f, 1.00f, 0.55f);
        else        desenhar_celula(jogo.corpo[i], 0.15f, 0.70f, 0.25f);
    }

    /* placar no topo da janela */
    glColor3f(1.0f, 1.0f, 1.0f);
    snprintf(texto, sizeof texto, "Pontos: %d", jogo.pontos);
    desenhar_texto(10, BASE_TEXTO_PLACAR, GLUT_BITMAP_HELVETICA_18, texto);
    snprintf(texto, sizeof texto, "Tamanho: %d", jogo.tamanho);
    desenhar_texto(LARGURA_JANELA - 130, BASE_TEXTO_PLACAR, GLUT_BITMAP_HELVETICA_18, texto);

    /* mensagens por cima do tabuleiro */
    if (jogo.estado == PAUSADO)
        desenhar_mensagem("PAUSADO", "Pressione P para continuar");
    else if (jogo.estado == FIM_DE_JOGO)
        desenhar_mensagem("FIM DE JOGO", "Pressione ENTER para voltar ao menu");
    else if (jogo.estado == VITORIA)
        desenhar_mensagem("VOCE VENCEU!", "Pressione ENTER para voltar ao menu");

    glutSwapBuffers();
}

static void redimensionar(int largura, int altura)
{
    if (altura == 0)
        altura = 1;
    glViewport(0, 0, largura, altura);

    /* coordenadas em "pixels lógicos": (0,0) é o canto inferior esquerdo */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_JANELA, 0, ALTURA_JANELA, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ====================== CALLBACKS DO GLUT ====================== */

/* Chamado a cada INTERVALO_MS: é o "relógio" do jogo. */
static void atualizar(int valor)
{
    (void)valor;

    if (jogo.estado == JOGANDO) {
        mover_cobra();
        glutPostRedisplay();
    }
    glutTimerFunc(INTERVALO_MS, atualizar, 0);      /* agenda o próximo passo */
}

/* Teclas comuns: ESC, ENTER e P. */
static void teclado(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla) {
        case 27:                                    /* ESC: sai da partida */
            glutLeaveMainLoop();
            break;

        case 13:                                    /* ENTER: volta ao menu depois do fim */
            if (jogo.estado == FIM_DE_JOGO || jogo.estado == VITORIA)
                glutLeaveMainLoop();
            break;

        case 'p':
        case 'P':
            if (jogo.estado == JOGANDO)
                jogo.estado = PAUSADO;
            else if (jogo.estado == PAUSADO)
                jogo.estado = JOGANDO;
            glutPostRedisplay();
            break;
    }
}

/* Teclas especiais: as setas. */
static void teclas_especiais(int tecla, int x, int y)
{
    Direcao nova;

    (void)x;
    (void)y;

    switch (tecla) {
        case GLUT_KEY_UP:    nova = CIMA;     break;
        case GLUT_KEY_DOWN:  nova = BAIXO;    break;
        case GLUT_KEY_LEFT:  nova = ESQUERDA; break;
        case GLUT_KEY_RIGHT: nova = DIREITA;  break;
        default:             return;
    }

    /* Não deixa a cobra dar meia-volta (ela bateria no próprio pescoço).
       Compara com a direção do último movimento feito, e não com "proxima",
       para que dois toques rápidos dentro do mesmo intervalo não burlem a regra. */
    if (nova != direcao_oposta(jogo.direcao))
        jogo.proxima = nova;
}

/* ====================== INTERFACE PÚBLICA ====================== */

ResultadoPartida cobra_jogar(void)
{
    static int semeado = 0;
    int   argc   = 1;
    char *argv[] = { "cobra", NULL };
    ResultadoPartida resultado;
    time_t fim;

    if (!semeado) {                                  /* semente do sorteio da comida: só uma vez */
        srand((unsigned)time(NULL));
        semeado = 1;
    }

    reiniciar_jogo();

    /* A freeglut se encerra por completo quando o glutMainLoop() termina.
       Por isso o glutInit() é chamado de novo a cada partida. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);
    glutInitWindowPosition(100, 100);

    /* Faz o glutMainLoop() RETORNAR (em vez de encerrar o programa) quando a
       janela for fechada ou quando chamarmos glutLeaveMainLoop(). É isso que
       permite voltar ao menu do terminal depois de cada partida. */
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutCreateWindow("Jogo da Cobrinha");
    glutDisplayFunc(desenhar);
    glutReshapeFunc(redimensionar);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclas_especiais);
    glutTimerFunc(INTERVALO_MS, atualizar, 0);

    glutMainLoop();                                  /* só volta quando a partida terminar */

    /* partida acabou: devolve o resultado */
    fim = (jogo.fim != 0) ? jogo.fim : time(NULL);   /* ESC ou janela fechada: usa a hora atual */

    resultado.pontos  = jogo.pontos;
    resultado.tamanho = jogo.tamanho;
    resultado.duracao = (int)difftime(fim, jogo.inicio);
    resultado.fim     = fim;
    return resultado;
}
