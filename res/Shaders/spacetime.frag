#version 330 core

/*
Translated from "LiveCoding - The Universe Within" Series.
Part 1 - https://www.youtube.com/watch?v=3CycKKJiwis
Part 2 - https://www.youtube.com/watch?v=KGJUl8Teipk
*/

in vec2 texCoords;
out vec4 color;
uniform vec2 resolution;
uniform float time;
uniform sampler2D tex;

float distLine(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float t = clamp(dot(pa, ba) / dot(ba, ba), 0., 1.);
    return length(pa - ba * t);
}

float N21(vec2 p) {
    p = fract(p * vec2(233.34, 851.73));
    p += dot(p, p+23.45);
    return fract(p.x * p.y);
}

vec2 N22(vec2 p) {
    float n = N21(p);
    return vec2(n, N21(p + n));
}

vec2 getPos(vec2 id, vec2 offset) {
    vec2 n = N22(id + offset) * time;

    return offset + sin(n) * 0.4;
}

float line(vec2 p, vec2 a, vec2 b) {
    float d = distLine(p, a, b);
    float m = smoothstep(.03, .01, d);
    float dab = length(a - b);

    m *= smoothstep(1.2, 0.8, dab) * 0.5 + smoothstep(0.05, 0.03, abs(dab - 0.75));

    return m;
}

vec3 layer(vec2 uv) {
    vec2 gv = fract(uv) - 0.5;

    //if (gv.x > .48 || gv.y > .48) { 
    //    return vec3(1, 0, 0); 
    //}

    vec2 id = floor(uv);

    vec2 p[9];

    int i = 0;
    for (float y=-1.0; y <= 1.0; y++) {
        for (float x = -1.0; x <= 1.0; x++) {
            p[i++] = getPos(id, vec2(x, y));
        }
    }

    float m = 0.0;
    float t = time * 5.0;
    for (int i = 0; i < 9; i++) {
        m += line(gv, p[4], p[i]);

        vec2 j = (p[i] - gv) * 20.0;
        float sparkle = 1.0 / dot(j, j);

        m += sparkle * (sin(t + fract(p[i].x) * 10.0) * 0.5 + 0.5);
    }

    m += line(gv, p[1], p[3]);
    m += line(gv, p[1], p[5]);
    m += line(gv, p[7], p[3]);
    m += line(gv, p[7], p[5]);

    return vec3(m);
}

void main() {
    vec2 uv = (gl_FragCoord.xy - .5 * resolution.xy) / resolution.y;
    //vec2 mouse = (u_Cursor.xy / u_Resolution.xy) - 0.5;

    float gradient = uv.y;

    vec3 m = vec3(0);
    float t = time * 0.1;

    float s = sin(t);
    float c = cos(t);
    mat2 rot = mat2(c, -s, s, c);

    uv *= rot;
    //mouse *= rot;

    for (float i = 0.0; i < 1.0; i += 1.0 / 4.0) {
        float z = fract(i + t);
        float size = mix(30.0, 0.5, z);
        float fade = smoothstep(0.0, 0.5, z) * smoothstep(1.0, 0.8, z);
        //m += layer(uv * size + i * 20.0 - mouse) * fade;  // with mouse movement
        m += layer(uv * size + i * 20.0) * fade;
    }

    //vec3 base = sin(t * vec3(0.8, 0.4, 0.2)) * 0.4 + 0.6;
    float textureAlpha = texture(tex, texCoords).a;
    vec3 base = vec3(textureAlpha, 0.0, 0.0);
    vec3 col = m * base;
    //col += 0.1 * base;

    color = vec4(col, textureAlpha);
}
