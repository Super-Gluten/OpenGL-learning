#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout (location = 6) in vec4 m_BoneIDs;
layout (location = 7) in vec4 m_Weights;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main() 
{
    gl_Position = view * model * vec4(aPos, 1.0);
    vs_out.normal = aNormal;
}