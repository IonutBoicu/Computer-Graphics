#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_normal;		

uniform mat4 model_matrix, view_matrix, projection_matrix;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform int material_shininess;
uniform float material_kd;
uniform float material_ks;
//uniform vec3 world_pos,world_normal;
//uniform float difuza,ambientala,speculara,;

out float light;

void main(){

	//TODO
	vec3 world_pos = (model_matrix * vec4(in_position,1)).xyz;
	vec3 world_normal = normalize(  mat3(model_matrix) *  in_normal );
	vec3 L = normalize ( light_position - world_pos);
	vec3  V  =  normalize  ( eye_position - world_pos);
	float difuza=material_kd*1*max(dot(world_normal,L),0);
	float ambientala=0.2f*material_kd;
	float speculara;
	if(dot(world_normal,L)>0)
	{
		speculara=material_ks*1*1*pow(max(dot(world_normal,normalize(L+V)),0),material_shininess);
	}
	else
		//speculara=material_ks*1*0*pow(max(dot(world_normal,normalize(L+V)),0),material_shininess);
		speculara=0;
	light=difuza+ambientala+speculara;
	gl_Position = projection_matrix*view_matrix*model_matrix*vec4(in_position,1); 
}
