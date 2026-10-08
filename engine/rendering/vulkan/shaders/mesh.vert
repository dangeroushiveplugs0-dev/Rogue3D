#version 450
layout(location=0) in vec3 in_position; layout(location=1) in vec3 in_normal; layout(location=2) in vec2 in_uv;
layout(set=0,binding=0) uniform Frame { mat4 view_projection; mat4 model; vec4 camera_position; } frame;
layout(location=0) out vec3 world_normal; layout(location=1) out vec3 world_position;
void main(){vec4 w=frame.model*vec4(in_position,1.0);world_position=w.xyz;world_normal=normalize(mat3(frame.model)*in_normal);gl_Position=frame.view_projection*w;}