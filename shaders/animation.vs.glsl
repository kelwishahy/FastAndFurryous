#version 330 core

in vec3 position;
in vec2 texCoord;

uniform mat4 projection;
uniform mat4 transform;
uniform int currentFrame;
uniform float frameWidth;
uniform bool facingLeft;

out vec2 texCoordFS;

void main() {
	float newTexCoord = frameWidth * currentFrame;
	texCoordFS = texCoord;
	texCoordFS.x += newTexCoord;

	if(facingLeft == true) {
		texCoordFS = texCoordFS.xy * vec2(-1.0, 1.0);
	}

    gl_Position = projection * transform * vec4(position.xyz, 1.0);
}