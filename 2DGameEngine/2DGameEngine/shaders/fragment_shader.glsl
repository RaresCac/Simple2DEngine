#version 330 core

in vec2 textureCoor;

out vec4 FragColor;

uniform vec3 colour;
uniform bool hasTexture;
uniform sampler2D objTexture;

void main() {
    vec4 frag;
    if (hasTexture){
        frag = texture(objTexture, textureCoor);
        if (frag.a == 0.0){
            discard;
        }
    } else {
        frag = vec4(colour, 1);
    }
    FragColor = frag;
} 