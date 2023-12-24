#version 430 core

layout(location=0) in vec3 v_vertex;
layout(location=1) in vec3 v_normal ;
layout(location=2) in vec3 v_uv ;
layout(location=3) in vec4 v_worldPosOffset; // only use in foliageProcess

out vec3 f_viewVertex ;
out vec3 f_uv ;
out vec3 L;
out vec3 H;
out vec3 N;
out vec3 world_vertex;
out vec3 world_normal;

layout(location = 0) uniform mat4 modelMat ;
layout(location = 5) uniform sampler2D elevationMap ;
layout(location = 6) uniform sampler2D normalMap ;
layout(location = 7) uniform mat4 viewMat ;
layout(location = 8) uniform mat4 projMat ;
layout(location = 9) uniform mat4 terrainVToUVMat;
layout(location = 1) uniform int vertexProcessIdx ;

layout (std430, binding=4) buffer RotationData{
	mat4 rotationData[] ;
};

void commonProcess(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) ;
	vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;

	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal ;

	//L = (viewMat * modelMat * vec4(0.4, 0.5, 0.8, 0.0)).xyz;
	L = vec3(0.4, 0.5, 0.8);
	L = normalize(L);
	N = normalize(viewNormal.xyz);
	vec3 V = -viewVertex.xyz;
	H = normalize(L + V);

	f_viewVertex = viewVertex.xyz;
	f_uv = v_uv ;

	world_vertex = worldVertex.xyz;
	world_normal = worldNormal.xyz;

	gl_Position = projMat * viewVertex ;
}

void foliagesProcess(){
	mat4 rotationMat = rotationData[uint(v_worldPosOffset.a)];
	vec4 worldVertex = rotationMat * modelMat * vec4(v_vertex, 1.0) + vec4(v_worldPosOffset.xyz, 0.0);
	vec4 worldNormal = rotationMat * modelMat * vec4(v_normal, 0.0) ;

	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal ;

	//L = (viewMat * modelMat * vec4(0.4, 0.5, 0.8, 0.0)).xyz;
	L = vec3(0.4, 0.5, 0.8);
	L = normalize(L);
	N = normalize(viewNormal.xyz);
	vec3 V = -viewVertex.xyz;
	H = normalize(L + V);

	f_viewVertex = viewVertex.xyz;
	f_uv = v_uv ;

	world_vertex = worldVertex.xyz;
	world_normal = worldNormal.xyz;

	gl_Position = projMat * viewVertex ;
}

void terrainProcess(){
	vec4 worldV = modelMat * vec4(v_vertex, 1.0) ;
	// calculate uv
	vec4 uv = terrainVToUVMat * worldV ;
	uv.y = uv.z ;
	// get height from map
	float h = texture(elevationMap, uv.xy).r ;
	worldV.y = h;		
	// get normal from map
	vec4 normalTex = texture(normalMap, uv.xy) ;
	// [0, 1] -> [-1, 1]
	normalTex = normalTex * 2.0 - 1.0 ;
		
	// transformation	
	vec4 viewVertex = viewMat * worldV ;
	vec4 viewNormal = viewMat * vec4(normalTex.rgb, 0) ;	

	L = vec3(0.4, 0.5, 0.8);
	L = normalize(L);
	N = normalize(viewNormal.xyz);
	vec3 V = -viewVertex.xyz;
	H = normalize(L + V);
	
	f_viewVertex = viewVertex.xyz;
	f_uv = uv.xyz ;

	world_vertex = worldV.xyz;
	world_normal = normalTex.rgb;

	gl_Position = projMat * viewVertex ;
}

void main(){
	if(vertexProcessIdx == 0){
		commonProcess() ;
	}
	else if(vertexProcessIdx == 1){
		foliagesProcess();
	}
	else if(vertexProcessIdx == 3){
		terrainProcess() ;
	}
	else{
		commonProcess() ;
	}	
}