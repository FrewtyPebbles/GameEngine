#pragma once

#include "Engine/render_backends/render_backend.h"
#include <vulkan/vulkan.hpp>
#include <map>

using std::map;

const vector<const char*> vkVALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};


#ifdef NDEBUG
	const bool vkENABLE_VALIDATION_LAYERS = false;
#else
	const bool vkENABLE_VALIDATION_LAYERS = true;
#endif


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
		Logger* logger,
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


	

	// = Initialization / De-Initialization =
	// ---

	bool initialize_vulkan();

	bool vk_create_instance();
	/// returns false on failure

	vector<const char*> vk_get_required_extensions();

	bool vk_pick_physical_device();

	//Checks if physical device has our required vulkan extensions:
	bool vk_check_physical_device_is_suitable(vk::PhysicalDevice device);

	uint64_t vk_measure_physical_device_suitability(vk::PhysicalDevice physicalDevice);

	vk::PhysicalDevice & vk_get_physical_device();

	//this also creates the queue
	bool vk_create_virtual_device();

	bool vk_cleanup();




	// = Validation = 
	// ---

	bool vk_check_validation_layer_support();
	/// checks that all of the validation layers are supported

	vk::DebugUtilsMessengerCreateInfoEXT vk_create_debug_messenger_create_info();

	bool vk_setup_debug_messenger();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL vk_handle_debug_messages(
		vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
		vk::DebugUtilsMessageTypeFlagsEXT message_type,
		const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* logger_void_pointer // The logger instance is passed here.
	);
	/// This is a vulkan callback function for handling validation layer / debug messages

	void vk_bind_to_window(SDL_Window*);

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === VULKAN ===

	// required vulkan attributes
	vk::Instance vk_instance;
	vk::SurfaceKHR kv_surface;
	// add support for multiple different physical devices.
	// that way later we can support rendering across multiple devices at once or switching devices.
	// the keys of this map are the physical device ids
	map<uint32_t, vk::PhysicalDevice> vk_physical_device_map;
	// key: priority value, value: physical device id
	std::multimap<uint64_t, uint32_t> vk_physical_device_priority_map;

	size_t vk_physical_device_id;
	vk::Device vk_device;
	vk::Queue vk_queue;
	vk::DebugUtilsMessengerEXT vk_debug_messenger;

};

