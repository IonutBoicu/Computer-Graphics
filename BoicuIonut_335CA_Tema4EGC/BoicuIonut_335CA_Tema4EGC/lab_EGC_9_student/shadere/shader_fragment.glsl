#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D textura1;
uniform sampler2D textura2;
uniform int has_alpha;
uniform int do_discard;
uniform vec3 light_position;
uniform vec3 forward_position;
uniform vec3 eye_position;
uniform int material_shininess;
uniform float material_kd;
uniform float material_ks;

in vec3 world_pos;
in vec3 world_normal;
in vec2 texcoord;
in vec3 posxyz;

void main(){

	// TODO
	// Retrive texel from each textures
	vec3 texel1 = texture(textura1, texcoord).xyz;
	vec3 texel2 = texture(textura2, texcoord).xyz;

	// TODO
	// Discard current fragment if alpha mask (second texture) is activated
	if(has_alpha == 1) {
		if(texel2.x < 0.1 && texel2.y < 0.1 && texel2.z < 0.1)
			discard;
	}
	// discard of beyond a point
	if(do_discard == 1) {
		if(posxyz.x < -9 || posxyz.x > 9)
			discard;
	}


	// TODO
	// Output fragment color using the retrived texel
	out_color = vec4(texel1,1);

		//TODO
	vec3 L = normalize (light_position - world_pos);
	vec3 V = normalize (eye_position - world_pos);
	vec3 H = normalize (L+V);

	float ambientala = material_kd;
	float difuza = material_kd * (max(dot(world_normal, L),0));
	
	int primesteLumina = (dot(world_normal, L)>0)?1:0;
	float specular = material_ks * primesteLumina * pow(max(dot(H, world_normal),0), material_shininess);

	float amplificare = 200;
	float factorAtenuare = (1/(pow(distance(light_position, world_pos), 2) + 1)) * amplificare;
	
	float light = ambientala + factorAtenuare * (difuza + specular);

	vec3 VCT = normalize (world_pos - light_position);
	vec3 direction = vec3(forward_position.x,forward_position.y, forward_position.z);
	float cos = dot(VCT,direction)/(length(VCT)*length(direction));
	
	if(cos > 0.8)
		out_color = vec4(texel1.x * light, texel1.y * light, texel1.z *light,1);
	else
		out_color = vec4(ambientala * texel1.x/4, ambientala * texel1.y/4, ambientala*texel1.z/4,1);
}