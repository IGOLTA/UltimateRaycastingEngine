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

GLuint floorCeilingTexture;
Texture* ceilingColor;
Texture* floorColor;

GLuint lightsTexture;

int SCANLINES = 1000;
int RAYS = 1000;
float TILE_PER_SPRITE = 5;
float HEIGHT_FACTOR = 5;

bool forward = false;
bool backward = false;
bool left = false;
bool right = false;

float lastMouseX = -1;

Map* map;
Player* player;

void SetupMap() {
    map = new Map(glm::ivec2(50, 50));
    map->ambiantLight = glm::vec3(1, 1, 1);
    Light l;
    l.color = glm::vec3(0.8, 0.8, 0);
    l.position = glm::vec3(0, 0, 0.5f);
    map->lights.push_back(l);

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

void SetupRaycastTexture() {
    // Generate texture
    glGenTextures(1, &raycastTexture);
    glBindTexture(GL_TEXTURE_1D_ARRAY, raycastTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void SetupFloorCeilingTextures() {
    //Data
    // Generate texture
    glGenTextures(1, &floorCeilingTexture);
    glBindTexture(GL_TEXTURE_1D_ARRAY, floorCeilingTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    ceilingColor = new Texture("resources/ceiling.jpg");
    floorColor = new Texture("resources/floor.jpg");
}

void SetupLightsTexture() {
    // Generate texture
    glGenTextures(1, &lightsTexture);
    glBindTexture(GL_TEXTURE_1D_ARRAY, lightsTexture);

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
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
}

void DeleteRaycastTexture() {
    glDeleteTextures(0, &raycastTexture);
}

void DeleteFloorCeilingTextures() {
    glDeleteTextures(0, &floorCeilingTexture);
    delete ceilingColor;
    delete floorColor;
}

void DeleteLightsTexture() {
    glDeleteTextures(0, &lightsTexture);

}

void Init() {
    Tile::LoadTiles();
    SetupMap();
    SetupPlayer();
    SetupLightsTexture();
    SetupRaycastTexture();
    SetupFloorCeilingTextures();
    SetupRenderQuad();
    Program::LoadShaders();
}

void Stop() {
    Tile::UnloadTiles();
    delete player;
    delete map;
    DeleteRenderQuad();
    DeleteRaycastTexture();
    DeleteFloorCeilingTextures();
    DeleteLightsTexture();
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

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    /* Lights */
    GLfloat* lights = new GLfloat[map->lights.size() * 6];

    for (int i = 0; i < map->lights.size(); i++) {
        lights[6 * i] = map->lights[i].position.x;
        lights[6 * i + 1] = map->lights[i].position.y;
        lights[6 * i + 2] = map->lights[i].position.z;

        lights[6 * i + 3] = map->lights[i].color.x;
        lights[6 * i + 4] = map->lights[i].color.y;
        lights[6 * i + 5] = map->lights[i].color.z;
    }

    glBindTexture(GL_TEXTURE_1D_ARRAY, lightsTexture);
    glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_RGB32F, map->lights.size(), 2, 0, GL_RGB, GL_FLOAT, lights);
    glGenerateMipmap(lightsTexture);

    delete[] lights;

    /*Floor and Ceiling*/
    glUseProgram(Program::GetShaderId("floor_and_ceiling"));
    //Ceiling
    GLfloat* texCoordData = new GLfloat[SCANLINES * 4];
    player->ceilingScan(SCANLINES, 2* HEIGHT_FACTOR, texCoordData);

    glBindTexture(GL_TEXTURE_1D_ARRAY, floorCeilingTexture);
    glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_R32F, SCANLINES, 4, 0, GL_RED, GL_FLOAT, texCoordData);
    glGenerateMipmap(floorCeilingTexture);

    delete[] texCoordData;

    glUniform3f(glGetUniformLocation(Program::GetShaderId("floor_and_ceiling"), "ambiant"), map->ambiantLight.x, map->ambiantLight.y, map->ambiantLight.z);

    //Draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D_ARRAY, floorCeilingTexture);
    glUniform1i(glGetUniformLocation(Program::GetShaderId("floor_and_ceiling"), "texCoordData"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *ceilingColor);
    glUniform1i(glGetUniformLocation(Program::GetShaderId("floor_and_ceiling"), "ceilingTexture"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, *floorColor);
    glUniform1i(glGetUniformLocation(Program::GetShaderId("floor_and_ceiling"), "floorTexture"), 2);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /*Walls*/
    glUseProgram(Program::GetShaderId("raycast_walls"));
    //Raycast
    GLfloat* raycastDatas = new GLfloat[(RAYS * 2 + 1) * 5];

    for (int i = RAYS; i >= -RAYS; i = i - 1) {
        float angle = i / (float)RAYS;

        RayHit hit = player->rayCast(angle);

        int index = RAYS - i;
        int offset = RAYS * 2 + 1;
        
        //Distance
        raycastDatas[index] = hit.distance;
        //IDs
        raycastDatas[index + 1 * offset] = hit.tileId;
        //Positions
        raycastDatas[index + 2 * offset] = hit.position.x;
        raycastDatas[index + 3 * offset] = hit.position.y;
        //Side
        raycastDatas[index + 4 * offset] = hit.side;
    }

    glUniform1f(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "HEIGHT_FACTOR"), HEIGHT_FACTOR);
    glUniform1f(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "TILE_PER_SPRITE"), TILE_PER_SPRITE);

    glBindTexture(GL_TEXTURE_1D_ARRAY, raycastTexture);
    glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_R32F, RAYS * 2 + 1, 5, 0, GL_RED, GL_FLOAT, raycastDatas);
    glGenerateMipmap(raycastTexture);
    delete[] raycastDatas;

    //Lights
    glUniform3f(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "ambiant"), map->ambiantLight.x, map->ambiantLight.y, map->ambiantLight.z);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightsTexture);
    glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "lightsData"), 2);
    
    glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "lightsCount"), map->lights.size());

    //Draw
    for (int i = 1; i <= Tile::GetTileNumber(); i++) {
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "currentTile"), i);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D_ARRAY, raycastTexture);
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "raycastData"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *Tile::GetTile(i)->getBaseTexture());
        glUniform1i(glGetUniformLocation(Program::GetShaderId("raycast_walls"), "baseColor"), 1);
    
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

void SizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Render();
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
    glfwSetFramebufferSizeCallback(window, SizeCallback);

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
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Stop();
    glfwTerminate();
    return 0;
}