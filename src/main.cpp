// ===================== 路径宏定义 =====================

#ifndef RESOURCE_PATH_H
#define RESOURCE_PATH_H

#define PROJECT_ROOT "F:/OpenGL-learning/"
#endif

#define IMAGE_DIR PROJECT_ROOT "Image/"
#define IMAGE_PATH(filename) IMAGE_DIR filename

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Shader/Shader.h>
#include <Struct/Vertex.h>
#include <Struct/Mesh.h>
#include <Struct/Cuboid.h>
#include <Struct/Sphere.h>

#ifdef _WIN32
    #include <windows.h>
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void printOperationTips();
unsigned int loadTexture(char const * path);

// window 设置
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera 设置
glm::vec3 cameraPos     = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront   = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp      = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// 初始化为 -90，否则 z 轴初始指向右侧
float pitch =  0.0f;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;

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

int lastLState = GLFW_RELEASE;
int lastEState = GLFW_RELEASE;
int lastTState = GLFW_RELEASE;
int lastCState = GLFW_RELEASE;

// position
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};


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

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 加载纹理
    // ------------------------------
    unsigned int diffsueMap = loadTexture(IMAGE_PATH("container2.png"));
    unsigned int specularMap = loadTexture(IMAGE_PATH("container2_specular.png"));

    // 创建球体着色器
    // ------------------------------
    Shader cuboidShader("Shader.vs", "Shader.fs");
    // 绑定对应纹理单元
    cuboidShader.use();
    cuboidShader.setInt("material.diffuse",  0);
    cuboidShader.setInt("material.specular", 1);
    
    // 创建光源着色器
    // ------------------------------
    Shader lightShader("lightShader.vs", "lightShader.fs");

    // 创建对应立方体和光源立方体
    // ------------------------------------------------------------------
    Cuboid cuboid;
    Cuboid LightCuboid;

    // 输出操作提示信息
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

        // 绘制
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 根据需求 按线框/填充模式绘制球体
        if (is_fill) {
            // 恢复为填充模式（默认）
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            // 设置多边形模式为线框
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        // 创造变换
        // ------------------------------------

        glm::mat4 model         = glm::mat4(1.0f); // 确保一开始被初始化为单位矩阵
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        

        float radius = 10.0f;
        
        view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        
        if (is_lightRotate) {
            float lightRotateTime = currentFrame - lightRotateStartTime;
            // 开启光源旋转
            // 计算光源在X-Z平面上的旋转位置
            lightPos.x = lightRotateRadius * sin(lightRotateTime * lightRotateSpeed);
            lightPos.z = lightRotateRadius * cos(lightRotateTime * lightRotateSpeed);
            lightPos.y = 1.0f; 
        }

        if (is_lightColorChange) {
            float lightColorChangeTime = currentFrame - lightColorChangeStartTime;
            lightColor.x = sin(lightColorChangeTime * 2.0f);
            lightColor.y = sin(lightColorChangeTime * 0.7f);
            lightColor.z = sin(lightColorChangeTime * 1.3f);

            ambientColor = lightColor * glm::vec3(0.2f, 0.2f, 0.2f);
            diffuseColor = lightColor * glm::vec3(0.5f, 0.5f, 0.5f);
        } else {
            lightColor = glm::vec3(1.0f);
            ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
            diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
        }
        
        // 定义立方体的着色器
        cuboidShader.use();
        cuboidShader.setVec3("viewPos", cameraPos);

        // 设置立方体的其他光照参数
        cuboidShader.setFloat("material.shininess", 64.0f);

        // 设置立方体的光照位置和强度
        cuboidShader.setVec3("lightColor", glm::vec3(1.0f));
        cuboidShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        cuboidShader.setVec3("light.ambient",  ambientColor);
        cuboidShader.setVec3("light.diffuse",  diffuseColor); // 将光照调暗了一些以搭配场景
        cuboidShader.setVec3("light.specular", 0.5f, 0.5f, 0.5f); 

        // model = glm::mat4(1.0f);
        // cuboidShader.setMat4("model", model);
        cuboidShader.setMat4("view", view);
        cuboidShader.setMat4("projection", projection);

        // 绑定纹理到对应的纹理单元
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffsueMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cuboidShader.setMat4("model", model);

            cuboid.render();
        }

        // 调用立方体渲染借口
        // cuboid.render();

        if (is_renderLight)
        {
            // 定义光源着色器
            // ------------------------------------
            lightShader.use();
            // 始终为填充模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // 为光源着色器定义模型位置和视角关系
            LightCuboid.setPosition(lightPos);
            LightCuboid.setScale(glm::vec3(0.2f));

            lightShader.setVec3("lightColor", lightColor);
            lightShader.setMat4("model", LightCuboid.getModelMatrix());
            lightShader.setMat4("view", view);
            lightShader.setMat4("projection", projection);

            // 渲染立方体
            LightCuboid.render();
        }

        // glfw: 交换缓冲区并拉取 IO 事件
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: 终止
    // -----------------
    glfwTerminate();
    // 其中资源会由 Mesh 类中的析构函数回收
    return 0;
}


// 处理所有输入：查询 GLFW 本帧中相关按键是否被按下/释放，并相应地做出反应
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}


// 滚动回调函数，实现通过滚轮进行一定的缩放
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
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
    std::cout << "  鼠标滚轮 - 缩放视野" << std::endl;
    std::cout << std::endl;
    
    std::cout << "功能切换:" << std::endl;
    std::cout << "  L - 切换填充模式(线框/填充)" << std::endl;
    std::cout << "  E - 切换光源是否显示" << std::endl;
    std::cout << "  T - 切换光源是否旋转" << std::endl;
    std::cout << "  C - 切换光源是否变色" << std::endl;
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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