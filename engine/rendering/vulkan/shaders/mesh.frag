#version 450
layout(location=0) in vec3 world_normal;
layout(location=1) in vec3 world_position;
layout(location=0) out vec4 out_color;
layout(set=0,binding=0) uniform Frame { mat4 view_projection; mat4 model; vec4 camera_position; } frame;
layout(set=0,binding=1) uniform Material { vec4 base_color; vec4 emissive; float metallic; float roughness; float reflectiveness; float wetness; float organic; float dryness; float normal_strength; float occlusion; } mat;
void main(){vec3 N=normalize(world_normal);vec3 L=normalize(vec3(.45,.75,.55));vec3 V=normalize(frame.camera_position.xyz-world_position);vec3 H=normalize(L+V);float ndl=max(dot(N,L),0.0);float spec=pow(max(dot(N,H),0.0),mix(8.0,128.0,1.0-clamp(mat.roughness,0.0,1.0)))*(0.04+0.96*clamp(mat.metallic,0.0,1.0));spec*=1.0+clamp(mat.wetness,0.0,1.0)*1.75;vec3 c=mat.base_color.rgb*ndl*(1.0-mat.metallic)+vec3(spec)+mat.emissive.rgb*mat.emissive.a;c*=mix(1.0,1.08,clamp(mat.organic,0.0,1.0));c*=mix(1.0,.94,clamp(mat.dryness,0.0,1.0));c*=clamp(mat.occlusion,0.0,1.0);out_color=vec4(c,mat.base_color.a);}