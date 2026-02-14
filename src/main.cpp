#define _HAS_STD_BYTE 0          // 禁用C++17的std::byte，避免和Windows的byte冲突
#define WIN32_LEAN_AND_MEAN      // 精简Windows头文件，减少冗余定义

// ===================== 路径宏定义 =====================
#ifndef RESOURCE_PATH_H
#define RESOURCE_PATH_H

#define PROJECT_ROOT "F:/OpenGL-learning/"
#endif

#define IMAGE_DIR PROJECT_ROOT "Image/"
#define IMAGE_PATH(filename) IMAGE_DIR filename

#define MODEL_DIR PROJECT_ROOT "Model/"
#define MODEL_PATH(filename) MODEL_DIR filename

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <stb_image.h>

#include "Shader/Shader.h"
#include "Struct/Vertex.h"
#include "Struct/Mesh.h"
#include "Struct/Cuboid.h"
#include "Struct/Sphere.h"
#include "Struct/Model.h"

#include "Camera/Camera.h"

#ifdef _WIN32
    #include <windows.h>
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void printOperationTips();
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(vector<std::string> faces);

// window 设置
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 折射率设置
const float REFRACTIVE_INDEX_WATER  = 1.33f;
const float REFRACTIVE_INDEX_AIR    = 1.00f;
const float REFRACTIVE_INDEX_ICE    = 1.309f;
const float REFRACTIVE_INDEX_GLASS  = 1.52f;
const float REFRACTIVE_INDEX_DIAMOND= 2.42f;

// 几何着色器 相关设置
const float EXPLODE_MAGNITUDE = 2.0f;
const float NORMAL_OFFSET = 0.2;

// camera 设置
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

bool firstMouse = true;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lightRotateStartTime = 0.0f; // 灯光开始旋转的时间
float lightColorChangeStartTime = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float lightRotateRadius = 2.0f;
float lightRotateSpeed = 1.0f;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 ambientColor;
glm::vec3 diffuseColor;

// ui
bool is_fill = true;
bool is_renderLight = true;
bool is_lightRotate = false;
bool is_lightColorChange = false;
bool is_renderBorder = true;
bool is_faceCulling = false;

int lastLState = GLFW_RELEASE;
int lastEState = GLFW_RELEASE;
int lastTState = GLFW_RELEASE;
int lastCState = GLFW_RELEASE;
int lastBState = GLFW_RELEASE;
int lastQState = GLFW_RELEASE;

int main()
{
    // 避免终端中文乱码情况发生
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    // glfw: 初始化和配置
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 窗口创建
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 让 glfw 捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: 加载 OpenGL 所需指针
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用混合绘制
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // Back face (法向量: 0.0, 0.0, -1.0)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, // bottom-right 

         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f, // top-left

        // Front face (法向量: 0.0, 0.0, 1.0)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top-right

         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom-left

        // Left face (法向量: -1.0, 0.0, 0.0)
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-left

        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-right

        // Right face (法向量: 1.0, 0.0, 0.0)
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top-right     

        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bottom-left     

        // Bottom face (法向量: 0.0, -1.0, 0.0)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom-left

         0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, // top-right

        // Top face (法向量: 0.0, 1.0, 0.0)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top-left
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // top-right    
        
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f  // bottom-left        
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    float points[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
    };

    vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    windows.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    windows.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    windows.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    windows.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    
    glBindVertexArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    // point VAO
    unsigned int pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    // load 2D texture
    unsigned int cubeTexture  = loadTexture(IMAGE_PATH("container.jpg"));
    unsigned int floorTexture = loadTexture(IMAGE_PATH("metal.png"));

    // load cube texture
    vector<std::string> faces
    {
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg"
    };
    unsigned int skyboxTexture = loadCubemap(faces);

    // stbi_set_flip_vertically_on_load(true);
    // // load model
    // Model ourModel(MODEL_PATH("backpack/backpack.obj"));
    // Model ourModel(MODEL_PATH("nanosuit_reflection/nanosuit.obj"));

    // framebuffer configuration
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // create a renderbuffer object for depth and stencil attachement 
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // check 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // uniform buffer
    // 1. create uniform buffer object and allocate memory
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));


    // create Shader

    Shader shader("shader.vs", "Shader_Red.fs");
    Shader normalShader("geometryShader.vs", "geometryShader.fs", "check_normal.gs");
    normalShader.use();
    normalShader.setFloat("normal_offset", NORMAL_OFFSET);
    
    // 2. bind Shader's uniform block to binding point
    // 将 各着色器的 uniform 块绑定到绑定点 0 上
    // Shader shaderRed("uniformBufferShader.vs", "Shader_Red.fs");
    // Shader shaderGreen("uniformBufferShader.vs", "Shader_Green.fs");
    // Shader shaderBlue("uniformBufferShader.vs", "Shader_Blue.fs");
    // Shader shaderYellow("uniformBufferShader.vs", "Shader_Other.fs");

    // unsigned int uniformBlockIndexRed    = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
    // unsigned int uniformBlockIndexGreen  = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
    // unsigned int uniformBlockIndexBlue   = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
    // unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");  

    // glUniformBlockBinding(shaderRed.ID,    uniformBlockIndexRed, 0);
    // glUniformBlockBinding(shaderGreen.ID,  uniformBlockIndexGreen, 0);
    // glUniformBlockBinding(shaderBlue.ID,   uniformBlockIndexBlue, 0);
    // glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);


    // 3. bind the uniform buffer to binding point
    // 4. add data to the uniform buffer
    // 填充了投影矩阵到 uniform 缓冲区中，并且将其绑定到绑定点 0 上
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom_), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    printOperationTips();

    // 渲染主循环
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        // -----
        processInput(window);

        // render
        // ------

        // 阶段一：
        // 进行离屏渲染，将立方体和结果渲染到 framebuffer 上
        // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view 矩阵设定
        // 3. bind the uniform buffer to binding point
        // 4. add data to the uniform buffer
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindVertexArray(cubeVAO);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);
        normalShader.setMat3("normalMatrix", normalMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);


        // // Red cube
        // shaderRed.use();
        // model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
        // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        // shaderRed.setMat4("model", model);
        // shaderRed.setMat4("normalMatrix", normalMatrix);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        

        // // Green cube
        // shaderGreen.use();
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
        // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        // shaderGreen.setMat4("model", model);
        // shaderGreen.setMat4("normalMatrix", normalMatrix);
        // glDrawArrays(GL_TRIANGLES, 0, 36);


        // // Blue cube
        // shaderBlue.use();
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
        // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        // shaderBlue.setMat4("model", model);
        // shaderBlue.setMat4("normalMatrix", normalMatrix);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        

        // // Yellow cube
        // shaderYellow.use();
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
        // normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        // shaderYellow.setMat4("model", model);
        // shaderYellow.setMat4("normalMatrix", normalMatrix);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0);

        // // skybox
        // // 禁用深度写入，以保证天空盒一定在其他物体之后
        // glDepthFunc(GL_LEQUAL);

        // skyboxShader.use();
        // view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        // skyboxShader.setMat4("view", view);
        // skyboxShader.setMat4("projection", projection);
        // glActiveTexture(GL_TEXTURE0);
        // glBindVertexArray(skyboxVAO);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // // 重新启用深度写入
        // glDepthFunc(GL_LESS);

        // // 阶段二：
        // // 将帧缓冲场景绘制到主屏幕上
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // screenShader.use();
        // glBindVertexArray(quadVAO);
        // glDisable(GL_DEPTH_TEST);
        // glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        // glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: 交换缓冲区并拉取 IO 事件
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);

    // glfw: 终止
    // -----------------
    glfwTerminate();
    
    return 0;
}


// 处理所有输入：查询 GLFW 本帧中相关按键是否被按下/释放，并相应地做出反应
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    int currentLState = glfwGetKey(window, GLFW_KEY_L);
    if (lastLState == GLFW_RELEASE && currentLState == GLFW_PRESS) {
        is_fill = !is_fill;
    }
    lastLState = currentLState;

    int currentEState = glfwGetKey(window, GLFW_KEY_E);
    if (lastEState == GLFW_RELEASE && currentEState == GLFW_PRESS) {
        is_renderLight = !is_renderLight;
    }
    lastEState = currentEState;

    int currentTState = glfwGetKey(window, GLFW_KEY_T);
    if (lastTState == GLFW_RELEASE && currentTState == GLFW_PRESS) {
        is_lightRotate = !is_lightRotate;
        if (is_lightRotate) {
            lightRotateStartTime = glfwGetTime();
        }
    }
    lastTState = currentTState;

    int currentCState = glfwGetKey(window, GLFW_KEY_C);
    if (lastCState == GLFW_RELEASE && currentCState == GLFW_PRESS) {
        is_lightColorChange = !is_lightColorChange;
        if (is_lightColorChange) {
            lightColorChangeStartTime = glfwGetTime();
        }
    }
    lastCState = currentCState;

    int currentBState = glfwGetKey(window, GLFW_KEY_B);
    if (lastBState == GLFW_RELEASE && currentBState == GLFW_PRESS) {
        is_renderBorder = !is_renderBorder;
    }
    lastBState = currentBState;

    int currentQState = glfwGetKey(window, GLFW_KEY_Q);
    if (lastQState == GLFW_RELEASE && currentQState == GLFW_PRESS) {
        is_faceCulling = !is_faceCulling;
    }
    lastQState = currentQState;
}

// glfw: 每当窗口大小发生变化（由操作系统或用户自行调整）时，此回调函数就会执行。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 设置鼠标回调函数，用于视角转动
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 滚动回调函数，实现通过滚轮进行一定的缩放
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// 输出操作提示信息
void printOperationTips()
{
    std::cout << "=============== 操作提示 ===============" << std::endl;
    std::cout << "移动控制:" << std::endl;
    std::cout << "  W - 向前移动" << std::endl;
    std::cout << "  S - 向后移动" << std::endl;
    std::cout << "  A - 向左移动" << std::endl;
    std::cout << "  D - 向右移动" << std::endl;
    std::cout << std::endl;
    
    std::cout << "视角控制:" << std::endl;
    std::cout << "  鼠标移动 - 转动视角" << std::endl;
    // 暂时取消了缩放
    // std::cout << "  鼠标滚轮 - 缩放视野" << std::endl;
    std::cout << std::endl;
    
    std::cout << "功能切换:" << std::endl;
    // std::cout << "  L - 切换填充模式(线框/填充)" << std::endl;
    // std::cout << "  E - 切换光源是否显示" << std::endl;
    // std::cout << "  T - 切换光源是否旋转" << std::endl;
    // std::cout << "  C - 切换光源是否变色" << std::endl;
    // std::cout << "  B - 切换是否显示边框" << std::endl;
    // std::cout << "  Q - 切换是否正面剔除" << std::endl;
    std::cout << std::endl;
    
    std::cout << "其他:" << std::endl;
    std::cout << "  ESC - 退出程序" << std::endl;
    std::cout << "  调整窗口大小 - 自动更新视口" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

// 用于从文件加载二维纹理的辅助函数
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        std::string path = IMAGE_DIR + faces[i];
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}