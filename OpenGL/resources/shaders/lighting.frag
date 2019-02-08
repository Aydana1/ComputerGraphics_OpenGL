#version 330

out vec4 Fcolor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform Light light;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    vec3 normal = normalize( Normal );
    vec3 lightDir = normalize( light.position - FragPos );
    // DOT PRODUCT
    float lightImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
    vec3 diffuse = light.diffuse * lightImpact * material.diffuse;
    
    // Specular
    float specularStrength = 0.2;
    vec3 reflecDir = reflect( -lightDir, normal );
    vec3 viewDir = normalize( viewPos - FragPos );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );
    vec3 specular = light.specular * specularStrength * specularImpact * material.specular;
    
    vec3 res = (ambient + diffuse + specular);
    
    Fcolor = vec4( res, 1.0 );
}
