#version 330 core
in vec3 position;
in vec2 texCoord;

out vec2 texCoordFS;

uniform mat4 projection;
uniform mat4 transform;

void main() {
    
    gl_Position = projection * transform * vec4(position, 1.0);
    texCoordFS = texCoord;
}