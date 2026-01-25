#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

class Geometry {
public:
    enum Type {
        SPHERE,                 // 球体
        CUBOID,                 // 长方体（直棱柱）
        QUADRANGULAR_PRISM,            // 四棱柱
        UNKNOWN                 // 其他未知几何体
    };

    Geometry(Type type = UNKNOWN) : type_(type) {}
    virtual ~Geometry();

    // 禁止拷贝（因为管理OpenGL资源）
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;
    
    // 允许移动
    Geometry(Geometry&& other) noexcept;
    Geometry& operator=(Geometry&& other) noexcept;

    // 公共接口
    Type getType() const { return type_; }
    const Mesh& getMesh() const { return mesh_; }
    
    // 获取顶点和索引的引用（用于子类填充数据）
    std::vector<Vertex>& getVertices() { return mesh_.vertices; }
    std::vector<Triangle>& getTriangles() { return mesh_.triangles; }
    
    // 初始化OpenGL缓冲区（必须在OpenGL上下文初始化后调用）
    bool initBuffers() {
        return mesh_.setupBuffers();
    }
    
    // 渲染几何体
    void render() const {
        mesh_.render();
    }
    
    // 变换相关接口
    void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);
    void setRotation(const glm::vec3& rotation);
    // void setMaterial(const Material& material_); // 材质变换

    glm::vec3 getPosition() const { return position_; }
    glm::vec3 getScale() const { return scale_; }
    glm::vec3 getRotation() const { return rotation_; }
    glm::mat4 getModelMatrix() const;
    
    // 清空几何体数据
    void clear() {
        mesh_.clear();
    }
    
protected:
    Type type_;                // 几何体类型
    Mesh mesh_;                // 网格数据（包含VAO/VBO/EBO/model）
    
    // 变换数据
    glm::vec3 position_ = glm::vec3(0.0f);   // 几何体偏移
    glm::vec3 scale_ = glm::vec3(1.0f);      // 几何体缩放
    glm::vec3 rotation_ = glm::vec3(0.0f);   // 几何体欧拉角
    
    // 矩阵变换相关
    mutable bool modelMatrixDirty_ = false;
    mutable glm::mat4 modelMatrix_ = glm::mat4(1.0f);

    // 标记模型矩阵需要更新
    void markModelMatrixDirty() {
        modelMatrixDirty_ = true;
    }
};

// 实现移动到.cpp文件中
Geometry::~Geometry() {
    // 析构函数会自动清理Mesh中的OpenGL资源
}

Geometry::Geometry(Geometry&& other) noexcept
    : type_(other.type_),
      mesh_(std::move(other.mesh_)),
      position_(other.position_),
      scale_(other.scale_),
      rotation_(other.rotation_),
      modelMatrixDirty_(other.modelMatrixDirty_),
      modelMatrix_(other.modelMatrix_) {
    // 重置源对象
    other.type_ = UNKNOWN;
}

Geometry& Geometry::operator=(Geometry&& other) noexcept {
    if (this != &other) {
        // 清理当前资源
        clear();
        
        // 移动资源
        type_ = other.type_;
        mesh_ = std::move(other.mesh_);
        position_ = other.position_;
        scale_ = other.scale_;
        rotation_ = other.rotation_;
        modelMatrixDirty_ = other.modelMatrixDirty_;
        modelMatrix_ = other.modelMatrix_;
        
        // 重置源对象
        other.type_ = UNKNOWN;
    }
    return *this;
}

void Geometry::setPosition(const glm::vec3& position) {
    position_ = position;
    markModelMatrixDirty();
}

void Geometry::setScale(const glm::vec3& scale) {
    scale_ = scale;
    markModelMatrixDirty();
}

void Geometry::setRotation(const glm::vec3& rotation) {
    rotation_ = rotation;
    markModelMatrixDirty();
}

glm::mat4 Geometry::getModelMatrix() const {
    if (modelMatrixDirty_) {
        // 重新计算模型矩阵
        modelMatrix_ = glm::mat4(1.0f);
        modelMatrix_ = glm::translate(modelMatrix_, position_);
        
        // 旋转（绕X、Y、Z轴）
        modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
        
        modelMatrix_ = glm::scale(modelMatrix_, scale_);
        
        modelMatrixDirty_ = false;
    }
    return modelMatrix_;
}

#endif