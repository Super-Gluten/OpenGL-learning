#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in vec4 m_BoneIDs;
layout (location = 7) in vec4 m_Weights;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    Position = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
}