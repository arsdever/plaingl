#version 460 core

layout(location = 0) in vec2 vertexPosition;

uniform mat4 camera_matrix;

out vec3 direction;

void main()
{
    gl_Position = vec4(vertexPosition, 0.9999, 1.0);
    direction =
        (camera_matrix * vec4(vertexPosition.x, -vertexPosition.y, -1.0, 1.0))
            .xyz;
}
