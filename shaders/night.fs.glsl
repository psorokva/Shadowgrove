#version 330

uniform sampler2D screen_texture;
uniform sampler2D shad00;
uniform sampler2D shad01;
uniform sampler2D dark00;
uniform sampler2D dark01;
uniform sampler2D light00;
uniform sampler2D light01;
uniform sampler2D light02;
uniform int torch_mode;
uniform int torch_anim;
uniform float time;
uniform float darken_screen_factor;
uniform float night_timer;
uniform float night_length_full;
uniform float night_length_0;
uniform float night_length_1;
uniform float night_length_2;
uniform float night_length_3;
uniform float night_length_4;
uniform float night_length_5;

in vec2 texcoord;
in float dist_to_player;

layout(location = 0) out vec4 color;

vec4 color_shift(vec4 in_color) {
    vec4 out_color = vec4(0.5, 0.5, 1.0, 1.0);
    vec4 sunset = vec4(0.7, 0.4, 0.6, 0.0);
    vec4 night = vec4(0.327, 0.185, 0.33, 0.0);
    vec4 sunrise = vec4(0.5, 0.4, 0.5, 0.0);
    //float dist_to_player = 300.0;
    float duration = 150.0;
    float sunsetDuration = 50.0;
    float nightDuration = 100.0;

    float time_adjusted = mod(time, duration);
    float effect = 0.6; 
    
    float INNER_RAD = 4.00;
    float OUTER_RAD = 8.50;

    if (dist_to_player <= INNER_RAD) {
       effect = 0.0;
    } else if (dist_to_player <= OUTER_RAD) {
       effect = 0.9 * (dist_to_player - INNER_RAD) / (OUTER_RAD - INNER_RAD);
    }   
    
    if (time_adjusted <= sunsetDuration) {
        // sunset to night transition
        float t = time_adjusted / sunsetDuration;
        out_color = in_color * (1.0 - effect) - effect * mix(sunset, night, t);
    } else if (time_adjusted <= nightDuration) {
        out_color = in_color * (1.0 - effect) - effect * night;
    } else if (time_adjusted <= duration) {
        // night to sunrise transition
        float t = (time_adjusted - nightDuration) / (duration - nightDuration);
        out_color = in_color * (1.0 - effect) - effect * mix(night, sunrise, t);
    }

    return out_color;
}

vec4 fade_color(vec4 in_color) {
	if (darken_screen_factor > 0)
		in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main() {
    vec4 in_color = texture(screen_texture, texcoord);
    vec4 shad_color = texture(shad00, texcoord); 
    vec4 shad1_color = texture(shad01, texcoord); 
    vec4 dark_color = texture(dark00, texcoord); 
    vec4 dark1_color = texture(dark01, texcoord); 
    vec4 light_color = texture(light00, texcoord); 
   
    float day_portion = 1.0;
    float eve_portion = 0.0;
    float dark_portion = 0.0;
    
    if (night_timer <= night_length_0 ) {
       eve_portion = night_timer / night_length_0;
       day_portion = 1.0 - eve_portion;
    } else if (night_timer <= night_length_1) {
        day_portion = 0.0;
        eve_portion = 1.0;
    } else if (night_timer <= night_length_2) {
        day_portion = 0.0;
        dark_portion = (night_timer - night_length_1) / (night_length_2 - night_length_1);
        eve_portion = 1.0 - dark_portion;
    } else if (night_timer <= night_length_3) {
        day_portion = 0.0;
        eve_portion = 0.0;
        dark_portion = 1.0;
    } else if (night_timer <= night_length_4) {
        eve_portion = (night_timer - night_length_3) / (night_length_4 - night_length_3);
        dark_portion = 1.0 - eve_portion;
        day_portion = 0.0;
    } else if (night_timer <= night_length_5) {
        dark_portion = 0.0;
        eve_portion = 1.0;
        day_portion = 0.0;
    } else if (night_timer <= night_length_full) {
        day_portion = (night_timer - night_length_5) / (night_length_full - night_length_5);
        eve_portion = 1.0 - day_portion;
        dark_portion = 0.0;
    }

    /// DAYTIME and Home Screen 
    color = in_color;

    // If using a torch
    if (torch_mode == 1 ){
        // change light color based on animation tick for flickering torch effect
        if (torch_anim == 2 ) {
            light_color = texture(light02, texcoord); 
        } else if (torch_anim == 1 ) {
            light_color = texture(light01, texcoord); 
        }
        /// DAYTIME WITH TORCH
        vec4 day_color = in_color + 0.2 * vec4(light_color.r*light_color.a, light_color.g*light_color.a, light_color.b*light_color.a, 1.0);
        // vec4 color = in_color * 1.05 * vec4(dark1_color.r*dark1_color.a, dark1_color.g*dark1_color.a, dark1_color.b*dark1_color.a, 1.0);

        /// EVE TORCH
        vec4 eve_color = in_color * vec4(shad1_color.r*shad1_color.a, shad1_color.g*shad1_color.a, shad1_color.b*shad1_color.a, 1.0);
        eve_color += 0.4 * vec4(light_color.r*light_color.a, light_color.g*light_color.a, light_color.b*light_color.a, 1.0);
    
        // MIGNIGHT WITH TORCH
        vec4 night_color = in_color * vec4(dark1_color.r*dark1_color.a, dark1_color.g*dark1_color.a, dark1_color.b*dark1_color.a, 1.0);
        night_color = color + 0.4 * vec4(light_color.r*light_color.a, light_color.g*light_color.a, light_color.b*light_color.a, 1.0);
      
        color = day_portion * day_color + eve_portion * eve_color + dark_portion * night_color;
    }

    /// Otherwise no torch
    if (torch_mode == 0 ){
        // Day
        vec4 day_color = in_color;
        // EVE NO TORCH
        vec4  eve_color = in_color * vec4(shad_color.r*shad_color.a, shad_color.g*shad_color.a, shad_color.b*shad_color.a, 1.0);
        /// MIDNIGHT NO TORCH
        vec4 night_color = in_color * vec4(dark_color.r*dark_color.a, dark_color.g*dark_color.a, dark_color.b*dark_color.a, 1.0);
        color = day_portion * day_color + eve_portion * eve_color + dark_portion * night_color;
    }

    // don't do night stuff for easier debugging rn
    color = in_color;
}
