#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "map/Map.h"
#include "Player.h"

#include "resources/shaders/Program.h"
#include "resources/Tile.h"

GLFWwindow* window;

std::ostream* LogStream = &std::cout;
std::ostream* ErrorStream = &std::cerr;

GLuint quadVAO;
GLuint quadEBO;
GLuint quadVBO;

GLuint raycastTexture;

int RAYS = 1000;
float TILE_PER_SPRITE = 10;
float HEIGHT_FACTOR = 10;

bool forward = false;
bool backward = false;
bool left = false;
bool right = false;

float lastMouseX = -1;

Map* map;
Player* player;

void SetupMap() {
    map = new Map(glm::ivec2(50, 50));

    for (int i = 0; i < 10; i++) {
        map->setTileIdAt(glm::ivec2(10, 5 + i), 1);
    }


    for (int i = 0; i < 30; i++) {
        map->setTileIdAt(glm::ivec2(20, 5 + i), 2);
    }

    for (int i = 0; i < 30; i++) {
        map->setTileIdAt(glm::ivec2(5 + i, 20), 1);
    }
}

void SetupPlayer() {
    player = new Player(map);

    player->setPosition(glm::ivec2(25, 25));
}

void SetupRenderTexture() {
    // Generate texture
    glGenTextures(1, &raycastTexture);
    glBindTexture(GL_TEXTURE_1D_ARRAY, raycastTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void SetupRenderQuad() {
    GLfloat quadVertices[] = {
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, -1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, -1.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    };

    GLuint quadIndices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    // Vertex Array Object (VAO)
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // Vertex Buffer Object (VBO)
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Element Buffer Object (EBO)
    glGenBuffers(1, &quadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void DeleteRenderQuad() {

}

void DeleteRenderTexture() {

}

void Init() {
    Tile::LoadTiles();
    SetupMap();
    SetupPlayer();
    SetupRenderTexture();
    SetupRenderQuad();
    Program::LoadShaders();
}

void Stop() {
    Tile::UnloadTiles();
    delete player;
    delete map;
    DeleteRenderQuad();
    DeleteRenderTexture();
    Program::UnloadShaders();
}

void Compute(double deltaTime, glm::dvec2 cursorPos) {
    //Move player
    glm::vec2 move(0, 0);

    if (forward) move += glm::vec2(1, 0);
    if (backward) move += glm::vec2(-1, 0);
    if (left) move += glm::vec2(0, 1);
    if (right) move += glm::vec2(0, -1);

    player->move(move, deltaTime);

    float panDelta = (float)cursorPos.x - lastMouseX;
    lastMouseX = cursorPos.x;
    player->pan(-panDelta);
}

void Render(double deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT);

    //Raycast
    GLfloat* raycastDatas = new GLfloat[(RAYS * 2 + 1) * 3];


    for (int i = RAYS; i >= -RAYS; i = i - 1) {
        float angle = i / (float)RAYS;

        RayHit hit = player->rayCast(angle);

        int index = RAYS - i;
        int offset = RAYS * 2 + 1;

        //Distamce
        raycastDatas[index] = hit.distance;

        //Offsets
        raycastDatas[index + offset] = hit.hitOffset;

        //IDs
        raycastDatas[index + 2 * offset] = hit.tileId;
    }

    glUniform1f(glGetUniformLocation(Program::GetShaderId("raycast_view"), "HEIGHT_FACTOR"), HEIGHT_FACTOR);
    glUniform1f(glGetUniformLocation(Program::GetShaderId("raycast_view"), "TILE_PER_SPRITE"), TILE_PER_SPRITE);

    glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_R32F, RAYS * 2 + 1, 3, 0, GL_RED, GL_FLOAT, raycastDatas);
    glGenerateMipmap(raycastTexture);
    delete[] raycastDatas;



    for (int i = 1; i <= Tile::GetTileNumber(); i++) {
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_view"), "currentTile"), i);

        glBindTexture(GL_TEXTURE_1D_ARRAY, raycastTexture);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_view"), "raycastData"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *Tile::GetTile(i)->getBaseTexture());
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_view"), "baseColor"), 1);
    
        glUseProgram(Program::GetShaderId("raycast_view"));

        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action)
    {
    case GLFW_PRESS:
        switch (key)
        {
        case GLFW_KEY_W:
            forward = true;
            break;
        case GLFW_KEY_A:
            left = true;
            break;
        case GLFW_KEY_S:
            backward = true;
            break;
        case GLFW_KEY_D:
            right = true;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            player->setRunning(true);
            break;
        case GLFW_KEY_ESCAPE:
            exit(0);
        }
        break;
    case GLFW_RELEASE:
        switch (key)
        {
        case GLFW_KEY_W:
            forward = false;
            break;
        case GLFW_KEY_A:
            left = false;
            break;
        case GLFW_KEY_S:
            backward = false;
            break;
        case GLFW_KEY_D:
            right = false;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            player->setRunning(false);
            break;
        }
        break;
    }
}

int main(void)
{
    if (!glfwInit()) {
        *ErrorStream << "Erreur: Impossible d'initialiser GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(1280, 720, "UltimateRaycastingEngine", NULL, NULL);
    if (!window)
    {
        *ErrorStream << "Erreur: Impossible de creer la fenetre." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        *ErrorStream << "Erreur: Impossible d'initialiser GLEW." << std::endl;
        return -1;
    }
    
    Init();

    glfwSetKeyCallback(window, KeyCallback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0.5f, 0);
    float previousFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float deltaTime = glfwGetTime() - previousFrameTime;
        previousFrameTime = glfwGetTime();

        glm::dvec2 cursorPos(0, 0);
        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);

        Compute(deltaTime, cursorPos);
        Render(deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Stop();
    glfwTerminate();
    return 0;
}