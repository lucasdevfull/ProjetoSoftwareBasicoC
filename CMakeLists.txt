# Atualizado para 3.21+ para garantir suporte nativo ao padrão C23
cmake_minimum_required(VERSION 3.21)
project(SNAKE LANGUAGES C CXX)

# Modificado para o padrão moderno C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Mantido o padrão C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Exporta compile_commands.json para autocompletion/LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# OpenGL vem do sistema (é a ponte com o driver de vídeo de cada máquina)
find_package(OpenGL REQUIRED)

# A freeglut é baixada e compilada junto com o jogo como biblioteca estática:
# não precisa instalar no sistema e ela vai dentro do executável.
# Use -DUSAR_FREEGLUT_DO_SISTEMA=ON para usar a freeglut instalada.
option(USAR_FREEGLUT_DO_SISTEMA "Usar a freeglut instalada no sistema" OFF)

if(USAR_FREEGLUT_DO_SISTEMA)
    find_package(GLUT REQUIRED)
else()
    include(FetchContent)
    FetchContent_Declare(freeglut
        URL      https://downloads.sourceforge.net/freeglut/freeglut-3.8.0.tar.gz
        URL_HASH MD5=ca8361b5b6a64b07795b41bd24f6a23e
        DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON  CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_DEMOS       OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)   # deixa o CMake 4 aceitar o projeto da freeglut
    set(CMAKE_C_EXTENSIONS ON)
    FetchContent_MakeAvailable(freeglut)
    set(CMAKE_C_EXTENSIONS OFF)
endif()

include_directories(include)

# Adicione TODOS os arquivos .c que voce criou
add_executable(cobra
    src/main.c
    src/cobra.c
    src/historico.c
    src/config.c
    src/ranking.c
    src/render.c
    src/jogo.c
)

# Linkagem das bibliotecas gráficas
if(USAR_FREEGLUT_DO_SISTEMA)
    target_link_libraries(cobra PRIVATE
        GLUT::GLUT
        OpenGL::GL
    )
else()
    target_include_directories(cobra PRIVATE ${freeglut_SOURCE_DIR}/include)
    target_compile_definitions(cobra PRIVATE FREEGLUT_STATIC)   # no Windows: não procurar a DLL
    target_link_libraries(cobra PRIVATE
        freeglut_static
        OpenGL::GL
    )
endif()