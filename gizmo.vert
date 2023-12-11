#version 460 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 projection_matrix;
uniform mat4 model_matrix;

void main()
{
    gl_Position = projection_matrix * model_matrix * vec4(vertexPosition, 1.0);
}
