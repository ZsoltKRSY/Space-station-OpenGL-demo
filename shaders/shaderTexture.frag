#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D depthMap;

//fog
uniform bool isFog;
vec4 fogColor = vec4(0.035f, 0.081f, 0.048f, 1.0f);

//lighting
//directional - sun
uniform vec3 lightDir;
uniform vec3 lightColor;
float dirAmbientStrength = 0.05f;
float dirSpecularStrength = 0.2f;

//point - holograms
uniform vec3 hologram1LightLoc, hologram2LightLoc, hologram3LightLoc;
uniform vec3 hologram1_2_3LightColor;
float pointAmbientStrength = 0.1f;
float pointSpecularStrength = 0.4f;
float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

//point - vulcan
uniform vec3 vulcanLight1Loc;
uniform vec3 vulcanLight2Loc;
uniform vec3 vulcanLightColor;

//point - negh
uniform bool neghLightIsOn;
uniform vec3 neghLight1Loc;
uniform vec3 neghLight2Loc;
uniform vec3 neghLight3Loc;
uniform vec3 neghLight4Loc;
uniform vec3 neghLightColor;

//spotlight - corridor
uniform vec3 corridorLight1Loc;
uniform vec3 corridorLight2Loc;
uniform vec3 corridorLight3Loc;
uniform vec3 corridorLight4Loc;
uniform vec3 corridorLightColor;
float spotAmbientStrength = 0.2f;
float spotSpecularStrength = 0.4f;
vec3 spotlightDiffuse = vec3(0.0f), spotlightSpecular = vec3(0.0f);

//components
vec3 ambient = vec3(0.0f);
vec3 diffuse = vec3(0.0f);
vec3 specular = vec3(0.0f);

void computePointLight(vec3 lightPos, vec3 lightColor, float ambientStrength, float specularStrength)
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    vec4 lightPosEye = view * model * vec4(lightPos, 1.0f);

    //normalize light direction
    vec3 lightDirN = normalize(lightPosEye.xyz - fPosEye.xyz);

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDirN = normalize(- fPosEye.xyz);

    //compute distance to light
    float dist = length(lightPosEye.xyz - fPosEye.xyz);
    //compute attenuation
    float att = 1.0f / (constant + linear * dist + linear * (dist * dist));

    //ambient
    ambient += att * ambientStrength * lightColor;

    //diffuse
    diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //specular
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);
    specular += att * specularStrength * specCoeff * lightColor;
}

void computeSpotlight(vec3 lightPos, vec3 lightColor, float ambientStrength, float specularStrength){
    float outerCone = 0.9f;
    float innerCone = 0.95f;
    vec3 normalEye = normalize(normalMatrix * fNormal);

    vec3 lightDirection = normalize(lightPos - fPosition);

    ambient += ambientStrength * fColor.rgb;

    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 viewDirection = normalize(- fPosEye.xyz);
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

    spotlightDiffuse += inten * max(dot(normalEye, lightDirection), 0.0f) * lightColor;
    spotlightSpecular += inten * specularStrength * specCoeff * lightColor;
}

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDirN = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient += dirAmbientStrength * lightColor;

    //compute diffuse light
    diffuse += max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);
    specular += dirSpecularStrength * specCoeff * lightColor;
}

float computeFog()
{
    float fogDensity = 0.0025f;

    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    
    float fragDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow()
{
	vec3 normalizedCoords = fPosLightSpace.xyz / fPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5f + 0.5f;

    if(normalizedCoords.z > 1.0f)
        return 0.0f;

	float shadow = 0.0f;
	float currentDepth = normalizedCoords.z;
	float bias = max(0.025f * (1.0f - dot(normalize(fNormal), lightDir)), 0.0005f);

    int sampleRadius = 2;
    vec2 pixelSize = 1.0f / textureSize(depthMap, 0);
    for(int y = -sampleRadius; y <= sampleRadius; ++y){
        for(int x = -sampleRadius; x <= sampleRadius; ++x){
            float closestDepth = texture(depthMap, normalizedCoords.xy + vec2(x, y) * pixelSize).r;
            if(currentDepth > closestDepth + bias)
                shadow += 1.0f;
        }
    }

    shadow /= pow((sampleRadius * 2 + 1), 2);

	return shadow;
}

void main() 
{
    computeDirLight();
    computePointLight(hologram1LightLoc, hologram1_2_3LightColor, pointAmbientStrength, pointSpecularStrength);
    computePointLight(hologram2LightLoc, hologram1_2_3LightColor, pointAmbientStrength, pointSpecularStrength);
    computePointLight(hologram3LightLoc, hologram1_2_3LightColor, pointAmbientStrength, pointSpecularStrength);

    computePointLight(vulcanLight1Loc, vulcanLightColor, pointAmbientStrength, pointSpecularStrength);
    //computePointLight(vulcanLight2Loc, vulcanLightColor, pointAmbientStrength, pointSpecularStrength);

    computeSpotlight(corridorLight1Loc, corridorLightColor, spotAmbientStrength, spotSpecularStrength);
    computeSpotlight(corridorLight2Loc, corridorLightColor, spotAmbientStrength, spotSpecularStrength);
    computeSpotlight(corridorLight3Loc, corridorLightColor, spotAmbientStrength, spotSpecularStrength);
    computeSpotlight(corridorLight4Loc, corridorLightColor, spotAmbientStrength, spotSpecularStrength);

    if(neghLightIsOn){
        computePointLight(neghLight1Loc, neghLightColor, pointAmbientStrength, pointSpecularStrength);
        computePointLight(neghLight2Loc, neghLightColor, pointAmbientStrength, pointSpecularStrength);
        computePointLight(neghLight3Loc, neghLightColor, pointAmbientStrength, pointSpecularStrength);
        computePointLight(neghLight4Loc, neghLightColor, pointAmbientStrength, pointSpecularStrength);
    }

    float shadow = computeShadow();

    vec3 color = min((ambient + (1.1f - shadow)*diffuse + spotlightDiffuse) * texture(diffuseTexture, fTexCoords).rgb + ((1.0f - shadow)*specular + spotlightSpecular) * texture(specularTexture, fTexCoords).rgb, 1.0f);
    
    if(isFog)
    {
        float fogFactor = computeFog();

        fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
    }
    else
    {
       fColor = vec4(color, 1.0f);
    }
}
