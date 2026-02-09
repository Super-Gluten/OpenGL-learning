#ifndef MESH_H
#define MESH_H

#include "Shader/Shader.h"
#include "Vertex.h"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

// 立体图形网格结构体：统一封装顶点数据和三角形索引
// 一个Mesh对应一个完整3D图形，并支持VAO/VBO/EBO
struct Mesh {
    std::vector<Vertex> vertices;      // 顶点列表（存储所有唯一顶点）
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    // OpenGL对象ID
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    
    Mesh() = default;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices  = vertices;
        this->indices   = indices;
        this->textures  = textures;

        setupBuffers();
    }
    
    // 拷贝构造函数（禁用）
    Mesh(const Mesh&) = delete;
    // 拷贝赋值运算符（禁用）
    Mesh& operator=(const Mesh&) = delete;
    
    // 移动构造函数
    Mesh(Mesh&& other) noexcept 
        : vertices(std::move(other.vertices))
        , indices(std::move(other.indices))
        , textures(std::move(other.textures))
        , VAO(other.VAO)
        , VBO(other.VBO)
        , EBO(other.EBO) 
    {
        // 将原对象中的OpenGL对象ID置零，这样原对象析构时就不会删除这些资源
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    
    // 移动赋值运算符
    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            cleanup(); // 清理当前对象持有的资源
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            textures = std::move(other.textures);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }
    
    ~Mesh() {
        cleanup();
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
    
    // 初始化 OpenGL 缓冲区
    bool setupBuffers() {
        // 清理旧的 OpenGL 对象
        cleanup();
        
        if (vertices.empty() || indices.empty()) {
            return false;
        }
        
        // 1. 创建并绑定 VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        // 2. 创建并绑定 VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     vertices.size() * sizeof(Vertex),
                     vertices.data(),
                     GL_STATIC_DRAW);
        
        // 3. 创建并绑定 EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
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

        // 切线属性 (location = 4)
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::tangentOffset());
        glEnableVertexAttribArray(4);

        // 副切线属性 (location = 5)
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             (void*)Vertex::bitangentOffset());
        glEnableVertexAttribArray(5);

        // ids
		glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), 
                             (void*)Vertex::m_BoneIDsOffset());
        glEnableVertexAttribArray(6);

		// weights
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                             (void*)Vertex::m_WeightsOffset());
        glEnableVertexAttribArray(7);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error binding VAO: " << err << std::endl;
            return false;
        } 

        // 5. 解绑 VAO
        glBindVertexArray(0);

        return true;
    }
    
    // 渲染网格
    bool render(Shader shader) const {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!" << std::endl;
            return false;
        }
        
        // 检查是否有顶点数据
        if (vertices.empty() || indices.empty()) {
            std::cerr << "No data to render!" << std::endl;
            return false;
        }

        
        shader.use();

        // 检查当前OpenGL上下文
        if (glGetString(GL_VERSION) == nullptr) {
            std::cerr << "No valid OpenGL context!" << std::endl;
            return false;
        }

        // 四个独立的纹理计数器
        unsigned int cnt_diffuse    = 1;
        unsigned int cnt_specular   = 1;
        unsigned int cnt_normal     = 1;
        unsigned int cnt_height     = 1;
        unsigned int cnt_reflection = 1;
        
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse") 
                number = std::to_string(cnt_diffuse++);
            else if(name == "texture_specular")
                number = std::to_string(cnt_specular++); 
            else if(name == "texture_normal")
                number = std::to_string(cnt_normal++); 
            else if(name == "texture_height")
                number = std::to_string(cnt_height++); 
            else if(name == "texture_reflection");
                number = std::to_string(cnt_reflection++);

            std::string uniformName = textures[i].type + number;

            shader.setInt(uniformName.c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(VAO);
        // GLenum err = glGetError();
        // if (err != GL_NO_ERROR) {
        //     std::cerr << "OpenGL error binding VAO: " << err << std::endl;
        //     return false;
        // } 
        
        glDrawElements(GL_TRIANGLES, 
                    indices.size(),
                    GL_UNSIGNED_INT, 
                    0);
        
        // err = glGetError();
        // if (err != GL_NO_ERROR) {
        //     std::cerr << "OpenGL error in glDrawElements: " << err << std::endl;
        //     return false;
        // }

        glBindVertexArray(0);

        // 恢复默认材质
        glActiveTexture(GL_TEXTURE0);
        
        return true;
    }
    
    // 清空网格数据
    void clear() {
        vertices.clear();
        indices.clear();
        cleanup();
    }
};

#endif