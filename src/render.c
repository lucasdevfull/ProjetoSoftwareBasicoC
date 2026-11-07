#include "render.h"
#include <stdio.h>
#include <GL/freeglut.h>

#define LARGURA_JANELA  600
#define ALTURA_JANELA   600
#define TITULO_JANELA   "Jogo da Cobrinha"

typedef enum : int {
    KEY_ESC   = 27,
    KEY_P_MIN = 'p',
    KEY_P_MAI = 'P'
} KeyboardKey;

/* Cores da cobra (cabeça e corpo), na ordem do enum CorCobra. */
static const float CORES_COBRA[][2][3] = {
    { { 0.55f, 1.00f, 0.55f }, { 0.15f, 0.70f, 0.25f } },   /* verde   */
    { { 0.55f, 0.75f, 1.00f }, { 0.15f, 0.40f, 0.90f } },   /* azul    */
    { { 1.00f, 0.75f, 0.40f }, { 0.95f, 0.50f, 0.10f } },   /* laranja */
    { { 0.80f, 0.60f, 1.00f }, { 0.55f, 0.25f, 0.85f } },   /* roxo    */
    { { 1.00f, 1.00f, 0.60f }, { 0.90f, 0.80f, 0.10f } },   /* amarelo */
};

/* Os callbacks do GLUT não recebem parâmetros, então o estado do jogo fica
   nesta única variável global (static: só este arquivo enxerga). */
static struct {
    Jogo             *jogo;
    CorCobra          cor;
    AoTerminarPartida ao_terminar;
    void             *contexto;
    bool              registrada;    /* a partida atual já foi entregue a ao_terminar? */
} tela;

/* ====================== AUXILIARES ====================== */

static float tamanho_casa(void)
{
    return (float)LARGURA_JANELA / (float)tela.jogo->grade;
}

/* Avisa quem chamou render_jogar que a partida acabou (uma vez por partida). */
static void registrar_se_terminou(void)
{
    if (tela.registrada || !jogo_terminou(tela.jogo))
        return;
    tela.registrada = true;
    if (tela.ao_terminar != NULL)
        tela.ao_terminar(tela.jogo, tela.contexto);
}

/* A pontuação aparece no título da janela. */
static void atualizar_titulo(void)
{
    char titulo[96];
    const char *situacao = "";

    switch (tela.jogo->estado) {
        case PAUSADO:     situacao = " (PAUSADO)";     break;
        case FIM_DE_JOGO: situacao = " (FIM DE JOGO)"; break;
        case VITORIA:     situacao = " (VITORIA!)";    break;
        default:                                       break;
    }
    snprintf(titulo, sizeof titulo, "%s - Pontos: %d%s",
             TITULO_JANELA, tela.jogo->pontos, situacao);
    glutSetWindowTitle(titulo);
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

/* Desenha um quadrado na casa da grade, deixando 1 pixel de folga
   para dar para enxergar a separação entre os quadrados. */
static void desenhar_casa(Posicao p, const float cor[3])
{
    float lado = tamanho_casa();
    float x = (float)p.x * lado;
    float y = (float)p.y * lado;

    glColor3f(cor[0], cor[1], cor[2]);
    desenhar_retangulo(x + 1, y + 1, x + lado - 1, y + lado - 1);
}

static void desenhar_texto_centralizado(float y, void *fonte, const char *texto)
{
    const char *c;
    float largura = (float)glutBitmapLength(fonte, (const unsigned char *)texto);

    glRasterPos2f((LARGURA_JANELA - largura) / 2.0f, y);
    for (c = texto; *c != '\0'; c++)
        glutBitmapCharacter(fonte, *c);
}

/* Escurece o tabuleiro e escreve uma mensagem no meio (pausa, fim de jogo...). */
static void desenhar_mensagem(const char *titulo, const char *subtitulo)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.70f);
    desenhar_retangulo(0, 0, LARGURA_JANELA, ALTURA_JANELA);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 1.0f, 1.0f);
    desenhar_texto_centralizado(ALTURA_JANELA / 2 + 10, GLUT_BITMAP_TIMES_ROMAN_24, titulo);
    desenhar_texto_centralizado(ALTURA_JANELA / 2 - 25, GLUT_BITMAP_HELVETICA_18, subtitulo);
}

static void desenhar(void)
{
    static const float COR_COMIDA[3] = { 0.90f, 0.20f, 0.20f };
    const Jogo *j = tela.jogo;
    int i;

    glClearColor(0.13f, 0.14f, 0.17f, 1.0f);      /* fundo do tabuleiro */
    glClear(GL_COLOR_BUFFER_BIT);

    desenhar_casa(j->comida, COR_COMIDA);

    for (i = 0; i < j->cobra.tamanho; i++)        /* cabeça mais clara que o corpo */
        desenhar_casa(j->cobra.corpo[i], CORES_COBRA[tela.cor][i == 0 ? 0 : 1]);

    if (j->estado == PAUSADO)
        desenhar_mensagem("PAUSADO", "Pressione P para continuar");
    else if (j->estado == FIM_DE_JOGO)
        desenhar_mensagem("FIM DE JOGO", "Clique para jogar de novo | ESC para sair");
    else if (j->estado == VITORIA)
        desenhar_mensagem("VOCE VENCEU!", "Clique para jogar de novo | ESC para sair");

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

/* Chamado a cada passo: é o "relógio" do jogo. */
static void atualizar(int valor)
{
    (void)valor;

    if (tela.jogo->estado == JOGANDO) {
        jogo_passo(tela.jogo);
        atualizar_titulo();
        glutPostRedisplay();
        registrar_se_terminou();
    }
    /* o intervalo diminui conforme a cobra come (veja jogo.c) */
    glutTimerFunc((unsigned)tela.jogo->intervalo, atualizar, 0);
}

/* Teclas comuns: ESC e P. */
static void teclado(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla) {
        case KEY_ESC:
            glutLeaveMainLoop();
            break;

        case KEY_P_MIN:
        case KEY_P_MAI:
            jogo_alternar_pausa(tela.jogo);
            atualizar_titulo();
            glutPostRedisplay();
            break;
    }
}

/* Teclas especiais: as setas. */
static void teclas_especiais(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla) {
        case GLUT_KEY_UP:    jogo_mudar_direcao(tela.jogo, CIMA);     break;
        case GLUT_KEY_DOWN:  jogo_mudar_direcao(tela.jogo, BAIXO);    break;
        case GLUT_KEY_LEFT:  jogo_mudar_direcao(tela.jogo, ESQUERDA); break;
        case GLUT_KEY_RIGHT: jogo_mudar_direcao(tela.jogo, DIREITA);  break;
    }
}

/* Mouse: clique esquerdo depois do fim reinicia a partida. */
static void mouse(int botao, int estado, int x, int y)
{
    (void)x;
    (void)y;

    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN && jogo_terminou(tela.jogo)) {
        jogo_reiniciar(tela.jogo);
        tela.registrada = false;
        atualizar_titulo();
        glutPostRedisplay();
    }
}

/* ====================== INTERFACE PÚBLICA ====================== */

void render_jogar(Jogo *jogo, CorCobra cor, AoTerminarPartida ao_terminar, void *contexto)
{
    int   argc = 1;
    char  nome_programa[] = "cobra";
    char *argv[] = { nome_programa, NULL };

    tela.jogo        = jogo;
    tela.cor         = cor;
    tela.ao_terminar = ao_terminar;
    tela.contexto    = contexto;
    tela.registrada  = false;

    /* A freeglut se encerra por completo quando o glutMainLoop() termina.
       Por isso o glutInit() é chamado de novo a cada vez que a janela abre. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);
    glutInitWindowPosition(100, 100);

    /* Faz o glutMainLoop() RETORNAR (em vez de encerrar o programa) quando a
       janela for fechada ou quando chamarmos glutLeaveMainLoop(). É isso que
       permite voltar ao menu do terminal. */
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutCreateWindow(TITULO_JANELA);
    atualizar_titulo();
    glutDisplayFunc(desenhar);
    glutReshapeFunc(redimensionar);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclas_especiais);
    glutMouseFunc(mouse);
    glutTimerFunc((unsigned)jogo->intervalo, atualizar, 0);

    glutMainLoop();                               /* só volta com ESC ou janela fechada */

    /* Saiu no meio de uma partida: ela também é registrada. */
    jogo_abandonar(jogo);
    registrar_se_terminou();
}
