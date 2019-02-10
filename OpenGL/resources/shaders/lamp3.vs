#version 330 core
layout( location = 0 ) in vec3 position;   // declared in vertices array

uniform mat4 model;   // local coordinates --> camera
uniform mat4 view;     // normalized --> window coordinates
uniform mat4 projection;  // camera --> normalized coordinates (-1, 1)

void main() {
    gl_Position = projection * view * model *vec4( position, 1.0f );  // w = 1.0f
}
