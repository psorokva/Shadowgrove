// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity, const mat3 &projection) {

	Motion &motion = registry.motions.get(entity);
    ScreenState& screen = registry.screenStates.components[0];
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
    if (screen.game_screen == game_screen_night) {
        transform.translate(motion.camera_position);
    } else {
        transform.translate(motion.position);
    }
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		assert(in_position_loc >= 0);
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glBindVertexArray(vao);
		glEnableVertexAttribArray(in_position_loc);
		gl_has_errors();
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer( in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),(void *)sizeof(vec3)); 
			// note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}

	else if (render_request.used_effect == EFFECT_ASSET_ID::INVENTORY_LINE) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();
		glBindVertexArray(vao);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

	} else {
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// Duplicated from above to use for immobile entities
// Might need to optimize in the future
void RenderSystem::drawTexturedStaticMesh(Entity entity, const mat3& projection) {

//    printf("in textured static mesh with entity %zu done \n", entity);
	Immobile& immobile = registry.immobiles.get(entity);
    ScreenState& screen = registry.screenStates.components[0];
//    printf("entity immobile %zu done\n", entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
    if (screen.game_screen == game_screen_night) {
        transform.translate(immobile.camera_position);
    } else {
        transform.translate(immobile.position);
    }
	transform.scale(immobile.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	glBindVertexArray(vao);

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	} else {
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}


// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen() {
	// Setting shaders
	// get the wind texture, sprite mesh, and program
    glUseProgram(effects[(GLuint) EFFECT_ASSET_ID::NIGHT]);
    gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(vao);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint night_program = effects[(GLuint)EFFECT_ASSET_ID::NIGHT];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(night_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(night_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	
	// add player position to shader
	//GLint player_position = glGetAttribLocation(night_program, "player_position");
	//vec2 pp = vec2{ 500.0, 450.0 };
	//glUniform2f(player_position, (float)pp.x , (float)pp.y);
	
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(night_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0 + 0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// also bind the shadow overlays  ///////////////////////////////////////////////////////////////////////////////////////

	//NightTimer& nighttime = registry.nightTimer.components[0];
	GLuint night_timer = glGetUniformLocation(night_program, "night_timer");
	glUniform1f(night_timer, screen.night_counter_ms);

	GLuint night_length_full = glGetUniformLocation(night_program, "night_length_full");
	glUniform1f(night_length_full, NIGHT_DURATION);
	
	GLuint night_length_0 = glGetUniformLocation(night_program, "night_length_0");
	glUniform1f(night_length_0, NIGHT_DURATION_0);
	GLuint night_length_1 = glGetUniformLocation(night_program, "night_length_1");
	glUniform1f(night_length_1, NIGHT_DURATION_1);
	GLuint night_length_2 = glGetUniformLocation(night_program, "night_length_2");
	glUniform1f(night_length_2, NIGHT_DURATION_2);
	GLuint night_length_3 = glGetUniformLocation(night_program, "night_length_3");
	glUniform1f(night_length_3, NIGHT_DURATION_3);
	GLuint night_length_4 = glGetUniformLocation(night_program, "night_length_4");
	glUniform1f(night_length_4, NIGHT_DURATION_4);
	GLuint night_length_5 = glGetUniformLocation(night_program, "night_length_5");
	glUniform1f(night_length_5, NIGHT_DURATION_5);

    GLuint alive = glGetUniformLocation(night_program, "alive");
    Player& player = registry.players.components[0];
    glUniform1i(alive, player.alive);

    GLuint hit = glGetUniformLocation(night_program, "hit");
    glUniform1i(hit, player.hit);

	int torches = registry.torches.size();
	GLuint active_torches = glGetUniformLocation(night_program, "active_torches");
	glUniform1i(active_torches, torches);
    
	if (registry.helpScreen.size() > 0) {
		GLuint help = glGetUniformLocation(night_program, "help");
		HelpScreen& help_screen = registry.helpScreen.components[0];
		glUniform1i(help, help_screen.help);
	}
	GLuint torch_timer = glGetUniformLocation(night_program, "torch_timer");
	glUniform1f(torch_timer, screen.torch_timer_ms);

	GLuint torch_mode = glGetUniformLocation(night_program, "torch_mode");
	glUniform1i(torch_mode, screen.torch_mode);

	GLuint torch_anim = glGetUniformLocation(night_program, "torch_anim");
	glUniform1i(torch_anim, screen.torch_anim);
	//gl_has_errors();
	// texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SHADOW_00]
	glActiveTexture(GL_TEXTURE1);
	GLuint shad_00 = glGetUniformLocation(night_program, "shad00");
	glUniform1i(shad_00, 1);
	//glUniform4fv(shad_00, 1, glm::value_ptr(color));
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SHADOW_00]);

	glActiveTexture(GL_TEXTURE2);
	GLuint shad_01 = glGetUniformLocation(night_program, "shad01");
	glUniform1i(shad_01, 2);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SHADOW_01]);

	glActiveTexture(GL_TEXTURE3);
	GLuint dark_00 = glGetUniformLocation(night_program, "dark00");
	glUniform1i(dark_00, 3);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::MIDNIGHT_00]);

	glActiveTexture(GL_TEXTURE4);
	GLuint dark01 = glGetUniformLocation(night_program, "dark01");
	glUniform1i(dark01, 4);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::MIDNIGHT_01]);

	glActiveTexture(GL_TEXTURE5);
	GLuint light_00 = glGetUniformLocation(night_program, "light00");
	glUniform1i(light_00, 5);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TORCHLIGHT_00]);

	glActiveTexture(GL_TEXTURE6);
	GLuint light_01 = glGetUniformLocation(night_program, "light01");
	glUniform1i(light_01, 6);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TORCHLIGHT_01]);

	glActiveTexture(GL_TEXTURE7);
	GLuint light_02 = glGetUniformLocation(night_program, "light02");
	glUniform1i(light_02, 7);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TORCHLIGHT_02]);
	gl_has_errors();

	glActiveTexture(GL_TEXTURE8);
	GLuint helpnight = glGetUniformLocation(night_program, "helpnight");
	glUniform1i(helpnight, 8);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::HELPNIGHT]);

	glActiveTexture(GL_TEXTURE9);
	GLuint helptorch = glGetUniformLocation(night_program, "helptorch");
	glUniform1i(helptorch, 9);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::HELPTORCH]);

	glActiveTexture(GL_TEXTURE10);
	GLuint light_05 = glGetUniformLocation(night_program, "light_05");
	glUniform1i(light_05, 10);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TORCHLIGHT_05]);

	glActiveTexture(GL_TEXTURE11);
	GLuint light_06 = glGetUniformLocation(night_program, "light_06");
	glUniform1i(light_06, 11);
	glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TORCHLIGHT_06]);

	gl_has_errors();
	//glUniform1i(shad_00, 0);

	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}


// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw() {
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.200, 0.900, 0.600 , 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();

    ScreenState &screen = registry.screenStates.components[0];

    Entity player = registry.players.entities[0];
    if (screen.game_screen == game_screen_main_menu) {
        Entity background = registry.daytimes.entities[0];
        drawTexturedStaticMesh(background, projection_2D);

        // render dog
        Entity dog = registry.daytimes.entities[1];
        drawTexturedStaticMesh(dog, projection_2D);

        // render sign last
        Entity logo = registry.daytimes.entities[2];
        drawTexturedStaticMesh(logo, projection_2D);

        // render player
        Motion& player_motion = registry.motions.get(player);
        player_motion.scale = { 2 * 60, 2 * 80 };
        drawTexturedMesh(player, projection_2D);

    }
    else if (screen.game_screen == game_screen_night) {
        // if (!registry.deathTimers.has(player)) {
            // Draw all textured meshes that have a position and size component
            for (Entity entity: registry.renderRequests.entities) {
                if (registry.motions.has(entity)) {
                    if (!registry.debugComponents.has(entity)) {
                        if (!registry.helpScreen.has(entity)) {
                            drawTexturedMesh(entity, projection_2D);
                        }
                    }
                }
                    // for non-moving entities (plants and potions)
                else if (registry.immobiles.has(entity)) {
                    if (!registry.helpScreen.has(entity)) {
                        drawTexturedStaticMesh(entity, projection_2D);
                    }
                }

                // Note, it's not very efficient to access elements indirectly via the entity
                // albeit iterating through all Sprites in sequence. A good point to optimize
            }
            // render inventory lines last?
            for (Entity entity: registry.renderRequests.entities) {
                if (registry.debugComponents.has(entity)) {
                    drawTexturedMesh(entity, projection_2D);
                }
            }

		drawTexturedMesh(player, projection_2D);

		// render help screen
		if (registry.helpScreen.size() > 0) {
			HelpScreen& help = registry.helpScreen.components[0];
			if (help.help == 1) {
				//printf("got here");
				for (Entity entity : registry.renderRequests.entities) {
					if (registry.helpScreen.has(entity)) {
						drawTexturedMesh(entity, projection_2D);
					}
				}
			}
		}

    }
    else if (screen.game_screen == game_screen_day) {
        Entity background = registry.daytimes.entities[0];
        drawTexturedStaticMesh(background, projection_2D);

        // render dog 2nd
        Entity dog = registry.daytimes.entities[1];
        drawTexturedStaticMesh(dog, projection_2D);

        // render danger sign 3rd
        Entity sign2 = registry.daytimes.entities[3];
        drawTexturedStaticMesh(sign2, projection_2D);

        // render player 2nd last
        drawTexturedMesh(player, projection_2D);

        // render instruction sign last
        Entity sign = registry.daytimes.entities[2];
        drawTexturedStaticMesh(sign, projection_2D);

        // render inventory lines
        for (Entity entity: registry.renderRequests.entities) {
            if (registry.motions.has(entity)) {
                if (!registry.immobiles.has(entity)) {
                    if (registry.debugComponents.has(entity)) {
                        drawTexturedMesh(entity, projection_2D);
                    }
                }
            }
        }
        // render UI screen
        for (Entity entity: registry.renderRequests.entities) {
            if (registry.immobiles.has(entity)) {
                if (registry.debugComponents.has(entity)) {
                    drawTexturedStaticMesh(entity, projection_2D);
                }
            }
        }

		if (registry.helpScreen.size() > 0) {
			HelpScreen& help = registry.helpScreen.components[0];
			if (help.help == 1) {
				Entity helpScreen = registry.helpScreen.entities[0];
				drawTexturedStaticMesh(helpScreen, projection_2D);
			}
		}
       
    }
    else if (screen.game_screen == game_screen_game_over) {
        for (Entity entity: registry.renderRequests.entities) {
            if (registry.enemies.has(entity)) {
                Motion& keeper_motion = registry.motions.get(entity);
                keeper_motion.position = { window_width_px / 2, window_height_px / 2 };
                keeper_motion.scale = {200, 400};
                keeper_motion.velocity = {0,0};
                 drawTexturedMesh(entity, projection_2D);
            }
        }
    }
    else if (screen.game_screen == game_screen_win) {
        Entity background = registry.daytimes.entities[0];
        drawTexturedStaticMesh(background, projection_2D);

        for (Entity entity: registry.renderRequests.entities) {
            if (registry.daytimes.has(entity) && registry.motions.has(entity)) {
                drawTexturedMesh(entity, projection_2D);
            }
        }

//		drawTexturedMesh(&win_dance, projection_2D);
    }
    else if (screen.game_screen == cut_scene_0 || screen.game_screen == cut_scene_1 || screen.game_screen == cut_scene_2) {
        Entity background = registry.daytimes.entities[0];
        drawTexturedStaticMesh(background, projection_2D);

        glm::mat4 trans = glm::mat4(1.0f);
//        float timeBetweenLines = 10000.0f; // Adjust this value to control the time between lines
//        float elapsedTime = 0.0f;

        // Render all speech in the registry
        for (Entity entity : registry.speechRenderRequests.entities)
        {
            SpeechRenderRequest request = registry.speechRenderRequests.get(entity);
//            while (elapsedTime <= timeBetweenLines) {
//                elapsedTime += 0.001;
//            }
//            elapsedTime = 0.0f;
            renderSpeechText(request.text, request.position.x, request.position.y, request.scale, request.color, trans);

        }

        for (Entity entity: registry.renderRequests.entities) {
            if (registry.immobiles.has(entity) && registry.healers.has(entity)) {
                drawTexturedStaticMesh(entity, projection_2D);
            }
        }

    }

	glm::mat4 trans = glm::mat4(1.0f);
	// Might need to use rotation and scaling in the future
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 1.0));
	
	// Render text at the bottom left of the screen
	//renderText("Test", 0.0f, 70.0f, 1.0f, glm::vec3(1.0, 1.0, 1.0), trans); // Note if y value is less than 70 the text gets cut off

	// Render all text in the registry
	for (Entity entity : registry.textRenderRequests.entities)
	{
		TextRenderRequest request = registry.textRenderRequests.get(entity);
		renderText(request.text, request.position.x, request.position.y, request.scale, request.color, trans);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix() {
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

//void RenderSystem::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans) {
//
//}