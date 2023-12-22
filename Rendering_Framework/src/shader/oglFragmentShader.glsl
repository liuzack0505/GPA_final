#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;

layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;


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
	shadedColor.a = 1.0;	
	fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void pureColor(){
	vec4 shadedColor = withFog(vec4(1.0, 0.0, 0.0, 1.0)) ;
	fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void magicRockPass(){
	vec4 texel = texture(albedoTexture, f_uv.xy);
	vec4 shadedColor = withFog(texel);
	fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void airplanePass(){
	vec4 texel = texture(albedoTexture, f_uv.xy);
	vec4 shadedColor = withFog(texel);
	fragColor.rgb = pow(shadedColor.rgb, vec3(0.5));
	fragColor.a = shadedColor.a;
}

void main(){	
	if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	}
	else if(pixelProcessId == 1){
		magicRockPass();
	}
	else if (pixelProcessId == 2) {
		airplanePass();
	}
	else {
		pureColor();
	}
}