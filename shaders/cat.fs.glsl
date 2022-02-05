#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    FragColor = texture(ourTexture, TexCoord);

    // discard any fragments with an alpha of 0
    if (FragColor.a == 0.0) {
        discard;
    }
}