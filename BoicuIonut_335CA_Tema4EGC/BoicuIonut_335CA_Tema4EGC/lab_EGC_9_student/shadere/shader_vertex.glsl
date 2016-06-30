#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 2) in vec2 in_texcoord;
layout(location = 1) in vec3 in_normal;		
uniform mat4 model_matrix, view_matrix, projection_matrix;

// TODO send tex_coord to fragment shader
out vec2 texcoord;
out vec3 posxyz;
out vec3 world_pos;
out vec3 world_normal;

void main(){

	// TODO - send texture coordinates to Fragment Shader
	texcoord = in_texcoord;
	posxyz = in_position;

	//calculeaza vectori in WORLD SPACE
	world_pos = (model_matrix * vec4(in_position,1)).xyz;
	world_normal = normalize( mat3(model_matrix) * in_normal );
	//world_normal = normalize(in_normal);

	gl_Position = projection_matrix*view_matrix*model_matrix*vec4(in_position,1); 
}
