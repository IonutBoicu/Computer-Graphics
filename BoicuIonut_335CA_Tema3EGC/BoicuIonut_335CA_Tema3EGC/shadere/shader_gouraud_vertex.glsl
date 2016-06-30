#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_normal;		

uniform mat4 model_matrix, view_matrix, projection_matrix;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform int material_shininess;
uniform float material_kd;
uniform float material_ks;
uniform float timp;
uniform float amplitudine;
uniform float faza;
uniform float frecventa;
uniform vec2 directie;
uniform vec2 centru;
uniform int directional;
uniform int circular;
uniform int toDraw;

out float light;

void main(){
	vec3 sum_normal;
	vec3 position, position1, position2, position3, position4;
	vec2 dirCirc, dir1, dir2, dir3, dir4;
	if((toDraw == 0) && (directional == 1 || circular == 1)) {
	// initializari
		vec2 dir;
		dir.x = in_position.x; dir.y = in_position.z;
		position.x = in_position.x; position.z = in_position.z;
	// 1
		dir1.x = in_position.x + 1; dir1.y = in_position.z;
		position1.x = in_position.x + 1; position1.z = in_position.z;
	// 2
		dir2.x = in_position.x; dir2.y = in_position.z - 1;
		position2.x = in_position.x; position2.z = in_position.z - 1;
	// 3
		dir3.x = in_position.x - 1; dir3.y = in_position.z;
		position3.x = in_position.x - 1; position3.z = in_position.z;
	// 4
		dir4.x = in_position.x; dir4.y = in_position.z + 1;
		position4.x = in_position.x; position4.z = in_position.z + 1;

		if(circular == 1) {
			dirCirc = (centru - dir) / sqrt((pow(centru.x - dir.x, 2) + pow(centru.y - dir.y, 2)));
			position.y += 2 * amplitudine * pow((sin(dot(dirCirc, dir) * frecventa + timp * faza) + 1) / 2, 2);
			// 1) x + 1, z
			vec2 dirCirc1 = (centru - dir1) / sqrt((pow(centru.x - dir1.x, 2) + pow(centru.y - dir1.y, 2)));
			position1.y += 2 * amplitudine * pow((sin(dot(dirCirc1, dir1) * frecventa + timp * faza) + 1) / 2, 2);

			// 2) x, z -1
			vec2 dirCirc2 = (centru - dir2) / sqrt((pow(centru.x - dir2.x, 2) + pow(centru.y - dir2.y, 2)));
			position2.y += 2 * amplitudine * pow((sin(dot(dirCirc2, dir2) * frecventa + timp * faza) + 1) / 2, 2);

			// 3) x - 1, z
			vec2 dirCirc3 = (centru - dir3) / sqrt((pow(centru.x - dir3.x, 2) + pow(centru.y - dir3.y, 2)));
			position3.y += 2 * amplitudine * pow((sin(dot(dirCirc3, dir3) * frecventa + timp * faza) + 1) / 2, 2);

			// 4) x , z + 1
			vec2 dirCirc4 = (centru - dir4) / sqrt((pow(centru.x - dir4.x, 2) + pow(centru.y - dir4.y, 2)));
			position4.y += 2 * amplitudine * pow((sin(dot(dirCirc4, dir4) * frecventa + timp * faza) + 1) / 2, 2);
			
		}
		if(directional == 1) {
			position.y += 2 * amplitudine * pow((sin(dot(directie, dir) * frecventa + timp * faza) + 1) / 2, 2);
			// 1) x + 1, z
			position1.y += 2 * amplitudine * pow((sin(dot(directie, dir1) * frecventa + timp * faza) + 1) / 2, 2);

			// 2) x, z -1
			position2.y += 2 * amplitudine * pow((sin(dot(directie, dir2) * frecventa + timp * faza) + 1) / 2, 2);

			// 3) x - 1, z
			position3.y += 2 * amplitudine * pow((sin(dot(directie, dir3) * frecventa + timp * faza) + 1) / 2, 2);

			// 4) x , z + 1
			position4.y += 2 * amplitudine * pow((sin(dot(directie, dir4) * frecventa + timp * faza) + 1) / 2, 2);
		}
		
		sum_normal = cross((position1 - position), (position2 - position)) +
							cross((position2 - position), (position3 - position)) +
							cross((position3 - position), (position4 - position)) +
							cross((position4 - position), (position1 - position));
		sum_normal = normalize(sum_normal);
		
	}
	else {
		position = in_position;
		sum_normal = in_normal;
	}

	vec3 world_pos = (model_matrix * vec4(position,1)).xyz;
	vec3 world_normal = normalize(  mat3(model_matrix) *  sum_normal);
	vec3 L = normalize ( light_position - world_pos);
	vec3  V  =  normalize  ( eye_position - world_pos);
	float difuza=material_kd*1*max(dot(world_normal,L),0);
	float ambientala=0.2f*material_kd;
	float speculara;
	speculara=material_ks*1*1*pow(max(dot(world_normal,normalize(L+V)),0),material_shininess);

	light=difuza+ambientala+speculara;
	gl_Position = projection_matrix*view_matrix*model_matrix*vec4(position,1); 
}
