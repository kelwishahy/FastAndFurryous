#version 330 core
out vec4 FragColor;
  
in vec2 texCoordFS;

uniform sampler2D ourTexture;
uniform bool selected;

void main() {
    FragColor = texture(ourTexture, texCoordFS);

    if (!selected) {
        FragColor *= vec4(1.0, 1.0, 1.0, 0.4);
    }
    
    // discard any fragments with an alpha of 0
    if (FragColor.a == 0.0) {
       discard;
    }
}