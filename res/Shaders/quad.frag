#version 460 core

in vec2 texCoords;
out vec4 final;

uniform sampler2D tex;

void main() {
    vec4 intex = texture(tex, texCoords);
    final = intex.aaaa;
}
