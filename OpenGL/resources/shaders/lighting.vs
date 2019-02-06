#version 330

layout( location = 0 ) in vec3 position;
//layout( location = 1 ) in vec3 oColor;
//layout ( location = 2 ) in vec2 texCord;
//
//out vec3 fColor;
//out vec3 fPosition;
//out vec2 TexCord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4( position.x, position.y, position.z, 1.0f );
}

