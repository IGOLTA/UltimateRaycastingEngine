#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader_loader.h"
#include "map/Map.h"
#include "Player.h"

const int RAYS =5;
const float VIEW_DISTANCE = 50;

GLFWwindow* window;

GLuint raycastingShader;

//Render quad
GLuint quadVAO;
GLuint quadEBO;
GLuint quadVBO;

Map* map;
Player* player;

//PlayerMovement
bool forward = false;
bool backward = false;
bool left = false;
bool right = false;

float lastMouseX = -1;

//Textures
GLuint raycastTexture;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
        }
        break;
    }
}

void GameSetup() {
    map = new Map(glm::ivec2(50, 50));

    for (int i = 0; i < 5; i++) {
        Tile tile;
        tile.type = Tile::WALL;
        map->setTileAt(glm::ivec2(18, 5 + i), tile);
    }

    for (int i = 0; i < 30; i++) {
        Tile tile;
        tile.type = Tile::WALL;
        map->setTileAt(glm::ivec2(20, 5 + i), tile);
        map->setTileAt(glm::ivec2(20, 5 + i), tile);
    }


    player = new Player(map);

    player->setPosition(glm::ivec2(25, 25));
}

void CleanGame() {
    delete player;
    delete map;
}

void Compute(float deltaTime) {
    //Move player
    glm::vec2 move(0, 0);

    if (forward) move += glm::vec2(1, 0);
    if (backward) move += glm::vec2(-1, 0);
    if (left) move += glm::vec2(0, 1);
    if (right) move += glm::vec2(0, -1);

    player->move(move, deltaTime);

    double xpos;
    glfwGetCursorPos(window, &xpos, nullptr);
    float panDelta = xpos - lastMouseX;
    lastMouseX = xpos;
    player->pan(-panDelta);
}

void SetupRenderTextures() {
    // Generate a simple texture
    glGenTextures(1, &raycastTexture);
    glBindTexture(GL_TEXTURE_2D, raycastTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Raycast
    GLushort raycastDatas[RAYS * 2 + 1][1];
    for (int i = RAYS; i >= -RAYS; i = i - 1) {
        float angle = i / (float) RAYS;

        RayHit hit = player->rayCast(angle);

        float viewDist= hit.distance / VIEW_DISTANCE;
        if (viewDist > 1) viewDist = 1;

        raycastDatas[RAYS - i][0] = int(viewDist * USHRT_MAX);
    }
    std::cout << std::endl;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, RAYS * 2 + 1, 1, 0, GL_RED, GL_UNSIGNED_SHORT, raycastDatas);
    glGenerateMipmap(raycastTexture);

    glUseProgram(raycastingShader);
    glBindTexture(GL_TEXTURE_2D, raycastTexture);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

void CleanRenderQuad() {
    glDeleteBuffers(1, &quadEBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
}

void LoadShaders() {
    raycastingShader = LoadShaders("shaders/raycast_view/vert.glsl", "shaders/raycast_view/frag.glsl");
}

void CleanRenderTextures() {
    glDeleteTextures(1, &raycastTexture);
}

int main(void)
{
    if (!glfwInit()) {
        std::cerr << "Erreur: Impossible d'initialiser GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cerr << "Erreur: Impossible de creer la fenetre." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Erreur: Impossible d'initialiser GLEW." << std::endl;
        return -1;
    }

    GameSetup();

    SetupRenderQuad();
    SetupRenderTextures();
    LoadShaders();

    glfwSetKeyCallback(window, key_callback);

    float previousFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float deltaTime = glfwGetTime() - previousFrameTime;
        previousFrameTime = glfwGetTime();

        Compute(deltaTime);
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    CleanRenderQuad();
    CleanRenderTextures();
    CleanGame();
    glfwTerminate();
    return 0;
}

