#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry {
public:
    Sphere(float radius = 1.0f, int segments = 16) : Geometry(SPHERE), segments_(segments)
    {
        setRadius(radius);
    }
    
    // 获取半径的公共接口
    float getRadius() const { return radius_; }
    int getSegments() const { return segments_; }
    
    // 主要接口：设置球半径，并会调用相应接口进行绘制
    void setRadius(float radius) {
        radius_ = radius;
        generateGeometry();
    }
    
private:
    float radius_;     // 球体半径
    int segments_;     // 球体分段数（控制精度）
    
    // 生成球体几何
    void generateGeometry() {
        clear();
        
        auto& vertices = getVertices();
        auto& indices = getIndices();
        
        // 生成顶点
        for (int i = 0; i <= segments_; ++i) {
            // 垂直角度（纬度）：从 -π/2 到 π/2
            float pitch = glm::pi<float>() * (static_cast<float>(i) / segments_ - 0.5f);
            float cosPitch = glm::cos(pitch);
            float sinPitch = glm::sin(pitch);
            
            for (int j = 0; j <= segments_; ++j) {
                // 水平角度（经度）：从 0 到 2π
                float yaw = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments_;
                float cosYaw = glm::cos(yaw);
                float sinYaw = glm::sin(yaw);
                
                // 计算顶点位置（球心在原点）
                glm::vec3 position(
                    radius_ * cosPitch * cosYaw,    // x
                    radius_ * sinPitch,             // y
                    radius_ * cosPitch * sinYaw     // z
                );
                
                // 法线就是归一化的位置向量（因为球心在原点）
                glm::vec3 normal = glm::normalize(position);
                
                // 计算纹理坐标（UV映射）
                glm::vec2 texCoord(
                    static_cast<float>(j) / segments_,  // u = 经度比例
                    static_cast<float>(i) / segments_   // v = 纬度比例
                );
                
                vertices.emplace_back(position, normal, texCoord);
            }
        }
        
        // 生成三角形（四边形拆分成两个三角形）
        for (int i = 0; i < segments_; ++i) {
            for (int j = 0; j < segments_; ++j) {
                // 当前四边形的四个顶点索引
                unsigned int v0 = i * (segments_ + 1) + j;          // 左上
                unsigned int v1 = v0 + 1;                           // 右上
                unsigned int v2 = v1 + (segments_ + 1);            // 右下
                unsigned int v3 = v0 + (segments_ + 1);            // 左下
                
                // 添加两个三角形（逆时针顺序）顶点到 indices 中
                indices.insert(indices.end(), {v0, v1, v2, v0, v2, v3});
            }
        }
        
        // 初始化缓冲区（如果已存在则更新）
        if (!initBuffers()) {
            std::cout << "Error: Failed to init Sphere's Buffers" << std::endl;
        }
    }
};

#endif