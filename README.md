# Trabalho AV2 – Jogo da Cobrinha em C

Programação de Software Básico em C – ARA3327 · 2026.2

- **Aluno:** Lucas Almeida da Silva
- **Matrícula (3 últimos dígitos):** 131 → A = 1, B = 3, C = 1

## Personalização

| O que                     | Regra                  | Valor           |
|---------------------------|------------------------|-----------------|
| Tamanho da grade          | 12 + 2 × C             | 14 × 14 casas   |
| Velocidade inicial        | 120 + 20 × (B mod 5)   | 180 ms          |
| Cor da cobra              | B mod 5 = 3            | roxo            |
| Pontos por comida         | 10 + 5 × (A mod 4)     | 15              |

Aceleração: a cada 5 comidas o intervalo diminui 10 ms, até o mínimo de 50 ms.

Os valores padrão estão em `src/config.c` e podem ser trocados no `config.txt`.

## Organização

| Módulo                   | O que faz                                                       |
|--------------------------|-----------------------------------------------------------------|
| `cobra.h / cobra.c`      | struct `Cobra` e funções com ponteiro (mover, crescer, colisão) |
| `jogo.h / jogo.c`        | regras da partida: comida, paredes, pontos, aceleração          |
| `render.h / render.c`    | janela freeglut, desenho OpenGL, teclado e mouse                |
| `config.h / config.c`    | leitura do `config.txt`                                         |
| `ranking.h / ranking.c`  | ranking dos 5 melhores no `ranking.txt`                         |
| `historico.h / historico.c` | extensão C: histórico no `historico.txt`, média e melhor     |
| `main.c`                 | menu no console                                                 |

## Como compilar

Precisa de um compilador C com suporte a C23, CMake 3.21+, OpenGL e freeglut
(no Debian/Ubuntu: `sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev`).

```sh
cmake -S . -B build
cmake --build build
cd build
./cobra
```

Também dá para usar o `Dockerfile`, que já tem todas as dependências.

## Como jogar

1. No console, digite seu nome e a dificuldade (1, 2 ou 3).
2. No menu, escolha **1 - Jogar** para abrir a janela.
3. Na janela:
   - **Setas:** mudam a direção da cobra.
   - **P:** pausa e continua.
   - **Clique esquerdo** depois do fim: começa outra partida.
   - **ESC:** fecha a janela e volta ao menu.
4. A pontuação aparece no título da janela. Ao fim de cada partida, o ranking aparece no console.

Arquivos criados na pasta onde o programa roda: `ranking.txt` e `historico.txt`.
