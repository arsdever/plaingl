#version 460 core

uniform vec4 materialColor;

out vec4 fragmentColor;

void main()
{
    fragmentColor = materialColor;
}
