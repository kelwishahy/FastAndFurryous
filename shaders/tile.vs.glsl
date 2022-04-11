#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in mat4 transform;
uniform mat4 projection;

out vec2 texCoordFS;

void main() {
    gl_Position = projection * transform * vec4(position, 0.0, 1.0);
    texCoordFS = texCoord;
}