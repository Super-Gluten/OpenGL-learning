#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Position;
in vec3 Normal;


uniform vec3 cameraPos;
uniform sampler2D texture_diffuse1;    // 单元0
uniform sampler2D texture_specular1;   // 单元1  
uniform sampler2D texture_normal1;     // 单元2
uniform sampler2D texture_height1;     // 单元3（如果有）
uniform samplerCube skybox;            // 单元4（天空盒）
uniform sampler2D texture_reflection1; // 单元5（反射贴图）

void main()
{             
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    R = vec3(R.y, R.x, R.z); 
    FragColor = vec4(texture(skybox, -R).rgb, 1.0);
}