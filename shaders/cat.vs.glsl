#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 projection;
//uniform mat4 transform;

void main() {
    
    //vec3 pos = projection * vec3(aPos.xy, 1.0);
	//gl_Position = vec4(pos.xy, aPos.z, 1.0);
    gl_Position = projection * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}