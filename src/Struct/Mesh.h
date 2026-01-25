#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include <vector>

// 三角形索引结构体：存储Mesh顶点列表的索引，实现顶点复用
struct Triangle {
    unsigned int v[3];  // 使用unsigned int以匹配OpenGL索引类型
    
    Triangle() : v{0, 0, 0} {}
    Triangle(unsigned int v0, unsigned int v1, unsigned int v2) : v{v0, v1, v2} {}
};

// 立体图形网格结构体：统一封装顶点数据和三角形索引
// 一个Mesh对应一个完整3D图形，并支持VAO/VBO/EBO
struct Mesh {
    std::vector<Vertex> vertices;      // 顶点列表（存储所有唯一顶点）
    std::vector<Triangle> triangles;   // 三角形列表
    
    // OpenGL对象ID
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    
    Mesh() = default;
    
    ~Mesh() {
        cleanup();
    }
    
    void createTestTriangle() {
        clear();
        
        // 简单彩色三角形
        vertices = {
            Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 
                glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)), // 红
            Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 
                glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)), // 绿
            Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 
                glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))  // 蓝
        };
        
        triangles = {
            Triangle(0, 1, 2)
        };
        
        setupBuffers();
    }

    // 清理OpenGL资源
    void cleanup() {
        if (EBO) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
    }
    
    // 初始化OpenGL缓冲区
    bool setupBuffers() {
        // 清理旧的OpenGL对象
        cleanup();
        
        if (vertices.empty() || triangles.empty()) {
            return false;
        }
        
        // 1. 创建并绑定VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        // 2. 创建并绑定VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     vertices.size() * sizeof(Vertex),
                     vertices.data(),
                     GL_STATIC_DRAW);
        
        // 3. 创建并绑定EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
        // 将Triangle结构体数组转换为连续的索引数组
        std::vector<unsigned int> indices;
        indices.reserve(triangles.size() * 3);
        for (const auto& tri : triangles) {
            indices.push_back(tri.v[0]);
            indices.push_back(tri.v[1]);
            indices.push_back(tri.v[2]);
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int),
                     indices.data(),
                     GL_STATIC_DRAW);
        
        // 4. 设置顶点属性指针
        // 位置属性 (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::positionOffset());
        glEnableVertexAttribArray(0);
        
        // 法线属性 (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::normalOffset());
        glEnableVertexAttribArray(1);
        
        // 纹理坐标属性 (location = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::texCoordOffset());
        glEnableVertexAttribArray(2);

        // 颜色属性 (location = 3)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::colorOffset());
        glEnableVertexAttribArray(3);
        
        // 5. 解绑VAO
        glBindVertexArray(0);

        return true;
    }
    
    // 渲染网格
    void render() const {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!" << std::endl;
            return;
        }
        
        // 检查是否有顶点数据
        if (vertices.empty() || triangles.empty()) {
            std::cerr << "No data to render!" << std::endl;
            return;
        }
        
        glBindVertexArray(VAO);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error binding VAO: " << err << std::endl;
        }  
        
        glDrawElements(GL_TRIANGLES, 
                    triangles.size() * 3,
                    GL_UNSIGNED_INT, 
                    0);
        
        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error in glDrawElements: " << err << std::endl;
        }
        
        glBindVertexArray(0);
    }
    
    // 清空网格数据
    void clear() {
        vertices.clear();
        triangles.clear();
        cleanup();
    }
};

#endif