# 🌈 Ray Tracer em C++ — Computação Gráfica

## 🧠 Descrição

Este projeto implementa um **renderizador do tipo *Ray Tracing*** em C++, baseado no livro  
📘 *Ray Tracing in One Weekend* de Peter Shirley.

O objetivo é gerar imagens 3D realistas simulando o comportamento físico da luz — com suporte a:
- Reflexão difusa e especular  
- Múltiplos materiais  
- Profundidade de campo  
- Anti-aliasing por amostragem múltipla  
- E saída direta em **formato PNG** utilizando a biblioteca **stb_image_write**

---

## 🧩 Estrutura do Projeto
raytracer/
├── main.cpp # Função principal — define cena e câmera
├── camera.h # Classe Camera (gera e dispara os raios)
├── vec3.h # Vetores e operações matemáticas básicas
├── ray.h # Representação de um raio
├── color.h # Manipulação e escrita de cores
├── hittable.h # Interface de objetos que podem ser atingidos
├── sphere.h # Implementação de esfera como objeto geométrico
├── material.h # Tipos de materiais (difuso, metálico, etc.)
└── stb_image_write.h # Biblioteca para salvar imagens em PNG

---

## 🧮 Funcionamento

O programa dispara um **raio por pixel** (com múltiplas amostras para suavização) e calcula:
1. Se o raio atinge um objeto (interseção);
2. O comportamento do raio após o impacto (reflexão, absorção, dispersão);
3. A cor resultante pela composição recursiva da luz ambiente e refletida.

A câmera é configurada com parâmetros de campo de visão, posição e foco.

---

## 💾 Saída

O render é salvo automaticamente em formato **PNG**, utilizando `stbi_write_png`:

```cpp
stbi_write_png("output.png", image_width, image_height, 3, image_data.data(), image_width * 3);
