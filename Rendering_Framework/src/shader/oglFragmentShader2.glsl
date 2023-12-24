#version 430 core

layout (location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler2D position_map;
layout (binding = 1) uniform sampler2D normal_map;
layout (binding = 2) uniform sampler2D diffuse_map;
layout (binding = 3) uniform sampler2D ambient_map;
layout (binding = 4) uniform sampler2D specular_map;

vec3 L = vec3(0.4, 0.5, 0.8);

const vec3 la = vec3(0.2);
const vec3 ld = vec3(0.64);
const vec3 ls = vec3(0.16);

void main() 
{
	vec4 result = vec4(0, 0, 0, 1.0);
	vec3 gPosition = texelFetch(position_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 gNormal = texelFetch(normal_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 gAmbient = texelFetch(ambient_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 gSpecular = texelFetch(specular_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 gDiffuse = texelFetch(diffuse_map, ivec2(gl_FragCoord.xy), 0).rgb;

	L = normalize(L);
	float dist = length(L);  
	vec3 V = -gPosition;
	vec3 H = normalize(L + V);

	vec3 ambient = gAmbient * la;
	float attenuation = 50.0 / (pow(dist, 2.0) + 1.0);
	vec3 diffuse = max(dot(gNormal, L), 0.0) * gDiffuse * ld;
	vec3 specular =  pow(max(dot(gNormal, H), 0.0), 32) * gSpecular * ls;
	result += vec4(( diffuse+ specular) * attenuation, 0.0);
	

	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	fragColor = result;
}