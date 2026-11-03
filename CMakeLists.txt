cmake_minimum_required(VERSION 3.20)
project(SNAKE LANGUAGES C CXX)

# Padrões modernos e estáveis (C17 e C++23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Exporta compile_commands.json para autocompletion/LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Localização das bibliotecas nativas de OpenGL e GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(include)

# Adicione TODOS os arquivos .c que voce criou
add_executable(cobra
    src/main.c
    src/cobra.c
    src/historico.c
)

target_include_directories(cobra PRIVATE 
    ${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

# Linkagem das bibliotecas gráficas
target_link_libraries(cobra PRIVATE 
    GLUT::GLUT
    OpenGL::GL
    OpenGL::GLU
)