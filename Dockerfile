FROM debian:stable-slim

ENV DEBIAN_FRONTEND=noninteractive

# Dependências de compilação, OpenGL, FreeGLUT e utilitários de janela/Mesa
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    build-essential \
    gdb \
    cmake \
    ninja-build \
    git \
    pkg-config \
    libgl-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    libglfw3-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    mesa-utils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace