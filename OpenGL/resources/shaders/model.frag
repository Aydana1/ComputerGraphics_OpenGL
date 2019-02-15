#version 330 core

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

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    float shininess;
};

struct Object {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Object object;
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;

in vec3 Normal;
in vec3 FragPos;
in vec3 fcolor;

in vec2 TexCoords;
uniform vec3 viewPos;

out vec4 Fcolor;

vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );

vec3 CelShader( DirLight dlight, SpotLight slight, vec3 normal, vec3 fragPos, vec3 viewDir );

uniform bool materialOn;

// Cel shading
uniform vec3 _Color;
uniform vec3 _UnlitColor;
uniform float _DiffuseThreshold;
uniform vec3 _OutlineColor;
uniform float _LitOutlineThickness;
uniform float _UnlitOutlineThickness;

// Toggle shaders
uniform bool flashLight;
uniform bool phongShader;
uniform bool celShader;

void main( )
{
    
    vec3 normal = normalize( Normal );
    vec3 viewDir = normalize( viewPos - FragPos );
    vec3 res = vec3( 0.0f, 0.0f, 0.0f );
    
    if( flashLight ) {
       res += CalcSpotLight( spotLight, normal, FragPos, viewDir );
    }
    
    if( celShader ) {
       res += CelShader( dirLight, spotLight, normal, FragPos, viewDir );
    }
    
    if( phongShader ) {
        res += CalcDirLight( dirLight, normal, viewDir );
    }
   
    Fcolor = vec4( res, 1.0f );
   
}

vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir ) {
    
    vec3 lightDir = normalize( -light.direction );
    float diffuseImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact

    vec3 reflecDir = reflect( -lightDir, normal );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );

    vec3 ambient = vec3( 0.0f );
    vec3 diffuse = vec3( 0.0f );
    vec3 specular = vec3( 0.0f );

    if( materialOn == true ) {
        ambient = light.ambient * vec3( texture( material.texture_diffuse, TexCoords ) );
        diffuse = light.diffuse * diffuseImpact * vec3( texture( material.texture_diffuse, TexCoords ) );
        specular = light.specular * specularImpact * vec3( texture( material.texture_specular, TexCoords ) );
    } else {
        ambient = light.ambient * object.ambient;
        diffuse = light.diffuse * diffuseImpact * object.diffuse;
        specular = light.specular * specularImpact * object.specular;
    }

    return ( ambient + diffuse + specular );
}

vec3 CelShader( DirLight dlight, SpotLight slight, vec3 normal, vec3 fragPos, vec3 viewDir ) {
    
    // Directional light impact
    vec3 lightDir = normalize( -dlight.direction );
    float diffuseImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
    
    // Cel Shading without outline
    vec3 color = vec3( 0.0 );
    
    if ( diffuseImpact > 0.95 )
        color = vec3( 1.0, 0.5, 0.5 );
    else if ( diffuseImpact > 0.5 )
        color = vec3( 0.6, 0.3, 0.3 );
    else if ( diffuseImpact > 0.25 )
        color = vec3( 0.4, 0.2, 0.2 );
    else
        color = vec3( 0.2, 0.1, 0.1 );
    
    // SpotLight impact
    lightDir = normalize( slight.position - fragPos );
    float distance = length( slight.position - fragPos );
    float attenuation = 1.0 / ( slight.constant  + distance * slight.linear + distance*distance * slight.quadratic );
    
    vec3 fragmentColor = vec3(_UnlitColor);
    
    // low priority: diffuse illumination
    if ( attenuation * max( 0.0, dot( normal, lightDir ) ) >= _DiffuseThreshold )
    {
        fragmentColor = vec3( slight.diffuse ) * vec3(_Color);
    }
    
    // higher priority: outline
    if ( dot( viewDir, normal) < mix( _UnlitOutlineThickness, _LitOutlineThickness,
              max(0.0, dot(normal, lightDir ) ) ) )
    {
        fragmentColor = vec3( slight.diffuse ) * vec3(_OutlineColor);
    }
    
    // highest priority: highlights
    if ( dot( normal, lightDir ) > 0.0 && attenuation *  pow( max( 0.0, dot( reflect( -lightDir, normal ), viewDir ) ),
                                                             object.shininess ) > 0.5 )
        // more than half highlight intensity?
    {
        fragmentColor = slight.specular
        * vec3( slight.diffuse ) * vec3( slight.specular )
        + (1.0 - slight.specular ) * fragmentColor;
    }
    
    return fragmentColor + color;
}

// flash light
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir ) {
    
    vec3 lightDir = normalize( light.position - fragPos );
    float diffuseImpact = max( dot( normal, lightDir ), 0.0 );  // larger angle --> less impact
    vec3 reflecDir = reflect( -lightDir, normal );
    float specularImpact = pow( max( dot( viewDir, reflecDir ), 0.0 ), material.shininess );
    
    vec3 ambient = vec3( 0.0f );
    vec3 diffuse = vec3( 0.0f );
    vec3 specular = vec3( 0.0f );
    
    if( materialOn == true ) {
        ambient = light.ambient * vec3( texture( material.texture_diffuse, TexCoords ) );
        diffuse = light.diffuse * vec3( texture( material.texture_diffuse, TexCoords ) );
        specular = light.specular * specularImpact * vec3( texture( material.texture_specular, TexCoords ) );
    } else {
        ambient = light.ambient * object.ambient;
        diffuse = light.diffuse * diffuseImpact * object.diffuse;
        specular = light.specular * specularImpact * object.specular;
    }

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


