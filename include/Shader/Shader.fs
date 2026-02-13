// #version 330 core
// out vec4 FragColor;

// struct Material {
//     sampler2D diffuse;
//     sampler2D specular;
//     float shininess;
// }; 

// struct DirLight {
//     vec3 direction;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// struct PointLight {
//     vec3 position;
    
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;

//     float constant;     // 常数项
//     float linear;       // 一次项
//     float quadratic;    // 二次项
// };

// struct SpotLight {
//     vec3 position; 
//     vec3 direction;
//     float cutOff;
//     float outerCutOff;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;

//     float constant;     // 常数项
//     float linear;       // 一次项
//     float quadratic;    // 二次项
// };

// #define NR_POINT_LIGHTS 4

// in vec3 FragPos;
// in vec3 Normal;
// in vec2 TexCoords;

// uniform vec3 viewPos;
// uniform SpotLight spotLight;
// uniform PointLight pointLights[NR_POINT_LIGHTS];
// uniform DirLight dirLight;
// uniform Material material;

// // 定义函数
// vec3 CalDirLight(DirLight light, vec3 norm, vec3 viewDir);
// vec3 CalPointLight(PointLight light, vec3 norm, vec3 FragPos, vec3 viewDir);
// vec3 CalSpotLight(SpotLight light, vec3 norm, vec3 FragPos,vec3 viewDir);

// void main()
// {
//     vec3 norm = normalize(Normal);
//     vec3 viewDir = normalize(viewPos - FragPos);

//     // DirLight
//     vec3 result = CalDirLight(dirLight, norm, viewDir);

//     // PointLight
//     for (int i = 0; i < NR_POINT_LIGHTS; i++) {
//         result += CalPointLight(pointLights[i], norm, FragPos,viewDir);
//     }

//     // SpotLight
//     result += CalSpotLight(spotLight, norm, FragPos, viewDir);

//     FragColor = vec4(result, 1.0);
// } 

// vec3 CalDirLight(DirLight light, vec3 norm, vec3 viewDir) 
// {
//     // ambient
//     vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
//     // diffuse
//     vec3 lightDir = normalize(-light.direction);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

//     // specular
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    
//     return (ambient + diffuse + specular);
// }
// vec3 CalPointLight(PointLight light, vec3 norm, vec3 FragPos, vec3 viewDir)
// {
//     // ambient
//     vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

//     // diffuse
//     vec3 lightDir = normalize(light.position - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

//     // specular
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

//     // attenuation
//     float distance = length(light.position - FragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

//     return (ambient + diffuse + specular) * attenuation;
// }

// vec3 CalSpotLight(SpotLight light, vec3 norm, vec3 FragPos,vec3 viewDir)
// {
//     // ambient
//     vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

//     // diffuse 
//     vec3 lightDir = normalize(light.position - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
//     // specular
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

//     // intensity
//     float theta     = dot(lightDir, normalize(-light.direction));
// 	float epsilon   = light.cutOff - light.outerCutOff;
// 	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

//     diffuse  *= intensity;
//     specular *= intensity;

//     // attenuation
//     float distance    = length(light.position - FragPos);
// 	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
//     return (ambient + diffuse + specular) * attenuation; 
// }

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{             
    FragColor = texture(texture_diffuse1, TexCoords);
}