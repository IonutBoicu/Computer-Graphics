#version 330
layout(location = 0) out vec4 out_color;

in float light;

void main(){

	out_color = vec4(light* 0.1686,light*0.7333, light,1);
}