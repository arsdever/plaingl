#version 460 core

uniform vec4 materialColor;
uniform int is_selected;

out vec4 fragmentColor;

void main() { fragmentColor = materialColor + vec4(is_selected); }
