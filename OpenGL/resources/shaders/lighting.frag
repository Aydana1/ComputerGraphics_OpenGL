#version 330

out vec4 outColor;
in vec3 fColor;
in vec3 fPosition;
in vec2 TexCord;

uniform vec4 newColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixValue;
uniform float time;

void main() {
   // fColor = vec4( 0.74f, 0.85f, 0.34f, 1.0f );
   // outColor = vec4( fColor, 1.0f );
    //outColor = newColor;
    //outColor = texture(myTexture, TexCord) * vec4( fColor, 1.0f );
    
    if( TexCord.y < 0.5 ) {
        outColor = mix( texture(texture1, TexCord), texture(texture2, vec2(TexCord.x, TexCord.y)), mixValue );
    } else {
        outColor = mix( texture(texture1, TexCord),
                       texture(texture2, vec2(TexCord.x + sin( TexCord.y * 60.0 + time * 2.0 )/ 30.0 , 1.0-TexCord.y)) * newColor, mixValue );
    }
    
    
    
    //* newColor;
}
