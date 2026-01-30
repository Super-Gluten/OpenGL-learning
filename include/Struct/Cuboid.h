#ifndef CUBOID_H
#define CUBOID_H

#include "Geometry.h"

class Cuboid : public Geometry {
public:
    Cuboid(float width = 1.0f, float height = 1.0f, float depth = 1.0f): Geometry(CUBOID)
    {
        setSize(width, height, depth);
    }
    
    // 获取长宽高的公共接口
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    float getDepth() const { return depth_; }
    
    // 主要接口：设置立方体参数接口，并会调用相应接口进行绘制
    void setSize(float width, float height, float depth) {
        width_ = width;
        height_ = height;
        depth_ = depth;
        
        // 自动调用绘制函数
        generateGeometry();
    }

private:
    float width_;
    float height_;
    float depth_;
    
    void generateGeometry() {
        clear();
        
        auto& vertices = getVertices();
        auto& indices = getIndices();
        
        float halfW = width_ * 0.5f;
        float halfH = height_ * 0.5f;
        float halfD = depth_ * 0.5f;
        
        // 8个顶点
        glm::vec3 positions[8] = {
            glm::vec3(-halfW, -halfH, -halfD),  // 0: 左-下-前
            glm::vec3( halfW, -halfH, -halfD),  // 1: 右-下-前
            glm::vec3( halfW,  halfH, -halfD),  // 2: 右-上-前
            glm::vec3(-halfW,  halfH, -halfD),  // 3: 左-上-前
            glm::vec3(-halfW, -halfH,  halfD),  // 4: 左-下-后
            glm::vec3( halfW, -halfH,  halfD),  // 5: 右-下-后
            glm::vec3( halfW,  halfH,  halfD),  // 6: 右-上-后
            glm::vec3(-halfW,  halfH,  halfD)   // 7: 左-上-后
        };
        
        // 为每个顶点预分配归一化平滑法线（所有面法线平均）
        glm::vec3 smoothNormals[8] = {
            // 每个顶点是3个相邻面法线的平均
            glm::normalize(glm::vec3(-1, -1, -1)),  // 0: 左-下-前
            glm::normalize(glm::vec3( 1, -1, -1)),  // 1: 右-下-前
            glm::normalize(glm::vec3( 1,  1, -1)),  // 2: 右-上-前
            glm::normalize(glm::vec3(-1,  1, -1)),  // 3: 左-上-前
            glm::normalize(glm::vec3(-1, -1,  1)),  // 4: 左-下-后
            glm::normalize(glm::vec3( 1, -1,  1)),  // 5: 右-下-后
            glm::normalize(glm::vec3( 1,  1,  1)),  // 6: 右-上-后
            glm::normalize(glm::vec3(-1,  1,  1))   // 7: 左-上-后
        };
        
        // 纹理坐标
        glm::vec2 texCoords[4] = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(0.0f, 1.0f)
        };

        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.0f);
        
        // 每个面的顶点索引（按顺时针顺序）
        int faceIndices[6][4] = {
            {0, 1, 2, 3},  // 前
            {5, 4, 7, 6},  // 后
            {4, 5, 1, 0},  // 下
            {3, 2, 6, 7},  // 上
            {4, 0, 3, 7},  // 左
            {1, 5, 6, 2}   // 右
        };
        
        // 为每个面生成4个顶点和2个三角形
        for (int face = 0; face < 6; ++face) {
            unsigned int baseIndex = vertices.size();
            
            // 添加4个顶点
            for (int i = 0; i < 4; ++i) {
                int posIdx = faceIndices[face][i];
                vertices.emplace_back(positions[posIdx], smoothNormals[posIdx], texCoords[i], color);
            }
            
            // 添加2个三角形（逆时针）顶点到 indices 中
            indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3});
        }
        if (!initBuffers()) {
            std::cout << "Error: Failed to init Cuboid's Buffers" << std::endl;
        }
    }
};

#endif