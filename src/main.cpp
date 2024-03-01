
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "map_generator.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main(){
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	MapGen generator(3,3);

	generator.createWorldMap();

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}



	// initialize the main systems
	renderer.init(window);
	

	world.init(&renderer, &generator);

	

	// variable timestep loop
	auto t = Clock::now();

	int frames = 0;

	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.step(elapsed_ms);
		physics.step(elapsed_ms);
		world.handle_collisions();

		renderer.draw();
		frames++;

		//printf("Frames: %i\n", frames);
		//printf("Elapsed ms: %f\n", elapsed_ms);
		///*if (elapsed_ms >= 1000) {
		//	float fps = frames / elapsed_ms * 1000.0f;
		//	printf("Fps: %f\n", fps);
		//	frames = 0;
		//}*/
		//int fps = frames / elapsed_ms * 1000.0;
		//printf("Fps: %i\n", fps);
		//frames = 0;
		
	}

	frames = 0;
	
	return EXIT_SUCCESS;
}
