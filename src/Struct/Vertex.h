#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <glad/glad.h>

// 顶点数据结构 - 使用glm类型便于计算，但存储为连续内存
struct Vertex {
    // 数据成员 - 确保内存连续布局
    glm::vec3 position;     // 顶点三维空间坐标
    glm::vec3 normal;       // 顶点法向量（用于光照计算）
    glm::vec2 texCoord;     // 纹理坐标（u, v）
    glm::vec3 color;        // 顶点颜色
    
    // 默认构造函数
    Vertex() : position(0.0f), normal(0.0f, 0.0f, 1.0f), texCoord(0.0f), color(0.0f) {}
    
    // 使用 glm::vec 输入的构造函数
    Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f, 0.0f, 1.0f), 
           const glm::vec2& tex = glm::vec2(0.0f), const glm::vec3 col = glm::vec3(0.5f, 0.5f, 0.0f))
        : position(pos), normal(norm), texCoord(tex), color(col) {}
    
    // 使用float指针输入的构造函数
    Vertex(const float* pos, const float* norm = nullptr, 
        const float* tex = nullptr, const float* col = nullptr) {
        position = glm::vec3(pos[0], pos[1], pos[2]);
        if (norm) {
            normal = glm::vec3(norm[0], norm[1], norm[2]);
        } else {
            normal = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        if (tex) {
            texCoord = glm::vec2(tex[0], tex[1]);
        } else {
            texCoord = glm::vec2(0.0f);
        }
        if (col) {
            color = glm::vec3(col[0], col[1], col[2]);
        } else {
            color = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }
    
    // 获取指向数据的指针（用于VBO上传）
    const float* data() const {
        return &position.x;  // position是第一个成员，位置是连续的
    }
    
    // 获取顶点数据的总大小（字节）
    static size_t size() {
        return sizeof(Vertex);
    }
    
    // 获取位置属性的偏移量
    static size_t positionOffset() {
        return offsetof(Vertex, position);
    }
    
    // 获取法线属性的偏移量
    static size_t normalOffset() {
        return offsetof(Vertex, normal);
    }
    
    // 获取纹理坐标属性的偏移量
    static size_t texCoordOffset() {
        return offsetof(Vertex, texCoord);
    }
    static size_t colorOffset() {
        return offsetof(Vertex, color);
    }
};

#endif