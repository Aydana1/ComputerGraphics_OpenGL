#version 330

out vec4 Fcolor;

uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = lightColor * ambientStrength;
    
    // Diffuse
    vec3 normal = normalize( Normal );
    vec3 lightDir = normalize( lightPos - FragPos );
    // DOT PRODUCT
    float lightImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
    vec3 diffuse = lightColor * lightImpact;
    
    // Specular
    float specularStrength = 0.5;
    vec3 reflecDir = reflect( -lightDir, normal );
    vec3 viewDir = normalize( viewPos - FragPos );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), 32.0 );
    vec3 specular = specularStrength * specularImpact * lightColor;
    
    vec3 res = (ambient + diffuse + specular) * objectColor;
    Fcolor = vec4( res, 1.0 );
}
