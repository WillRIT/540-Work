#ifndef __LIGHTING__
#define __LIGHTING__

//File contains all functions necessary to light a scene

//Types of Lights
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT		   2

//Max number of lights in a scene (power of 2)
#define MAX_LIGHTS 64

#define MAX_SPECULAR_EXPONENT 256.0f

static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal
static const float PI = 3.14159265359f;
static const float TWO_PI = PI * 2.0f;
static const float HALF_PI = PI / 2.0f;
static const float QUARTER_PI = PI / 4.0f;

//see Lights.h for explanation of fields
struct Lights
{
    //64 bytes 
    int type;
    float3 direction;
    float range;
    float3 position;
    float intensity;
    float3 color;
    float spotInnerAngle;
    float spotOuterAngle;
    float2 padding;
};

 //calculate diffuse lighting
float DiffuseLight(float3 normal, float3 directionToLight)
{
    //automatically normalizes values
    return saturate(dot(normal, directionToLight)); //return direction light "bounces" off object
}

float PhongSpecularLight(float3 normal, float3 directionToLight, float3 surfaceToCamera, float roughness)
{
    //Calculate necessary vectors
    //reflect the light across the normal (direction light leaves surface)
    float3 r = reflect(-directionToLight, normal);
    
    //Create an exponent value to represent the roughness of the surface
    //high value = shiny, low = dull
    float specularExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    //Perform specular calculation
    return pow(max(dot(r, surfaceToCamera), 0.0f), specularExponent);
}

// Lambert diffuse BRDF - Same as the basic lighting!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

//softens the light
float Attenuate(Lights currentLight, float3 worldPos)
{
    float dist = distance(currentLight.position, worldPos);
    float attenuation = saturate(1.0f - (dist * dist / (currentLight.range * currentLight.range)));
    return attenuation * attenuation;
}

float D_GGX(float3 n, float3 h, float roughness)
{
// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness; // Remapping roughness
    float a2 = max(a * a, MIN_ROUGHNESS);
// Denominator to be squared is ((n dot h)^2 * (a^2 - 1) + 1)
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    return a2 / (PI * denomToSquare * denomToSquare);
}

float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    return 1 / (NdotV * (1 - k) + k); // The numerator is replaced with 1 because of the dividing by zero
}


float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
    float3 h = normalize(v + l);
    // Run each function: D and G are scalars, F is a vector
    
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
    F_out = F;
    // Final formula
    float3 specResult = (D * F * G) / 4;
    
    return specResult * max(dot(n, 1), 0);
}

float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

//performs all necessary calculations for a directional light
float3 DirectionLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float roughness, float3 color) //can pass in a scalar to effect specular light on a per-pixel basis
{
    //get direction from surface to light
    float3 surfaceToLight = -currentLight.direction;
    
    //Perform all lighting calculations (ambient is in main)
    float diffuse = DiffuseLight(normal, surfaceToLight);
    float3 specular = PhongSpecularLight(normal, surfaceToLight, surfaceToCamera, roughness);
    
    //return calculated light
    return (diffuse * color + specular) * currentLight.intensity * currentLight.color;
}
//performs all necessary calculations for a point light
float3 PointLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float3 color)
{
    //get distance from light
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    
    //Preform all lighting calculations
    float diffuse = DiffuseLight(normal, surfaceToLight);
    float specular = PhongSpecularLight(normal, surfaceToLight, surfaceToCamera, roughness);
    float attenuation = Attenuate(currentLight, worldPosition);
    
    //return lighting results
    return (diffuse * color + specular) * attenuation * currentLight.intensity;

}
//performs all necessary calculations for a spotlight
float3 SpotLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float3 color)
{
    //get necessary components
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    float3 angle = saturate(dot(-surfaceToLight, currentLight.direction));
    
    //get two "cones"
    float cosInner = cos(currentLight.spotInnerAngle);
    float cosOuter = cos(currentLight.spotOuterAngle);
    float falloffRange = cosOuter - cosInner;
    
    //simulate shrinking of the lights to form cone
    float spotTerm = saturate((cosOuter - angle) / falloffRange).x;
    
    //use terms with point light calculation to create the cone
    return PointLight(currentLight, normal, surfaceToCamera, worldPosition, roughness, color) * spotTerm;
}

// Basic sample and unpack
float3 SampleAndUnpackNormalMap(Texture2D map, SamplerState samp, float2 uv)
{
    return map.Sample(samp, uv).rgb * 2.0f - 1.0f;
}
float3 NormalMapping(Texture2D map, SamplerState samp, float2 uv, float3 normal, float3 tangent)
{
	// Grab the normal from the map
    float3 normalFromMap = SampleAndUnpackNormalMap(map, samp, uv);

	// Gather the required vectors for converting the normal
    float3 N = normal;
    float3 T = normalize(tangent - N * dot(tangent, N));
    float3 B = cross(T, N);

	// Create the 3x3 matrix to convert from TANGENT-SPACE normals to WORLD-SPACE normals
    float3x3 TBN = float3x3(T, B, N);

	// Adjust the normal from the map and simply use the results
    return normalize(mul(normalFromMap, TBN));
}

//performs all necessary calculations for a direction light (PBR variant)
float3 DirectionLightPBR(Lights currentLight, float3 normal, float3 surfaceToCamera, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
    //get direction from surface to light
    float3 surfaceToLight = -currentLight.direction;
    
    //perform all lighting calculations
    float diffuse = DiffusePBR(normal, surfaceToLight);
    float3 F;
    //this needs specular color (metal or not)
    float3 specular = MicrofacetBRDF(normal, surfaceToLight, surfaceToCamera, roughness, specularColor, F);
    
    //calculate diffused light with energy conservation
    float3 balancedDiffuse = DiffuseEnergyConserve(diffuse, specular, metalness);
    //return calculated light
    //this needs surface color
    return (balancedDiffuse * surfaceColor + specular) * currentLight.intensity * currentLight.color;
}

//performs all necessary calculations for a point light (PBR variant)
float3 PointLightPBR(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
    //get distance from light
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    
    //Preform all lighting calculations
    float diffuse = DiffusePBR(normal, surfaceToLight);
    float3 F;
    float3 specular = MicrofacetBRDF(normal, surfaceToLight, surfaceToCamera, roughness, specularColor, F);
    float attenuation = Attenuate(currentLight, worldPosition);
    
    //energy conservation
    float3 balancedDiffuse = DiffuseEnergyConserve(diffuse, specular, metalness);
    
    //return lighting results
    return (balancedDiffuse * surfaceColor + specular) * attenuation * currentLight.intensity * currentLight.color;
}

//performs all necessary calculations for a spotlight (PBR variant)
float3 SpotLightPBR(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
    //get necessary components
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    float3 angle = saturate(dot(-surfaceToLight, currentLight.direction));
    
    //get two "cones"
    float cosInner = cos(currentLight.spotInnerAngle);
    float cosOuter = cos(currentLight.spotOuterAngle);
    float falloffRange = cosOuter - cosInner;
    
    //simulate shrinking of the lights to form cone
    float spotTerm = saturate((cosOuter - angle) / falloffRange).x;
    
    //use terms with point light calculation to create the cone
    return PointLightPBR(currentLight, normal, surfaceToCamera, worldPosition, roughness, metalness, surfaceColor, specularColor) * spotTerm;
}

float3 CalculateTotalLightPBR(int numLights, Lights lights[MAX_LIGHTS], float3 normal, float3 surfaceToCamera, float3 worldPos, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
    
    float3 totalLight; //store total lighting
    //loop through lights
    for (int i = 0; i < numLights; i++)
    {
        //grab a copy of the current index
        Lights currentLight = lights[i];
        //normalize the direction to ensure consistent results
        currentLight.direction = normalize(currentLight.direction);
        
        switch (currentLight.type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionLightPBR(currentLight, normal, surfaceToCamera, roughness, metalness, surfaceColor, specularColor);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLightPBR(currentLight, normal, surfaceToCamera, worldPos, roughness, metalness, surfaceColor, specularColor);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLightPBR(currentLight, normal, surfaceToCamera, worldPos, roughness, metalness, surfaceColor, specularColor);
                break;
        }
    }
    
    return totalLight;
}

#endif