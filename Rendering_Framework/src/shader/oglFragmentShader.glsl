#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 L;
in vec3 H;
in vec3 N;

layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 6) uniform sampler2D normalMap ;
layout(location = 10) uniform bool magicRockNormalMapping;

uniform sampler2DArray TextureArray3D;

const vec3 la = vec3(0.2);
const vec3 ld = vec3(0.64);
const vec3 ls = vec3(0.16);

vec4 withFog(vec4 color){
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1) ;
	const float MAX_DIST = 400.0 ;
	const float MIN_DIST = 350.0 ;
	
	float dis = length(f_viewVertex) ;
	float fogFactor = (MAX_DIST - dis) / (MAX_DIST - MIN_DIST) ;
	fogFactor = clamp(fogFactor, 0.0, 1.0) ;
	fogFactor = fogFactor * fogFactor ;
	
	vec4 colorWithFog = mix(FOG_COLOR, color, fogFactor) ;
	return colorWithFog ;
}


void terrainPass(){
	vec4 texel = texture(albedoTexture, f_uv.xy);
	vec4 shadedColor = withFog(texel); 

	vec3 ka = shadedColor.xyz;
	vec3 kd = ka;

	vec3 ambient = ka * la;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * ld;
	shadedColor.a = 1.0;	
	fragColor.rgb = pow(ambient + diffuse, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void pureColor(){
	vec4 shadedColor = withFog(vec4(1.0, 0.0, 0.0, 1.0)) ;
	fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void magicRockPass(){
	vec4 texel = texture(albedoTexture, f_uv.xy);
	//vec4 shadedColor = withFog(texel);

	vec3 ka = texel.rgb;
	vec3 kd = ka;
	vec3 ks = vec3(1.0);
	float shiness = 32;

	vec3 ambient = ka * la;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * ld;

	vec3 N_normalMapping = normalize(texture(normalMap, f_uv.xy).rgb * 2.0 - vec3(1.0));
	vec3 specular_normalMapping =  pow(max(dot(N_normalMapping, H), 0.0), shiness) * ks * ls;
	vec3 sum3_normalMapping = ambient + diffuse + specular_normalMapping;
	vec4 sum4_normalMapping = vec4(sum3_normalMapping, 1.0);
	vec4 shadedColor_normalMapping = withFog(sum4_normalMapping);

	vec3 specular = pow(max(dot(N, H), 0.0), shiness) * ks * ls;
	vec3 sum3 = ambient + diffuse + specular;
	vec4 sum4 = vec4(sum3, 1.0);
	vec4 shadedColor = withFog(sum4);

	if (magicRockNormalMapping) {
		fragColor.rgb = pow(shadedColor_normalMapping.rgb, vec3(0.5));
		fragColor.a = shadedColor_normalMapping.a;
	}
	else {
		fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
		fragColor.a = shadedColor.a;
	}
}

void airplanePass(){
	vec4 texel = texture(albedoTexture, f_uv.xy);
	vec4 shadedColor = withFog(texel);

	vec3 ka = shadedColor.xyz;
	vec3 kd = ka;
	vec3 ks = vec3(1.0);
	float shiness = 32;

	vec3 ambient = ka * la;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * ld;
	vec3 specular =  pow(max(dot(N, H), 0.0), shiness) * ks * ls;

	fragColor.rgb = pow(ambient + diffuse + specular, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void foliagesPass(){
	vec4 texel = texture(TextureArray3D, f_uv);
	if(texel.a < 0.5){
		discard;
	}
	vec4 shadedColor = withFog(texel);

	vec3 ka = shadedColor.xyz;
	vec3 kd = ka;

	vec3 ambient = ka * la;
	vec3 diffuse = max(dot(N, L), 0.0) * kd * ld;

	fragColor.rgb = pow(ambient + diffuse, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void main(){	
	if(pixelProcessId == 1){
		magicRockPass();
	}
	else if (pixelProcessId == 2) {
		airplanePass();
	}
	else if (pixelProcessId == 3) {
		foliagesPass();
	}
	else if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	}
	else {
		pureColor();
	}
}