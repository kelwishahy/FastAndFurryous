#version 330 core
in vec3 position;
in vec2 texCoord;
uniform mat4 projection;
uniform mat4 transform;

out vec2 texCoordFS;

void main() {
    
    gl_Position = projection * transform * vec4(position, 1.0);
    texCoordFS = texCoord;
}