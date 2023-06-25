#version 400

// #define DISPLAY_LOD

#define TEMP_COLOR vec4(0.2f, 0.2f, 0.9f, 1.0f)

struct LIGHT {
	vec4 position; // assume point or direction in EC in this example shader
	vec4 ambient_color, diffuse_color, specular_color;
	vec4 light_attenuation_factors; // compute this effect only if .w != 0.0f
	vec3 spot_direction;
	float spot_exponent;
	float spot_cutoff_angle;
	bool light_on;
	vec4 position_MC;
};

struct MATERIAL {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec4 emissive_color;
	float specular_exponent;
};

uniform vec4 u_global_ambient_color;
#define NUMBER_OF_LIGHTS_SUPPORTED 10
uniform LIGHT u_light[NUMBER_OF_LIGHTS_SUPPORTED];
uniform MATERIAL u_material;

uniform sampler2D u_base_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_emissive_texture;

uniform bool u_flag_diffuse_texture_mapping = false;
uniform bool u_flag_normal_texture_mapping = false;
uniform bool u_flag_emissive_texture_mapping = false;
uniform bool u_normal_based_directX = false;

uniform bool u_flag_shading_effect = false;
uniform bool u_flag_light_effect = false;
uniform bool u_flag_light_effect2 = false;
uniform int u_light_effect_num = 2;
uniform int u_light_effect_num2 = 4;

uniform uint u_timestamp = 0;

uniform bool u_flag_blending = false;
uniform bool u_flag_fog = false;

uniform float u_fragment_alpha;

const float zero_f = 0.0f;
const float one_f = 1.0f;
const float TO_RADIAN = 0.01745329252f; 

in vec3 v_position_EC;
in vec3 v_normal_EC;
in vec2 v_tex_coord;
in vec2 v_temp_coord;

in vec3 v_position_MC;
in vec3 v_normal_MC;

layout (location = 0) out vec4 final_color;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_normal_texture, v_tex_coord).xyz * 2.0 - 1.0;
	if (u_normal_based_directX)
	    tangentNormal.z *= -1;  // for normal map based in directX

    vec3 Q1  = dFdx(v_position_EC);
    vec3 Q2  = dFdy(v_position_EC);
    vec2 st1 = dFdx(v_tex_coord);
    vec2 st2 = dFdy(v_tex_coord);

    vec3 N   = normalize(v_normal_EC);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec4 lighting_equation_textured(in vec3 P_EC, in vec3 N_EC, in vec4 base_color, in vec4 emissive_color) {
	vec4 color_sum;
	float local_scale_factor, tmp_float; 
	vec3 L_EC, L_MC;

	color_sum = emissive_color + u_global_ambient_color * base_color;
 
	for (int i = 0; i < NUMBER_OF_LIGHTS_SUPPORTED; i++) {
		if (!u_light[i].light_on) continue;

		local_scale_factor = one_f;
		vec4 diffuse_color = u_light[i].diffuse_color;

		if (u_light[i].position.w != zero_f) { // point light source
			L_EC = u_light[i].position.xyz - P_EC.xyz;

			if (u_light[i].light_attenuation_factors.w  != zero_f) {
				vec4 tmp_vec4;

				tmp_vec4.x = one_f;
				tmp_vec4.z = dot(L_EC, L_EC);
				tmp_vec4.y = sqrt(tmp_vec4.z);
				tmp_vec4.w = zero_f;
				local_scale_factor = one_f/dot(tmp_vec4, u_light[i].light_attenuation_factors);
			}

			L_EC = normalize(L_EC);

			if (u_light[i].spot_cutoff_angle < 180.0f) { // [0.0f, 90.0f] or 180.0f

				vec3 spot_dir = normalize(u_light[i].spot_direction);
				float spot_cutoff_angle = clamp(u_light[i].spot_cutoff_angle, zero_f, 90.0f);

				if (u_flag_light_effect && u_light_effect_num == i) {
					vec3 eye_uaxis = vec3(1.0f, 0.0f, 0.0f);
					vec3 on_plane_vector1 = normalize(cross(eye_uaxis, spot_dir));
					vec3 tmp_axis = normalize(cross(spot_dir, on_plane_vector1));

					vec3 on_plane_vector2 = normalize(cross(-L_EC, spot_dir));
					vec3 p_projected = normalize(cross(spot_dir, on_plane_vector2));
					float cos_angle = acos(dot(tmp_axis, p_projected)) / TO_RADIAN;
					cos_angle = abs(cos_angle);

					if (cos_angle >= 67.5f && cos_angle < 90.0f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f + 10.0f * (cos_angle - 67.5f) / 22.5f;
					}
					else if (cos_angle >= 45.0f && cos_angle < 67.5f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f * (cos_angle - 45.0f) / 22.5f;
					}
					else if (cos_angle >= 22.5f && cos_angle < 45.0f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f + 10.0f * (cos_angle - 22.5f) / 22.5f;
					}
					else if (cos_angle >= 0.0f && cos_angle < 22.5f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f * (cos_angle) / 22.5f;
					}
					else if (cos_angle >= 157.5f && cos_angle < 180.0f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f + 10.0f * (cos_angle - 157.5f) / 22.5f;
					}
					else if (cos_angle >= 135.0f && cos_angle < 157.5f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f * (cos_angle - 135.0f) / 22.5f;
					}
					else if (cos_angle >= 112.5f && cos_angle < 135.0f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f + 10.0f * (cos_angle - 112.5f) / 22.5f;
					}
					else if (cos_angle >= 90.0f && cos_angle < 112.5f) {
						spot_cutoff_angle = spot_cutoff_angle - 10.0f * (cos_angle - 90.0f) / 22.5f;
					}
				}
				else if (u_flag_light_effect2 && (u_light_effect_num2 == i)) {
					L_MC = u_light[i].position_MC.xyz - v_position_MC.xyz;
					vec3 spot_dir_MC = vec3(0.0f, 0.0f, -1.0f);
					vec3 xaxis = vec3(1.0f, 0.0f, 0.0f);
					vec3 on_plane_vector1 = normalize(cross(xaxis, spot_dir_MC));
					vec3 tmp_axis = normalize(cross(spot_dir_MC, on_plane_vector1));

					vec3 on_plane_vector2 = normalize(cross(-L_MC, spot_dir_MC));
					vec3 p_projected = normalize(cross(spot_dir_MC, on_plane_vector2));
					float cos_angle = acos(dot(tmp_axis, p_projected)) / TO_RADIAN;
					cos_angle = abs(cos_angle);

					int time_cycle = 90;

					if ((cos_angle >= 45.0f + (u_timestamp % time_cycle)) && (cos_angle < 90.0f + (u_timestamp % time_cycle))) {
						spot_cutoff_angle = spot_cutoff_angle - 30.0f * (cos_angle - (45.0f + (u_timestamp % time_cycle))) / 45.0f;
					}
					else if ((cos_angle >= 0.0f + (u_timestamp % time_cycle)) && (cos_angle < 45.0f + (u_timestamp % time_cycle))) {
						spot_cutoff_angle = spot_cutoff_angle - 30.0f + 30.0f * (cos_angle - (0.0f + (u_timestamp % time_cycle))) / 45.0f;
					}
					else if ((cos_angle >= 90.0f + (u_timestamp % time_cycle)) && (cos_angle < 135.0f + (u_timestamp % time_cycle))) {
						spot_cutoff_angle = spot_cutoff_angle - 30.0f + 30.0f * (cos_angle - (90.0f + (u_timestamp % time_cycle))) / 45.0f;
					}
					else if ((cos_angle >= 135.0f + (u_timestamp % time_cycle)) && (cos_angle < 180.0f + (u_timestamp % time_cycle))) {
						spot_cutoff_angle = spot_cutoff_angle - 30.0f * (cos_angle - (135.0f + (u_timestamp % time_cycle))) / 45.0f;
					}
				}

				tmp_float = dot(-L_EC, spot_dir);
				if (tmp_float >= cos(radians(spot_cutoff_angle))) {
					if (u_flag_light_effect && (u_light_effect_num == i)) {
						tmp_float = cos(90.0f * acos(tmp_float));
						if (tmp_float < 0.5f) {
							tmp_float = 0.5f;
						}
					}
					else if (u_flag_light_effect2 && (u_light_effect_num2 == i)) {
						tmp_float = cos(90.0f * acos(tmp_float) + (u_timestamp % 720) / 2.0f);
						if (tmp_float < 0.5f) {
							diffuse_color = TEMP_COLOR;
						}
					}
					else {
						tmp_float = pow(tmp_float, u_light[i].spot_exponent);
					}
				}
				else 
					tmp_float = zero_f;
				local_scale_factor *= tmp_float;
			}
		}
		else {  // directional light source
			L_EC = normalize(u_light[i].position.xyz);
		}	

		if (local_scale_factor > zero_f) {				
		 	vec4 local_color_sum = u_light[i].ambient_color * u_material.ambient_color;

			tmp_float = dot(N_EC, L_EC);  
			if (tmp_float > zero_f) {  
				local_color_sum += diffuse_color*base_color*tmp_float;
			
				vec3 H_EC = normalize(L_EC - normalize(P_EC));
				tmp_float = dot(N_EC, H_EC); 
				if (tmp_float > zero_f) {
					local_color_sum += u_light[i].specular_color
				                       *u_material.specular_color*pow(tmp_float, u_material.specular_exponent);
				}
			}
			color_sum += local_scale_factor * local_color_sum;
		}
	}
 	return color_sum;
}

// May contol these fog parameters through uniform variables
#define FOG_COLOR vec4(0.7f, 0.7f, 0.7f, 1.0f)
#define FOG_NEAR_DISTANCE 350.0f
#define FOG_FAR_DISTANCE 700.0f

void main(void) {
	vec4 base_color, emissive_color, shaded_color;
	vec3 normal;
	float fog_factor;

	if (u_flag_diffuse_texture_mapping) 
		base_color = texture(u_base_texture, v_tex_coord);
	else 
		base_color = u_material.diffuse_color;

	if (u_flag_emissive_texture_mapping) 
		emissive_color = texture(u_emissive_texture, v_tex_coord);
	else 
		emissive_color = u_material.emissive_color;

	if (u_flag_normal_texture_mapping)
		normal = getNormalFromMap();
	else
		normal = v_normal_EC;
		
	shaded_color = lighting_equation_textured(v_position_EC, normalize(normal), base_color, emissive_color);

	if (u_flag_fog) {
 	  	fog_factor = (FOG_FAR_DISTANCE - length(v_position_EC.xyz))/(FOG_FAR_DISTANCE - FOG_NEAR_DISTANCE);  		
		fog_factor = clamp(fog_factor, 0.0f, 1.0f);
		final_color = mix(FOG_COLOR, shaded_color, fog_factor);
	}
	else {
		final_color = shaded_color;
	}

	if (u_flag_shading_effect){
		for (int i = 1; i <= 10; i++){
			if ((v_temp_coord.x + 0.4f * (i - 5) + (u_timestamp % 200) / 250.0f - 1.0f) >= v_temp_coord.y && (v_temp_coord.x + 0.4f * (i - 6) + (u_timestamp % 200) / 250.0f - 1.0f) < v_temp_coord.y && (i % 2 == 1)){
				discard;
			}
		}
	}

	if (u_flag_blending) {
		final_color = vec4(u_fragment_alpha * final_color.rgb, u_fragment_alpha);
	}
}
