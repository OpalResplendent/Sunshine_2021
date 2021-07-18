
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <list>

#include "lazymath.cpp"
#include "textrendering.cpp"
#include "shaders.cpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Settings
int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
float SCR_ASPECT = (float)SCR_WIDTH / (float)SCR_HEIGHT;

char TITLE[256] = "Sunshine, an effigy.";

Shader* shaders[10];

enum HOOK_STATE { READY, COOLDOWN, DEPLOYING, EMBEDDED, RETURNING };

struct GrapplingHook {
    HOOK_STATE state;

    v2 position;
    v2 velocity;

    float pullStrength;
    std::list <v2> points;
};

struct PlayerCharacter {
    v2 position;
    v2 velocity;
    float gravity;
    float drag;

    GrapplingHook redHook;
    GrapplingHook blueHook;
};

struct GameState {
    PlayerCharacter PC;
    float Time;
    float dT;
};
GameState GS;


struct InputBlock {
    double mx, my;
    float lx, ly; // left x, left y
};

InputBlock Input;

void InitGame()
{
    GS.Time = (float)glfwGetTime();
}

// lower left xy, w, h
// we gonna draw a lotta squares, so make this not dumb later
void DrawSquare(glm::vec4 square, glm::vec3 color)
{
    float square[20] = {
        square.x, square.y, color.r, color.g, color.b, // ll
        square.x + square.z, square.y, color.r, color.g, color.b, // lr
        square.x, square.y + square.w, color.r, color.g, color.b, // ul
        square.x + square.z, square.y + square.w, color.r, color.g, color.b // ur
    };
    unsigned int elements[6]{
        0, 1, 2,
        1, 3, 2
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawRope(GrapplingHook* hook, Shader* shader, glm::vec3 color)
{
    unsigned int numtriangles = (hook->points.size()-1)*2;
    unsigned int pointsbytes = sizeof(float) * hook->points.size() * 12 * 2;
    unsigned int elementsbytes = sizeof(unsigned int) * numtriangles * 3;
    float* points = (float*)malloc(pointsbytes);
    unsigned int* elements = (unsigned int*)malloc(elementsbytes);

    std::list<v2>::iterator itr = hook->points.begin();
    v2 current = *itr++;
    v2 next = *itr;
    //std::list<v2>::iterator next = current++;

    // first
    v2 direction = DirectionV2(current, next);
    v2 rightangle = RotateV2Right(direction);
    unsigned int index = 0;

    points[index +  0] = (current + (rightangle * 4.04)).x;
    points[index +  1] = (current + (rightangle * 4.04)).y;
    points[index +  2] = 0.1f;
    points[index +  3] = color.r;
    points[index +  4] = color.g;
    points[index +  5] = color.b;

    points[index +  6] = (current - (rightangle * 4.04)).x;
    points[index +  7] = (current - (rightangle * 4.04)).y;
    points[index +  8] = 0.1f;
    points[index +  9] = color.r;
    points[index + 10] = color.g;
    points[index + 11] = color.b;

    //current++;
    index += 12;

    // middle
    for (int ii = 1; ii < hook->points.size() - 1; ++ii)
    {

    }

    // last
    direction = -direction;
    rightangle = RotateV2Right(direction);
    current = next;
    
    points[index + 0] = (current + (rightangle * 4.04)).x;
    points[index + 1] = (current + (rightangle * 4.04)).y;
    points[index + 2] = 0.1f;
    points[index + 3] = color.r;
    points[index + 4] = color.g;
    points[index + 5] = color.b;

    points[index + 6] = (current - (rightangle * 4.04)).x;
    points[index + 7] = (current - (rightangle * 4.04)).y;
    points[index + 8] = 0.1f;
    points[index + 9] = color.r;
    points[index + 10] = color.g;
    points[index + 11] = color.b;

    //current++;
    //index += 12;

    // element index
    for (int ii = 0; ii < numtriangles/2; ++ii)
    {
        elements[ii * 6 + 0] = ii + 0;
        elements[ii * 6 + 1] = ii + 1;
        elements[ii * 6 + 2] = ii + 2;
        elements[ii * 6 + 3] = ii + 0;
        elements[ii * 6 + 4] = ii + 3;
        elements[ii * 6 + 5] = ii + 2;
    }

    // render
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pointsbytes, points, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementsbytes, elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glUseProgram(shaderProgram);
    shader->use();
    glBindVertexArray(VAO);
    shader->use();

    glDrawElements(GL_TRIANGLES, numtriangles*3, GL_UNSIGNED_INT, 0);

    // free
    free(points);
    free(elements);
}

void UpdatePlayer()
{
    GS.PC.redHook.position = v2{ (float)Input.mx, (float)(SCR_HEIGHT - Input.my) };
    GS.PC.redHook.state = DEPLOYING;

    if (GS.PC.redHook.state != READY
        && GS.PC.redHook.state != COOLDOWN)
    {
        GS.PC.redHook.points.clear();
        GS.PC.redHook.points.push_front(GS.PC.position);
        GS.PC.redHook.points.push_back(GS.PC.redHook.position);
    }
};

void RenderPlayer()
{
    if ( GS.PC.redHook.state != READY
        && GS.PC.redHook.state != COOLDOWN )
    {
        DrawRope(&GS.PC.redHook, shaders[0], glm::vec3{ 0.8f, 0.1f, 0.1f });
    }
}

int main()
{
    // OpenGL Initialization ==================
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader Initialization ==================
    shaders[0] = new Shader("rope.vert", "rope.frag");

    // VBO ====================================
    float vertices[] = {
         100.5f,  100.5f, 01.0f,  0.05f, 0.2f, 0.05f,
         100.5f, -100.0f, 01.0f,  0.05f, 0.2f, 0.05f,
         -100.0f, -100.0f, 01.0f,  0.05f, 0.2f, 0.05f,
         -100.0f,  100.5f, 01.0f,  0.05f, 0.2f, 0.05f
    };
    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    InitGame();

    // Main Loop =============================
    while (!glfwWindowShouldClose(window))
    {
        // Input
        glfwPollEvents();
        processInput(window);
        bool mouseinbox = Input.mx < SCR_WIDTH*0.75 && Input.mx > SCR_WIDTH*0.25 &&
                            Input.my < SCR_HEIGHT*0.75 && Input.my > SCR_HEIGHT*0.25;

        // Update ============================
        GS.dT = (float)glfwGetTime() - GS.Time;
        GS.Time += GS.dT;
        UpdatePlayer();

        // Render ============================
        glClearColor(0.4f, 0.0f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // set matrixes
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f)); // pan
        projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT,
                    0.1f, 100.0f); // 2d screensize
        projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT); // 2d screensize

        shaders[0]->use();
        shaders[0]->setMat4("model", model);
        shaders[0]->setMat4("view", view);
        shaders[0]->setMat4("projection", projection);
        
        glBindVertexArray(VAO);
        shaders[0]->use();
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        RenderPlayer();

        // Present
        glfwSwapBuffers(window);
    }

    // OS takes care of most deallocation
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glfwGetCursorPos(window, &Input.mx, &Input.my);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

