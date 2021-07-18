
#include <iostream>
#include <list>

#include "platform.h"
#include "render.h"

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
float SCR_ASPECT = (float)SCR_WIDTH / (float)SCR_HEIGHT;


// SCR_WHATEVER was here


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


// Settings


char TITLE[256] = "Sunshine, an effigy.";

Shader* shaders[10];
GameState GS;
int WindowFocus;

#include "Collision.h"

void InitDebug()
{
    AABB DPC = {};
    DPC.position = v2{0,0};
    DPC.velocity = v2{100,0};
    DPC.size = v2{10,10};

    AABB TPF = {};
    TPF.position = {50, 0};
    TPF.size = {10,1000};

    CollisionResult thistime = SweptAABBvAABB(&DPC, &TPF);

    AABB boxa = AABB{ -10, -10,
                       10, 10 };
    AABB boxb = AABB{ -5, -5,
                       10, 10 };
    bool onground = false;

    if (AABBvAABB(&boxa, &boxb))
    {
        onground = true;
    }

}

void ResetPlayer()
{
    GS.PC->position = v2{ (float)SCR_WIDTH / 2, (float)SCR_HEIGHT / 2 };
    GS.PC.velocity = v2{ -10, 100 };
    GS.PC.redHook->state = READY;
}

void InitGame()
{
    GS.Time = (float)glfwGetTime();

    GS.Platforms.push_back(AABB{ 0, 0, 1280, 30 });
    GS.Platforms.push_back(AABB{ 40, 240, 200, 40 });
    GS.Platforms.push_back(AABB{ 1040, 240, 200, 40 });

    GS.Platforms.push_back(AABB{ 590, 500, 40, 40 });
    GS.Platforms.push_back(AABB{ 440, 500, 40, 40 });
    GS.Platforms.push_back(AABB{ 1100, 440, 140, 40 });

    GS.Platforms.push_back(AABB{ 900, 200, 140, 440 });
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

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    InitDebug();
    InitGame();

    // Main Loop =============================
    while (!glfwWindowShouldClose(window))
    {
        // Input
        glfwPollEvents();
        processInput(window);
        bool mouseinbox = GS.Input.mx < SCR_WIDTH * 0.75 && GS.Input.mx > SCR_WIDTH * 0.25 &&
            GS.Input.my < SCR_HEIGHT * 0.75 && GS.Input.my > SCR_HEIGHT * 0.25;

        // Update ============================
        GS.dT = (float)glfwGetTime() - GS.Time;
        GS.Time += GS.dT;
        GS.PC.Update(&GS);

        if (GS.PC.position.y < -1000)
        {
            ResetPlayer();
        }

        // Render ============================
        glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
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

        std::list<AABB>::iterator platformitr = GS.Platforms.begin();
        for (int ii = 0; ii < GS.Platforms.size(); ++ii)
        {
            FancyDrawAABB(*platformitr, Color { 0.44f, 0.0f, 0.44f }, shaders[0]);
            ++platformitr;
        }

        GS.PC.Render(shaders[0]);

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

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
        ResetPlayer();

    WindowFocus = glfwGetWindowAttrib(window, GLFW_FOCUSED);

    // Controllers =====================================
    // ALL buttons downprev
    for (int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
    {
        GS.Input.virtualcontroller.button[ii].downprevious = GS.Input.virtualcontroller.button[ii].down;
    }

    // assume everything isn't down
    for (int jj = 0; jj < CONTROLLER_BUTTONS; ++jj)
    {
        GS.Input.virtualcontroller.button[jj].down = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        GS.Input.virtualcontroller.a.down += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        GS.Input.virtualcontroller.left.down += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        GS.Input.virtualcontroller.right.down += 1;
    }
    
    // wentup/wentdown
    for (int ii = 0; ii < CONTROLLER_BUTTONS; ++ii)
    {
        GS.Input.virtualcontroller.button[ii].wentdown = (GS.Input.virtualcontroller.button[ii].down > 0
                                               && GS.Input.virtualcontroller.button[ii].downprevious == 0);
        GS.Input.virtualcontroller.button[ii].wentup = (GS.Input.virtualcontroller.button[ii].down == 0
                                               && GS.Input.virtualcontroller.button[ii].downprevious > 0);
    }

    // Mice =============================================
    // mouse previous position
    GS.Input.mousexprevious = GS.Input.mousex;
    GS.Input.mouseyprevious = GS.Input.mousey;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my); // in window coords already

    GS.Input.mousex = (int)mx;
    GS.Input.mousey = (int)SCR_HEIGHT - (int)my;

    for (int ii = 0; ii < MOUSE_BUTTONS; ++ii)
    {
        GS.Input.mousebutton[ii].downprevious = GS.Input.mousebutton[ii].down;
    }

    // mouse buttons: L, R, M, X1, X2
    if (WindowFocus) // || INACTIVE_INPUT)
    {
        GS.Input.mousebutton[0].down = !!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
        GS.Input.mousebutton[1].down = !!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT));
        GS.Input.mousebutton[2].down = !!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE));
        GS.Input.mousebutton[3].down = !!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4));
        GS.Input.mousebutton[4].down = !!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5));
    }

    for (int ii = 0; ii < MOUSE_BUTTONS; ++ii)
    {
        GS.Input.mousebutton[ii].wentdown = (GS.Input.mousebutton[ii].down > 0
            && GS.Input.mousebutton[ii].downprevious == 0);
        GS.Input.mousebutton[ii].wentup = (GS.Input.mousebutton[ii].down == 0
            && GS.Input.mousebutton[ii].downprevious > 0);
    }

}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

