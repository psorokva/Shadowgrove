#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;
in float dist_to_player;

layout(location = 0) out vec4 color;

vec4 color_shift(vec4 in_color) {
    vec4 out_color = vec4(0.5, 0.5, 1.0, 1.0);;
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
    color = color_shift(in_color);
    color = fade_color(color);
}