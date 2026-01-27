#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// 一个抽象的摄像机类
class Camera
{
public:
    // 相机属性
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp;

    // 欧拉角
    float yaw_;
    float pitch_;
    // 摄像机选项
    float movementSpeed;
    float mouseSensitivity;
    float zoom_;

    // 通过向量的构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom_(ZOOM)
    {
        position_ = position;
        worldUp = up;
        yaw_ = yaw;
        pitch_ = pitch;
        updateCameraVectors();
    }
    // 带有标量值的构造函数
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom_(ZOOM)
    {
        position_ = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw_ = yaw;
        pitch_ = pitch;
        updateCameraVectors();
    }

    // 返回使用欧拉角和视图矩阵计算得出的视图矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position_, position_ + front_, up_);
    }

    // 处理从任何类似键盘的输入系统接收的输入。接受以相机定义的枚举形式呈现的输入参数（以便将其与窗口系统相分离）
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)
            position_ += front_ * velocity;
        if (direction == BACKWARD)
            position_ -= front_ * velocity;
        if (direction == LEFT)
            position_ -= right_ * velocity;
        if (direction == RIGHT)
            position_ += right_ * velocity;
    }

    // 处理来自鼠标输入系统的输入信息。要求提供在 x 和 y 方向上的偏移值。
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainpitch_ = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw_   += xoffset;
        pitch_ += yoffset;

        // 限制俯仰角在 (-89, 89) 内，避免屏幕翻转
        if (constrainpitch_)
        {
            if (pitch_ > 89.0f)
                pitch_ = 89.0f;
            if (pitch_ < -89.0f)
                pitch_ = -89.0f;
        }

        // 使用更新后的欧拉角更新前向、右侧和向上向量
        updateCameraVectors();
    }

    // 处理来自鼠标滚轮事件所接收的输入。仅需要在垂直滚轮轴上进行输入即可。
    void ProcessMouseScroll(float yoffset)
    {
        zoom_ -= (float)yoffset;
        if (zoom_ < 1.0f)
            zoom_ = 1.0f;
        if (zoom_ > 45.0f)
            zoom_ = 45.0f;
    }

private:
    // 根据摄像机（更新后的）的欧拉角计算前方向量
    void updateCameraVectors()
    {
        // 计算新的 front 向量
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        // 重新计算 right 和 Up 向量
        right_ = glm::normalize(glm::cross(front_, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up_    = glm::normalize(glm::cross(right_, front_));
    }
};
#endif