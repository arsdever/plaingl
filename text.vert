#version 460 core

layout (location = 0) in vec4 vertexPosition; // <vec2 pos, vec2 tex>
out vec2 textureCoordinates;

uniform mat4 model_matrix;
uniform mat4 vp_matrix;

void main()
{
    gl_Position = vp_matrix * model_matrix * vec4(vertexPosition.xy, 0.0, 1.0);
    textureCoordinates = vertexPosition.zw;
}
