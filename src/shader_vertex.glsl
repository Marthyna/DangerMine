#version 330 core

// Atributos de vertice recebidos como entrada ("in") pelo Vertex Shader
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no codigo C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 light;
vec4 light_vector;

// Atributos de vertice que serao gerados como saida ("out") pelo Vertex Shader.
// Estes serao interpolados pelo rasterizador, gerando valores
// para cada fragmento, os quais serao recebidos como entrada pelo Fragment
// Shader
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;
out vec3 gouraud_color;

void main()
{
    // A variavel gl_Position define a posicao final de cada vertice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estara entre -1 e 1 apas divisao por w.

    // O codigo em "main.cpp" define os vertices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operacoes de modelagem, definicao da camera, e projecao, para computar
    // as coordenadas finais em NDC (variavel gl_Position). Apos a execucao
    // deste Vertex Shader, a placa de video (GPU) fara a divisao por W
    gl_Position = projection * view * model * model_coefficients;

    // Agora definimos outros atributos dos vertices que serao interpolados pelo
    // rasterizador para gerar atributos unicos para cada fragmento gerado.

    // Posicao do vertice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posicao do vertice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vertice atual no sistema de coordenadas global (World)
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    texcoords = texture_coefficients;

    float distance = length(light - position_model);
    light_vector = normalize(light - position_model);
    float diffuse = max(dot(normal, light_vector), 0.1);
    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
    gouraud_color = vec3(0.2, 0.2, 0.2) * diffuse;
}

