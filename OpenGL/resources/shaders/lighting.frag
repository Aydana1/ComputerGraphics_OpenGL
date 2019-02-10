#version 330

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 direction;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;   // cos(phi)  -  angle between spotLight and lightDirection
    float outerCutOff;   // cos(gamma)

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

#define NR_POINT_LIGHTS 8

uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[ NR_POINT_LIGHTS ];  // array of struct

in vec2 TexCord;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 viewPos;

out vec4 Fcolor;

vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

void main() {
    
    vec3 normal = normalize( Normal );
    vec3 viewDir = normalize( viewPos - FragPos );
    vec3 res = vec3( 0.0f, 0.0f, 0.0f );
    // phase 1: Directional lighting
    res += CalcDirLight( dirLight, normal, viewDir );
    // phase 2: Several point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        res += CalcPointLight( pointLights[i], normal, FragPos, viewDir );
    };
    
    res += CalcSpotLight( spotLight, normal, FragPos, viewDir );

    Fcolor = vec4( res, 1.0 );
}

vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir ) {
    
    vec3 lightDir = normalize( -light.direction );
    float diffuseImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
    
    vec3 reflecDir = reflect( -lightDir, normal );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );
    
    // Ambient
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCord ) );
    // Diffuse
    vec3 diffuse = light.diffuse * diffuseImpact * vec3( texture( material.diffuse, TexCord ) );
    // Specular
    vec3 specular = light.specular * specularImpact * vec3( texture( material.specular, TexCord ) );
    
    return ( ambient + diffuse + specular );
}

vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir ) {
    
    vec3 lightDir = normalize( light.position - fragPos );
    float diffuseImpact = max( dot( normal, lightDir ), 0.0 );
    
    vec3 reflecDir = reflect( -lightDir, normal );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );

    // Ambient
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCord ) );
    // Diffuse
    vec3 diffuse = light.diffuse * diffuseImpact *  vec3( texture( material.diffuse, TexCord ) );
    // Specular
    vec3 specular = light.specular * specularImpact * vec3( texture( material.specular, TexCord ) );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0 / ( light.constant  + distance * light.linear + distance*distance * light.quadratic );
    specular *= attenuation;
    diffuse *= attenuation;
    ambient *= attenuation;
    
    return ( ambient + diffuse + specular );
}

// flash light
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir ) {
    vec3 lightDir = normalize( light.position - fragPos );
    // Ambient
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCord ) );
    // Diffuse
    vec3 diffuse = light.diffuse * vec3( texture( material.diffuse, TexCord ) );
    // Specular
    vec3 reflecDir = reflect( -lightDir, normal );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );
    vec3 specular = light.specular * specularImpact * vec3( texture( material.specular, TexCord ) );

    // INTENSITY (radius) OF FLASHLIGHT
    // Theta - angle between lightDir and spotlight
    float cosTheta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( cosTheta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    // when spolight area is big -> more ambient color -> when cos(theta) is small i.e. close to 0.0
    diffuse *= intensity;  // between 0.0 and 1.0 intensity
    specular *= intensity;

    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0 / ( light.constant  + distance * light.linear + distance*distance * light.quadratic );
    specular *= attenuation;
    diffuse *= attenuation;
    ambient *= attenuation;

    return ( ambient + diffuse + specular );
}


