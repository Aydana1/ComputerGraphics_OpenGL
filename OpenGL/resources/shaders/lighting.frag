#version 330

out vec4 Fcolor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 direction;
    vec3 position;
    float cutOff;   // cos(theta)  -  angle between spotLight and lightDirection
    
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;
uniform Light light;

in vec2 TexCord;

void main() {
    
    float distance = length( light.position - FragPos );
    float attenuation = 1.0 / ( light.constant  + distance * light.linear + distance*distance * light.quadratic );

    vec3 normal = normalize( Normal );
    vec3 lightDir = normalize( light.position - FragPos );
    //float lightImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
  
    
    // Theta - angle between lightDir and spotlight
    float cosTheta = dot( lightDir, normalize( -light.direction ) );
    vec3 res;
    if( cosTheta > light.cutOff ) {
        
        // Ambient
         vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCord ) );
         ambient *= attenuation;
        
        // Diffuse
        vec3 diffuse = light.diffuse * vec3( texture( material.diffuse, TexCord ) );
        diffuse *= attenuation;
        diffuse *= cosTheta;
        
        // Specular
        vec3 reflecDir = reflect( -lightDir, normal );
        vec3 viewDir = normalize( viewPos - FragPos );
        float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );
        vec3 specular = light.specular * specularImpact * vec3( texture( material.specular, TexCord ) );
        specular *= attenuation;
        specular *= cosTheta;
        
        res = ( ambient + diffuse + specular );
        
    } else {
        // Ambient lighting
        vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCord ) );
        res = ambient;
    }
  
    Fcolor = vec4( res, 1.0 );
}
