#version 330 core

uniform vec3 colour;
layout(location = 0) out vec4 out_color;

void main() {
	out_color = vec4(colour, 1.0);

}