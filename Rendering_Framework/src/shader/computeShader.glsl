#version 430 core

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in ;

struct DrawCommand{
	uint count ;
	uint instanceCount ;
	uint firstIndex ;
	uint baseVertex ;
	uint baseInstance ;
};

struct Sample{
	vec4 position ; 
};

struct rawSample{
	vec4 position;
	vec4 extra_data;// (type, center.y, radius, 0)
};

layout (std430, binding=3) buffer DrawCommandsBlock{
	DrawCommand commands[] ;
};

layout (std430, binding=1) buffer InstanceData{
	rawSample rawInstanceProps[] ;
};

layout (std430, binding=2) buffer CurrValidInstanceData{
	Sample currValidInstanceProps[] ;
};


layout (location = 0) uniform int GrassMaxInstance;
layout (location = 1) uniform int Bush1MaxInstance;
layout (location = 2) uniform int Bush2MaxInstance;
layout (location = 3) uniform int Building1MaxInstance;
layout (location = 4) uniform int Building2MaxInstance;
layout (location = 5) uniform vec4 leftFace;
layout (location = 6) uniform vec4 bottomFace;
layout (location = 7) uniform vec4 nearFace;
layout (location = 8) uniform vec4 rightFace;
layout (location = 9) uniform vec4 topFace;
layout (location = 10) uniform vec4 farFace;

bool getClipspace(rawSample data){
	vec4 instance_center = vec4(data.position.xyz, 1.0);
	instance_center.y += data.extra_data.y;

	//decided the radius
	float radius = data.extra_data.z;

	bool isCulledBottome = dot(instance_center, bottomFace) < -radius;
	bool isCulledTop = dot(instance_center, topFace) < -radius;
	bool isCulledLeft = dot(instance_center, leftFace) < -radius;
	bool isCulledRight = dot(instance_center, rightFace) < -radius;
	bool isCulledNear = dot(instance_center, nearFace) < -radius;
	bool isCulledFar = dot(instance_center, farFace) < 0;
	return isCulledBottome || isCulledTop || isCulledLeft || isCulledRight || isCulledNear || isCulledFar;
	/*bool isCulledLeft = dot(instance_center, leftFace) < 0;
	vec4 clipSpaceV = viewProjMat * instance_center ;
	clipSpaceV = clipSpaceV / clipSpaceV.w ;
	return (clipSpaceV.x < -1.0) || (clipSpaceV.x > 1.0) || (clipSpaceV.y < -1.0) || (clipSpaceV.y > 1.0) || (clipSpaceV.z < -1.1) || (clipSpaceV.z > 1.0) ;*/
	
	
}

void main(){
	const uint idx = gl_GlobalInvocationID.x;
	if(idx >= Building2MaxInstance){ return; } 

	rawSample cur = rawInstanceProps[idx];
	bool frustumCulled = getClipspace(cur);
	if(frustumCulled == false){
		if(cur.extra_data.x == 4){
			const uint UNIQUE_IDX = atomicAdd(commands[4].instanceCount, 1);
			currValidInstanceProps[Building1MaxInstance + UNIQUE_IDX].position = vec4(cur.position.xyz, idx) ;
		} else if(cur.extra_data.x == 3){
			const uint UNIQUE_IDX = atomicAdd(commands[3].instanceCount, 1);
			currValidInstanceProps[Bush2MaxInstance + UNIQUE_IDX].position = vec4(cur.position.xyz, idx) ;
		} else if(cur.extra_data.x == 2){
			const uint UNIQUE_IDX = atomicAdd(commands[2].instanceCount, 1);
			currValidInstanceProps[Bush1MaxInstance + UNIQUE_IDX].position = vec4(cur.position.xyz, idx) ;
		} else if(cur.extra_data.x == 1){
			const uint UNIQUE_IDX = atomicAdd(commands[1].instanceCount, 1);
			currValidInstanceProps[GrassMaxInstance + UNIQUE_IDX].position = vec4(cur.position.xyz, idx) ;
		} else {
			const uint UNIQUE_IDX = atomicAdd(commands[0].instanceCount, 1);
			currValidInstanceProps[UNIQUE_IDX].position = vec4(cur.position.xyz, idx) ;
		}
	}
}