#version 460 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 coord;
out vec2 texCoords;

uniform vec2 offset;
uniform vec2 scale;
uniform mat4 projection = mat4(0.0);

void main() {
    texCoords = vertex.xy;
    mat4 model = mat4(
        scale.x,  0.0,      0.0, 0.0,
        0.0,      scale.y,  0.0, 0.0,
        0.0,      0.0,      0.0, 1.0,
        offset.x, offset.y, 0.0, 1.0
    );
    gl_Position = projection * model * vertex;
}
