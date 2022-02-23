#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 projection;
uniform mat4 transform;
uniform int curr_frame;
uniform float frame_width;

void main() {
	float newTexCoord = frame_width * curr_frame;
	TexCoord = aTexCoord;
	TexCoord.x += newTexCoord; 
    gl_Position = projection * transform * vec4(aPos.xyz, 1.0);
}