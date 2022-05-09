#define GUN 0
#define AIM 1
#define PLANE 2
#define LANDSCAPE 3
#define SKY 4
#define BULLET 5
#define ROCK 6
#define PICKAXE 7
#define ENEMY 8

#define NUM_OF_ROCKS 12
#define NUM_OF_ENEMIES 6

#define PICKAXE_ANGLE_X 2.356194
#define PICKAXE_ANGLE_Y 6.086837
#define PICKAXE_ANGLE_Z -1.178097

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <array>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include "utils.h"
#include "matrices.h"
#include "camera.h"
#include "collisions.h"
#include "bullet.h"
#include "sceneobject.h"
#include "bezier.h"
#include "helpers.h"
#include "enemy.h"
#include "rock.h"
#include "player.h"

struct ObjModel
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    ObjModel(const char *filename, const char *basepath = NULL, bool triangulate = true)
    {
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());
        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");
    }
};

void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4 &M);

void DrawVirtualObject(const char *object_name);
void DrawLandscape(glm::mat4 model, GLint model_uniform, GLint object_id_uniform);
void DrawLandscapes(glm::mat4 model, GLint model_uniform, GLint object_id_uniform);
void BuildTrianglesAndAddToVirtualScene(ObjModel *);
void ComputeNormals(ObjModel *model);
void BuildObject(const char *path);

void LoadShadersFromFiles();
void LoadTextureImage(const char *filename);
void LoadShader(const char *filename, GLuint shader_id);
void TextRendering_Init();
void TextRendering_PrintString(GLFWwindow *window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_ShowLivesCouting(GLFWwindow *window, int lives);
void TextRendering_ShowTotalPoints(GLFWwindow *window, int points);
void ShowGameEnd(GLFWwindow *window);
void TextRendering_PrintMatrix(GLFWwindow *window, glm::mat4 M, float x, float y, float scale);

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);

float TextRendering_LineHeight(GLFWwindow *window);
float TextRendering_CharWidth(GLFWwindow *window);

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);
GLuint LoadShader_Vertex(const char *filename);
GLuint LoadShader_Fragment(const char *filename);

std::map<std::string, SceneObject> g_VirtualScene;
std::stack<glm::mat4> g_MatrixStack;

float g_ScreenRatio = 1.0f;

float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;
bool g_MiddleMouseButtonPressed = false;

double mouseXPos, mouseYPos, mouseXOffset, mouseYOffset;
double g_LastCursorPosX, g_LastCursorPosY;

bool g_UsePerspectiveProjection = true;

GLuint vertex_shader_id;
GLuint fragment_shader_id;
GLuint program_id = 0;
GLuint g_NumLoadedTextures = 0;
GLint model_uniform;
GLint view_uniform;
GLint projection_uniform;
GLint object_id_uniform;
GLint bbox_min_uniform;
GLint bbox_max_uniform;

int g_lives = 3;
int g_points = 0;
int g_chosenTool = 0; // 0 = gun, 1 = pickaxe (standart is gun)
bool end = false;

int main()
{
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_WIDTH, "Danger Mine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, SCREEN_WIDTH, SCREEN_WIDTH);

    LoadShadersFromFiles();

    // Carregamos imagens para serem utilizadas como textura
    LoadTextureImage("../../data/sand.bmp");
    LoadTextureImage("../../data/metal.jpeg");
    LoadTextureImage("../../data/landscape.jpeg");
    LoadTextureImage("../../data/sky.png");
    LoadTextureImage("../../data/rock.jpeg");

    // Construímos a representação de objetos geométricos através de malhas de triângulos
    BuildObject("../../data/gun.obj");
    BuildObject("../../data/pickaxe.obj");
    BuildObject("../../data/aim.obj");
    BuildObject("../../data/plane.obj");
    BuildObject("../../data/landscape.obj");
    BuildObject("../../data/sky.obj");
    BuildObject("../../data/rock.obj");
    BuildObject("../../data/bullet.obj");
    BuildObject("../../data/enemy.obj");

    glEnable(GL_DEPTH_TEST);

    Camera camera(program_id);
    Collision collision;
    std::vector<Bullet> bullets;
    std::array<std::array<float, 3>, 6> plane_positions{{{0.0f, -1.0f, 0.0f},
                                                         {0.0f, 5.0f, 0.0f},
                                                         {0.0f, -1.0f, -10.0f},
                                                         {0.0f, -1.0f, 10.0f},
                                                         {0.0f, -1.0f, 10.0f},
                                                         {0.0f, -1.0f, -10.0f}}};
    double tprev = glfwGetTime();

    std::vector<Rock> rocks;
    std::vector<Enemy> enemies;
    Player player;

    for (int i = 0; i < NUM_OF_ENEMIES; i++)
    {
        enemies.push_back(Enemy());
        enemies[i].setRandomPosition();
        enemies[i].setControlPoints(camera);
    }

    for (int i = 0; i < NUM_OF_ROCKS; i++)
        rocks.push_back(Rock());

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id);

        glm::mat4 projection;

        float nearplane = -0.1f;
        float farplane = -20.0f;
        if (g_UsePerspectiveProjection)
        {
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            float t = 3.0f * camera.g_CameraDistance / 1.25f;
            float b = -t;
            float r = t * g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        bool isCollidingWithGround = collision.checkForGroundCollision(
            camera,
            glm::vec3(plane_positions[0][0],
                      plane_positions[0][1],
                      plane_positions[0][2]));

        bool isCollidingWithRock = collision.checkForRocksCollision(player, rocks);

        collision.checkForBulletScenaryCollision(bullets, plane_positions);
        camera.update();

        glm::mat4 model = Matrix_Identity();
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 inverse = invert(camera.view);

        // Desenhamos o player com a arma
        if (g_chosenTool == 0)
        {
            player.draw(g_VirtualScene, model, bbox_max_uniform, bbox_min_uniform, object_id_uniform, model_uniform, GUN, inverse, camera);
        }
        else if (g_chosenTool == 1)
        {
            // Desenhamos a picareta
            model = inverse * Matrix_Translate(0.45f, -0.5f, -0.8f) * Matrix_Scale(0.08f, 0.08f, 0.08f) * Matrix_Rotate_Z(PICKAXE_ANGLE_Z) * Matrix_Rotate_Y(PICKAXE_ANGLE_Y) * Matrix_Rotate_X(PICKAXE_ANGLE_X);
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, PICKAXE);
            DrawVirtualObject("pickaxe");
        }
        bool isCollidingWithEnemy = collision.checkForEnemiesPlayerCollision(camera, enemies);

        if (isCollidingWithEnemy)
            if (g_lives > 0) 
                g_lives--;

        if (g_lives <= 0)
            end = true;

        if (g_points == NUM_OF_ENEMIES)
            end = true;

        // Desenhamos a mira
        model = Matrix_Identity();
        model = inverse * Matrix_Translate(0.0f, 0.0f, -1.0f) * Matrix_Scale(0.005f, 0.005f, 0.005f);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, AIM);
        DrawVirtualObject("aim");

        // Desenhamos o plano
        model = Matrix_Translate(plane_positions[0][0], plane_positions[0][1], plane_positions[0][2]) * Matrix_Scale(10.0f, 10.0f, 10.0f);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, PLANE);
        DrawVirtualObject("plane");

        // Desenhamos o ceu
        model = Matrix_Translate(plane_positions[1][0], plane_positions[1][1], plane_positions[1][2]) * Matrix_Scale(10.0f, 10.0f, 10.0f);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(object_id_uniform, SKY);
        DrawVirtualObject("sky");

        // Desenhamos as fronteiras do mapa
        DrawLandscapes(model, model_uniform, object_id_uniform);

        // Desenhamos as rochas
        for (int i = 0; i < rocks.size(); i++)
            rocks[i].draw(g_VirtualScene, model, bbox_max_uniform, bbox_min_uniform, object_id_uniform, model_uniform, ROCK);

        // Desenhamos os inimigos
        for (int i = 0; i < enemies.size(); i++)
            enemies[i].draw(camera, g_VirtualScene, model, bbox_max_uniform, bbox_min_uniform, object_id_uniform, model_uniform, ENEMY);

        // Desenhamos o tiro
        for (int i = 0; i < bullets.size(); i++)
            bullets[i].draw(g_VirtualScene, model, bbox_max_uniform, bbox_min_uniform, object_id_uniform, model_uniform, BULLET, tprev);

        camera.listenForInputs(window, &mouseXPos, &mouseYPos, &mouseXOffset, &mouseYOffset, isCollidingWithGround, isCollidingWithRock, bullets, g_chosenTool);

        bool addPoint = collision.checkForBulletEnemyCollision(enemies, bullets);
        if (addPoint) g_points++;
        if (end) 
        {
            ShowGameEnd(window);
            glfwTerminate();
            return 0;
        }
        else
        {
            TextRendering_Init();
            TextRendering_ShowLivesCouting(window, g_lives);
            TextRendering_ShowTotalPoints(window, g_points);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}

void DrawLandscapes(glm::mat4 model, GLint model_uniform, GLint object_id_uniform)
{
    model = Matrix_Translate(0.0f, -1.0f, -10.0f) * Matrix_Scale(5.0f, 6.0f, 6.0f);
    DrawLandscape(model, model_uniform, object_id_uniform);

    model = Matrix_Translate(0.0f, -1.0f, 10.0f) * Matrix_Scale(5.0f, 6.0f, 6.0f);
    DrawLandscape(model, model_uniform, object_id_uniform);

    model = Matrix_Rotate_Y(1.571f) * Matrix_Translate(0.0f, -1.0f, 10.0f) * Matrix_Scale(5.0f, 6.0f, 6.0f);
    DrawLandscape(model, model_uniform, object_id_uniform);

    model = Matrix_Rotate_Y(1.571f) * Matrix_Translate(0.0f, -1.0f, -10.0f) * Matrix_Scale(5.0f, 6.0f, 6.0f);
    DrawLandscape(model, model_uniform, object_id_uniform);
}

void DrawLandscape(glm::mat4 model, GLint model_uniform, GLint object_id_uniform)
{
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(object_id_uniform, LANDSCAPE);
    DrawVirtualObject("landscape");
}

void LoadTextureImage(const char *filename)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if (data == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    GLuint texture_id, sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);
    g_NumLoadedTextures += 1;
}

void DrawVirtualObject(const char *object_name)
{
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;

    glUniform4f(bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene[object_name].first_index * sizeof(GLuint)));

    glBindVertexArray(0);
}

void LoadShadersFromFiles()
{
    vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    if (program_id != 0)
        glDeleteProgram(program_id);

    program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    model_uniform = glGetUniformLocation(program_id, "model");
    view_uniform = glGetUniformLocation(program_id, "view");
    projection_uniform = glGetUniformLocation(program_id, "projection");
    object_id_uniform = glGetUniformLocation(program_id, "object_id");
    bbox_min_uniform = glGetUniformLocation(program_id, "bbox_min");
    bbox_max_uniform = glGetUniformLocation(program_id, "bbox_max");

    glUseProgram(program_id);
    glUniform1i(glGetUniformLocation(program_id, "sand"), 0);
    glUniform1i(glGetUniformLocation(program_id, "metal"), 1);
    glUniform1i(glGetUniformLocation(program_id, "landscape"), 2);
    glUniform1i(glGetUniformLocation(program_id, "sky"), 3);
    glUniform1i(glGetUniformLocation(program_id, "rock"), 4);
    glUseProgram(0);
}

void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

void PopMatrix(glm::mat4 &M)
{
    if (g_MatrixStack.empty())
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

void ComputeNormals(ObjModel *model)
{
    if (!model->attrib.normals.empty()) return;

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4 vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx, vy, vz, 1.0);
            }

            const glm::vec4 a = vertices[0];
            const glm::vec4 b = vertices[1];
            const glm::vec4 c = vertices[2];

            const glm::vec4 n = crossproduct(b - a, c - a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize(3 * num_vertices);

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3 * i + 0] = n.x;
        model->attrib.normals[3 * i + 1] = n.y;
        model->attrib.normals[3 * i + 2] = n.z;
    }
}

void BuildTrianglesAndAddToVirtualScene(ObjModel *model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float> model_coefficients, normal_coefficients, texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval, maxval, maxval);
        glm::vec3 bbox_max = glm::vec3(minval, minval, minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];

                indices.push_back(first_index + 3 * triangle + vertex);

                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                model_coefficients.push_back(vx);
                model_coefficients.push_back(vy);
                model_coefficients.push_back(vz);
                model_coefficients.push_back(1.0f);
                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                if (idx.normal_index != -1)
                {
                    const float nx = model->attrib.normals[3 * idx.normal_index + 0];
                    const float ny = model->attrib.normals[3 * idx.normal_index + 1];
                    const float nz = model->attrib.normals[3 * idx.normal_index + 2];
                    normal_coefficients.push_back(nx);
                    normal_coefficients.push_back(ny);
                    normal_coefficients.push_back(nz);
                    normal_coefficients.push_back(0.0f);
                }

                if (idx.texcoord_index != -1)
                {
                    const float u = model->attrib.texcoords[2 * idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2 * idx.texcoord_index + 1];
                    texture_coefficients.push_back(u);
                    texture_coefficients.push_back(v);
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name = model->shapes[shape].name;
        theobject.first_index = first_index;
        theobject.num_indices = last_index - first_index + 1;
        theobject.rendering_mode = GL_TRIANGLES;
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0;
    GLint number_of_dimensions = 4;
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!normal_coefficients.empty())
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1;
        number_of_dimensions = 4;
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!texture_coefficients.empty())
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2;
        number_of_dimensions = 2;
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    glBindVertexArray(0);
}

GLuint LoadShader_Vertex(const char *filename)
{
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    LoadShader(filename, vertex_shader_id);
    return vertex_shader_id;
}

GLuint LoadShader_Fragment(const char *filename)
{
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    LoadShader(filename, fragment_shader_id);
    return fragment_shader_id;
}

void LoadShader(const char *filename, GLuint shader_id)
{
    std::ifstream file;
    try
    {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar *shader_string = str.c_str();
    const GLint shader_string_length = static_cast<GLint>(str.length());
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);
    glCompileShader(shader_id);
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    GLchar *log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    if (log_length != 0)
    {
        std::string output;

        if (!compiled_ok)
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }
    delete[] log;
}

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    if (linked_ok == GL_FALSE)
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        GLchar *log = new GLchar[log_length];
        glGetProgramInfoLog(program_id, log_length, &log_length, log);
        std::string output;
        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        delete[] log;

        fprintf(stderr, "%s", output.c_str());
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    mouseXPos = xpos;
    mouseYPos = ypos;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mod)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        g_chosenTool = (g_chosenTool == 0) ? 1 : 0;
    }
}

void ErrorCallback(int error, const char *description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

void ShowGameEnd(GLFWwindow *window)
{
    glClearColor(0.7333f, 0.5216f, 0.5216f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static char buffer[20] = "FIM";
    static int numchars = 3;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);
    
    TextRendering_Init();
    TextRendering_PrintString(window, buffer, 1.0f - (numchars + 1) * charwidth, 1.0f - lineheight, 1.0f);
    glfwSwapBuffers(window);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void TextRendering_ShowLivesCouting(GLFWwindow *window, int lives)
{
    static char buffer[20] = "? lives";
    static int numchars = 8;

    numchars = snprintf(buffer, 20, "%d lives", lives);

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f - (numchars + 1) * charwidth, 1.0f - lineheight, 1.0f);
}

void TextRendering_ShowTotalPoints(GLFWwindow *window, int points)
{
    static char buffer[50] = "Total points: ?";
    static int numchars = 15;

    numchars = snprintf(buffer, 50, "Total points: %d", points);

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, (numchars + 1) * charwidth, 1.0f - lineheight, 1.0f);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_ScreenRatio = (float)width / height;
}

void BuildObject(const char *path)
{
    ObjModel model(path);
    ComputeNormals(&model);
    BuildTrianglesAndAddToVirtualScene(&model);
}