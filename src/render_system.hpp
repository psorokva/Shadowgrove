#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// fonts
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**The following arrays store the assets the game will use. They are loaded at initialization and are assumed to not be modified by the render loop.
	 * Whenever possible, add to these lists instead of creating dynamic state it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths = {
		  // std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::CHICKEN, mesh_path("chicken.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("player_01.png"),
			textures_path("player_02.png"),
			textures_path("player_03.png"),
			textures_path("player_04.png"),
			textures_path("player_05.png"),
			textures_path("keeper_01.png"),
			textures_path("keeper_02.png"),
			textures_path("dog_00.png"),
			textures_path("ingredient_00.png"),
			textures_path("ingredient_01.png"),
			textures_path("ingredient_02.png"),
			textures_path("ingredient_03.png"),
			textures_path("ingredient_04.png"),
			textures_path("ingredient_05.png"),
			textures_path("ingredient_06.png"),
			textures_path("ingredient_07.png"),
			textures_path("potion_00.png"),
			textures_path("potion_01.png"),
			textures_path("potion_02.png"),
			textures_path("potion_03.png"),
			textures_path("potion_04.png"),
			textures_path("potion_05.png"),
			textures_path("bush_00.png"),
			textures_path("bush_01.png"),
			textures_path("bush_02.png"),
			textures_path("bush_03.png"),
			textures_path("bush_04.png"),
			textures_path("bush_05.png"),
			textures_path("bush_06.png"),
			textures_path("bush_07.png"),
			textures_path("bush_08.png"),
			textures_path("bush_09.png"),
			textures_path("bush_10.png"),
			textures_path("bush_11.png"),
			textures_path("bush_12.png"),
			textures_path("bush_13.png"),
			textures_path("bush_14.png"),
			textures_path("bush_15.png"),
			textures_path("bush_16.png"),
			textures_path("bush_17.png"),
			textures_path("bush_18.png"),
			textures_path("bush_19.png"),
			textures_path("bush_20.png"),
			textures_path("bush_21.png"),
			textures_path("bush_22.png"),
			textures_path("bush_23.png"),
			textures_path("bush_24.png"),
			textures_path("bush_25.png"),
			textures_path("bush_26.png"),
			textures_path("bush_27.png"),
			textures_path("bush_28.png"),
			textures_path("bush_29.png"),
			textures_path("bush_30.png"),
			textures_path("bush_31.png"),
			textures_path("bush_32.png"),
			textures_path("bush_33.png"),
			textures_path("bush_34.png"),
			textures_path("bush_35.png"),
			textures_path("bush_36.png"),
			textures_path("bush_37.png"),
			textures_path("bush_38.png"),
			textures_path("bush_39.png"),
			textures_path("bush_40.png"),
			textures_path("mush_00.png"),
			textures_path("mush_01.png"),
			textures_path("mush_02.png"),
			textures_path("mush_03.png"),
			textures_path("mush_04.png"),
			textures_path("mush_05.png"),
			textures_path("mush_06.png"),
			textures_path("mush_07.png"),
			textures_path("mush_08.png"),
			textures_path("mush_09.png"),
			textures_path("mush_10.png"),
			textures_path("mush_11.png"),
			textures_path("mush_12.png"),
			textures_path("mush_13.png"),
			textures_path("mush_14.png"),
			textures_path("mush_15.png"),
			textures_path("mush_16.png"),
			textures_path("mush_17.png"),
			textures_path("mush_18.png"),
			textures_path("mush_19.png"),
			textures_path("mush_20.png"),
			textures_path("mush_21.png"),
			textures_path("plant_00.png"),
			textures_path("plant_01.png"),
			textures_path("plant_02.png"),
			textures_path("plant_03.png"),
			textures_path("plant_04.png"),
			textures_path("plant_05.png"),
			textures_path("plant_06.png"),
			textures_path("plant_07.png"),
			textures_path("plant_08.png"),
			textures_path("plant_09.png"),
			textures_path("plant_10.png"),
			textures_path("plant_11.png"),
			textures_path("plant_12.png"),
			textures_path("plant_13.png"),
			textures_path("plant_14.png"),
			textures_path("plant_15.png"),
			textures_path("plant_16.png"),
			textures_path("plant_17.png"),
			textures_path("plant_18.png"),
			textures_path("plant_19.png"),
			textures_path("plant_20.png"),
			textures_path("plant_21.png"),
			textures_path("plant_22.png"),
			textures_path("plant_23.png"),
			textures_path("plant_24.png"),
			textures_path("plant_25.png"),
			textures_path("plant_26.png"),
			textures_path("plant_27.png"),
			textures_path("plant_28.png"),
			textures_path("plant_29.png"),
			textures_path("plant_30.png"),
			textures_path("plant_31.png"),
			textures_path("face00.png"),
			textures_path("face01.png"),
			textures_path("face02.png"),
			textures_path("dogface00.png"),
			textures_path("dogface01.png"),
			textures_path("text_bubble_brown_big.png"),
			textures_path("text_bubble_brown_small.png"),
			textures_path("daytimescreen.png"),
			textures_path("endscreen.png"),
			textures_path("introscreen.png"),
            textures_path("helpscreen.png"),
            textures_path("sign.png"),
            textures_path("sign2.png"),
			textures_path("Torchlight_00.png"),
			textures_path("Torchlight_01.png"),
			textures_path("Torchlight_03.png"),
			textures_path("Torchlight_07_dim.png"),
			textures_path("Torchlight_08_dim.png"),
			textures_path("ShadowOverlay_00.png"),
			textures_path("ShadowOverlay_01.png"),
			textures_path("MidnightOverlay_00.png"),
			textures_path("MidnightOverlay_01.png"),
			textures_path("CRAFTING2.png"),
			textures_path("NightHelpOverlay.png"),
			textures_path("NightHelpTorchlight.png"),
            textures_path("logo_00.png"),
            textures_path("logo_01.png"),
			textures_path("win0.png"),
			textures_path("win1.png"),
			textures_path("f1CHECKO.png"),
			textures_path("spark_00.png"),
			textures_path("spark_01.png"),
			textures_path("spark_02.png"),
			textures_path("spark_03.png"),
			textures_path("spark_04.png"),
			textures_path("spark_05.png"),
			textures_path("spark_06.png"),
			textures_path("spark_07.png"),
			textures_path("torch_03.png"),
			textures_path("torch_04.png"),
			textures_path("torch_05.png"),
			textures_path("torch_00.png"),
			textures_path("torch_01.png"),
			textures_path("torch_02.png"),

	}; //textures_path("bush_06.png"), etc... TODO: convert to tilesets and save on file loading

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("inventory_line"),
		shader_path("textured"),
		shader_path("night") 
		};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Initialize fonts to output text to the screen
	bool initializeFonts(unsigned int font_default_size);

	// Print text to the screen
	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans);
    void renderSpeechText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans);

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	void setDance(Entity* dance) {
		win_dance = dance;
	}

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawTexturedStaticMesh(Entity entity, const mat3& projection);
	void drawTexturedUIMesh(Entity entity, const mat3& projection);
	void drawToScreen();
	Entity * win_dance;
	// Window handle
	GLFWwindow* window;

	// Use before EnableVertexAttribArray to avoid GL errors
	GLuint vao;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;

};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
