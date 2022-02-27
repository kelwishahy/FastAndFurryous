#version 330 core
out vec4 FragColor;
  
in vec2 texCoordFS;

uniform sampler2D ourTexture;

void main() {
    FragColor = texture(ourTexture, texCoordFS);

    // discard any fragments with an alpha of 0
    if (FragColor.a == 0.0) {
        discard;
    }
}