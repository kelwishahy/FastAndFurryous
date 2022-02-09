#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
uniform mat4 projection;
uniform mat4 transform;

void main() {
    gl_Position = projection * transform * vec4(aPos, 1.0);
}