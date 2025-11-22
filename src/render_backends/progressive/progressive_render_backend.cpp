#include "Engine/render_backends/progressive/progressive_render_backend.h"

// ==== Class Functions ====

ProgressiveRenderBackend::ProgressiveRenderBackend(Uint32 sdl_only_window_flags) {
	// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`
	this->sdl_window_flags = SDL_WINDOW_VULKAN | sdl_only_window_flags;
}

// === Game Loop Hooks ===

void ProgressiveRenderBackend::before_start_window(string window_title, int window_width, int window_height) {
	// This is before SDL is initialized
}

void ProgressiveRenderBackend::before_game_loop() {
	// This is where vulkan is initialized.
}

void ProgressiveRenderBackend::after_game_loop() {
	// This is where vulkan is de-initialized.
}

void ProgressiveRenderBackend::update_game() {
	// This is where the screen is updated with the vulkan surface.
}