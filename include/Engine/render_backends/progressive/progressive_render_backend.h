#pragma once

#include "Engine/render_backends/render_backend.h"

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

	ProgressiveRenderBackend(Uint32 sdl_only_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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

	bool create_device();

	// = Base Functions =
	// ---
	bool pick_physical_device();
	void bind_to_window(SDL_Window*);

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === VULKAN ===

	// required vulkan attributes
	vk::Instance vk_instance = vk::Instance(nullptr);
	vk::SurfaceKHR kv_surface = vk::SurfaceKHR(nullptr);
	vk::PhysicalDevice vk_physical_device = vk::PhysicalDevice(nullptr);
	vk::Device vk_device = vk::Device(nullptr);

};
