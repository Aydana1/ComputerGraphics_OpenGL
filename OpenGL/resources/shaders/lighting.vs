#version 330

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 oColor;
layout ( location = 2 ) in vec2 texCord;

out vec3 fColor;
out vec3 fPosition;
out vec2 TexCord;

uniform mat4 matrix;

void main() {
    float horizOffset = 0.3;
    gl_Position = matrix * vec4( position.x + horizOffset, position.y, position.z, 1.0f );
    fColor = oColor;
    fPosition = position;
    TexCord = vec2(texCord.x, 1.0 - texCord.y);
}

