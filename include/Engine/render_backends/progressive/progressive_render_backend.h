#pragma once

#include "Engine/render_backends/render_backend.h"
#include <vulkan/vulkan.hpp>

// --- ProgressiveRenderBackend --- 
// This is the most cutting edge rendering backend in the engine.
// It has the newest features and the worst cross platform support.
// It supports Windows, Linux, and maybe Android.
// --------------------------------

class ProgressiveRenderBackend : public RenderBackend {

public:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// ==== Class Functions ====
// These include things like constructors, destructors and operators.
// ---

	ProgressiveRenderBackend(
		string application_name,
		string application_description,
		vector<string> application_authors,
		int application_version_major,
		int application_version_minor,
		int application_version_patch,
		string application_version_identifier,
		Uint32 sdl_only_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
	);
	/// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`

//////////////////////
///// ATTRIBUTES /////
//////////////////////

private:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// === Game Loop Functions ===

	void before_start_window(string window_title, int window_width, int window_height) override;

	void before_game_loop() override;

	void after_game_loop() override;

	void update_game() override;

// ==== Render Functions ====
// Overload these functions depending on the render engine	
// ---

	bool vk_create_instance();
	/// returns false on failure

	bool vk_cleanup();

	void initialize_vulkan();

	bool vk_create_device();

	// = Base Functions =
	// ---
	bool vk_pick_physical_device();
	void vk_bind_to_window(SDL_Window*);

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === VULKAN ===

	// required vulkan attributes
	vk::Instance vk_instance;
	vk::SurfaceKHR kv_surface;
	vk::PhysicalDevice vk_physical_device;
	vk::Device vk_device;

};
