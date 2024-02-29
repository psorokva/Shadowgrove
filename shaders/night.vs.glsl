#version 330

in vec3 in_position;

//uniform vec2 player_position;

out vec2 texcoord;
out float dist_to_player;

void main()  {
	vec3 player_position = vec3(1.0, -1.0, 0.0);
	
    gl_Position = vec4(in_position.xy, 0, 1.0);
	float dx = player_position.x - in_position.x;
	float dy = player_position.y - in_position.y;
	//float dx = player_position.x - gl_Position.x;
	//float dy = player_position.y - gl_Position.y;
	dist_to_player = sqrt(dx*dx + dy*dy);
	texcoord = (in_position.xy + 1) / 2.f;
}
