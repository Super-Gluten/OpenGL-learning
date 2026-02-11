#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 gsColor;

void buildHouse(vec4 position)
{
    // 左下
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    // 右下
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    // 左上
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);
    EmitVertex();

    // 右上
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);
    EmitVertex();

    // 顶部 且颜色定义为白
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);
    gsColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    gsColor = gs_in[0].color;
    buildHouse(gl_in[0].gl_Position);
}