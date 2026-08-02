#define MAX_DIR_LIGHTS   4
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS  4

struct DirectionalLight_t
{
    float3 vAmbient;
    float _flPad0;
    float3 vDiffuse;
    float _flPad1;
    float3 vSpecular;
    float _flPad2;
    
    float3 vDirection;
    float _flPad3;
};

struct PointLight_t
{
    float3 vAmbient;
    float _flPad0;
    float3 vDiffuse;
    float _flPad1;
    float3 vSpecular;
    float _flPad2;

    float3 vPosition;
    float flRange;

    float3 vAttenuation;
    float _flPad3;
};

struct SpotLight_t
{
    float3 vAmbient;
    float _flPad0;
    float3 vDiffuse;
    float _flPad1;
    float3 vSpecular;
    float _flPad2;

    float3 vPosition;
    float flRange;

    float3 vDirection;
    float flSpot;

    float3 vAttenuation;
    float _flPad3;
};

struct Material_t
{
    float3 vAmbient;
    float flShininess;
    
    float3 vDiffuse;
    float flOpacity;
    
    float3 vSpecular;
    float _flPad0;
};

// Functions
void ComputeDirectionalLight(Material_t Material, DirectionalLight_t Light, float3 vNormal, float3 vToEye, 
    out float3 vAmbient, out float3 vDiffuse, out float3 vSpecular)
{
    vAmbient = float4(0, 0, 0, 0);
    vDiffuse = float4(0, 0, 0, 0);
    vSpecular = float4(0, 0, 0, 0);

    float3 vLightVector = -Light.vDirection;
    vAmbient = Material.vAmbient;
    
    float flDiffuseFactor = dot(vLightVector, vNormal);

    [flatten]
    if (flDiffuseFactor > 0.0f)
    {
        float3 vReflectDirection = reflect(-vLightVector, vNormal);
        float flSpecularFactor = pow(max(dot(vReflectDirection, vToEye), 0.0f), Material.flShininess);

        vDiffuse = flDiffuseFactor * Material.vDiffuse * Light.vDiffuse;
        vSpecular = flSpecularFactor * Material.vSpecular * Light.vSpecular;
    }
}

void ComputePointLight(Material_t Material, PointLight_t Light, float3 vPosition, float3 vNormal, float3 vToEye,
    out float3 vAmbient, out float3 vDiffuse, out float3 vSpecular)
{
    vAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    vDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    vSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 vLightVector = Light.vPosition - vPosition;
    float flDistance = length(vLightVector);

    if (flDistance > Light.flRange)
        return;
    
    vLightVector /= flDistance;
    vAmbient = Material.vAmbient * Light.vAmbient;
    
    float flDiffuseFactor = dot(vLightVector, vNormal);
    
    [flatten]
    if (flDiffuseFactor > 0.0f)
    {
        float3 vReflectDirection = reflect(-vLightVector, vNormal);
        float flSpecularFactor = pow(max(dot(vReflectDirection, vToEye), 0.0f), Material.flShininess);

        vDiffuse = flDiffuseFactor * Material.vDiffuse * Light.vDiffuse;
        vSpecular = flSpecularFactor * Material.vSpecular * Light.vSpecular;
    }

    float flAttenuation = 1.f / dot(Light.vAttenuation, float3(1.0f, flDistance, flDistance * flDistance));

    vDiffuse *= flAttenuation;
    vSpecular *= flAttenuation;
}

void ComputeSpotLight(Material_t Material, SpotLight_t Light, float3 vPosition, float3 vNormal, float3 vToEye,
    out float3 vAmbient, out float3 vDiffuse, out float3 vSpecular)
{
    vAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    vDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    vSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 vLightVector = Light.vPosition - vPosition;
    float flDistance = length(vLightVector);
    
    if (flDistance > Light.flRange)
        return;
    
    vLightVector /= flDistance;
    vAmbient = Material.vAmbient * Light.vAmbient;
    
    float flDiffuseFactor = dot(vLightVector, vNormal);
    
    [flatten]
    if (flDiffuseFactor > 0.0f)
    {
        float3 vReflectDirection = reflect(-vLightVector, vNormal);
        float flSpecularFactor = pow(max(dot(vReflectDirection, vToEye), 0.0f), Material.flShininess);

        vDiffuse = flDiffuseFactor * Material.vDiffuse * Light.vDiffuse;
        vSpecular = flSpecularFactor * Material.vSpecular * Light.vSpecular;
    }
    
    float flSpot = pow(max(dot(-vLightVector, Light.vDirection), 0.0f), Light.flSpot);
    float flAttenuation = 1.f / dot(Light.vAttenuation, float3(1.0f, flDistance, flDistance * flDistance));

    vAmbient *= flSpot;
    vDiffuse *= flAttenuation;
    vSpecular *= flAttenuation;
}