#shader vertex
#version 330 core

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec4 a_color;
layout(location = 2) in vec2 a_texCord;
layout(location = 3) in float a_texIndex;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

uniform mat4 lightSpaceMatrix;


out vec4 v_color;
out vec2 v_texCord;
out float v_texIndex;
out vec4 v_position;
out vec4 v_fragPosLightSpace;
out vec3 normal;

void main() {

    v_color = a_color;
    v_texCord = a_texCord;
    v_texIndex = a_texIndex;
    v_position = u_model * position;

    vec3 position3 = vec3(position);

    normal = mat3(u_model) * position3;

    v_fragPosLightSpace = lightSpaceMatrix * v_position;


    gl_Position = u_proj * u_view *u_model * v_position;
}













#shader fragment
#version 330 core

layout(location = 0)  out vec4 color;

const float PI = 3.14159265359;

struct Light {

    vec4 position;
    vec4 direction;
    vec3 color;

};

in vec4 v_color;
in vec2 v_texCord;
in float v_texIndex;


in vec3 v_viewPos;
in vec4 v_position;
in vec3 normal;

in vec4 v_fragPosLightSpace;

// uniform vec4 u_lightPos;
uniform Light light;
uniform vec4 u_viewPos;

// materials textures

uniform sampler2D albedoMap[3];
uniform sampler2D normalMap[3];
uniform sampler2D roughnessMap[3];
uniform sampler2D metallicMap[3];
uniform sampler2D u_shadowMap[1];

// IBL texture

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;


 vec3 getNormalFromMap() {

    int index = int(v_texIndex);

    if (index <= 2) {

        vec3 position3 = vec3(v_position);
        vec3 tangentNormal = texture(normalMap[index], v_texCord).xyz * 2.0 - 1.0;

        vec3 Q1 = dFdx(position3);
        vec3 Q2 = dFdy(position3);
        vec2 st1 = dFdx(v_texCord);
        vec2 st2 = dFdy(v_texCord);

        vec3 N = normalize(normal);
        vec3 T = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B = -normalize(cross(N,T));
        mat3 TBN = mat3(T, B, N);

        return normalize(TBN * tangentNormal);
    } 
}  

float distributionGGX(vec3 N, vec3 H, float roughness) {

    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;

}

float geometrySchlickGGX(float NdotV, float roughness) {

    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;

}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {

    float NdotV = max(dot(N,V), 0.0);
    float NdotL = max(dot(N,L), 0.0);

    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    float cosThetaNew = min(cosTheta, 1.0);


    return F0 + (1.0 - F0) * pow(1.0 - cosThetaNew, 5.0);
   

}

vec3 fresnelSchlickRough(float cosTheta, vec3 F0, float roughness) {

    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
   
}


float shadowCalculation(vec4 fragPosLightSpace) {

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_shadowMap[0], projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap[0], 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_shadowMap[0], projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow / 9.0;

    if (projCoords.z > 1.0) shadow = 0.0;  

//    shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}



void main() {

    

    int index = int(v_texIndex);
    if (index <= 2) {

        vec3 viewPos3 = vec3(u_viewPos);
        vec3 position3 = vec3(v_position);
        vec3 lightPos3 = vec3(light.position);
        vec3 lightDir3 = vec3(light.direction);


        vec3 albedo = pow(texture(albedoMap[index], v_texCord).rgb, vec3(2.2));
        float roughness = texture(roughnessMap[index], v_texCord).r;
        float metallic = texture(metallicMap[index], v_texCord).r;
        
        vec3 N = getNormalFromMap();
        vec3 V = normalize(viewPos3 - position3);      
//        vec3 V = normalize(-position3);
        vec3 R = reflect(-V, N);


        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);

        vec3 Lo = vec3(0.0);

        vec3 L = normalize(lightPos3 - position3);
//        vec3 L = -lightDir3;                                                   //directional light
        vec3 H = normalize(V + L);
        float distance = length(lightPos3 - position3);

        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.color * attenuation;
//        vec3 radiance = light.color;                                     //directional light

        // Cook-Torrence BRDF

        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        float HdotV = max(dot(H,V), 0.0);
        vec3 F = fresnelSchlick(HdotV, F0);
//        vec3 F = fresnelSchlickRough(HdotV, F0, roughness);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 Ks = F;
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= 1.0 - metallic;

        vec3 irradiance = texture(irradianceMap, N).rgb;

        float NdotL = max(dot(N, L), 0.0);


//        Lo += (Kd * albedo / PI ) * radiance * NdotL;
//        Lo += (specular) * radiance * NdotL;
        Lo += ((Kd * albedo / PI) + specular) * radiance * NdotL;

        // simple ambient light (no environment lighting)

//        vec3 ambient = vec3(0.03) * albedo;

        vec3 FR = fresnelSchlickRough(HdotV, F0, roughness);
        vec3 KsR = FR;
        vec3 KdR = 1.0 - KsR;



        // environment lighting

        vec3 diffuse = irradiance * albedo;
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(brdfLUT, vec2(max(dot(N,V), 0.0), roughness)).rg;
        vec3 specularC = prefilteredColor * (FR * brdf.x * brdf.y);


        vec3 ambient = (Kd * diffuse + specularC);
        
        vec3 finalColor = ambient + Lo;
//        vec3 finalColor = Lo;

      

        // HDR tonemapping and gamma correction
        finalColor = finalColor / (finalColor + vec3(1.0));
        finalColor = pow(finalColor, vec3(1.0/2.2));

        color = vec4(finalColor, 1.0);


        
//        color = vec4(ambient, 1.0);

    }
    else color = v_color;
   
}