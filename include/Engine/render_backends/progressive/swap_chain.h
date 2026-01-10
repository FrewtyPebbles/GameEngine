#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>
#include <span>

using std::vector;
using std::unordered_map;
using std::string;

namespace Tritium {
	class Engine;
};

class VirtualDevice;
class GraphicsPipeline;
class RenderPass;

struct SwapChainSupportDetails {
	SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	vk::SurfaceCapabilitiesKHR vk_surface_capabilities;
	vector<vk::SurfaceFormatKHR> vk_surface_formats;
	vector<vk::PresentModeKHR> vk_present_modes;
};

class SwapChain {
public:
	SwapChain(Tritium::Engine* engine, SDL_Window* sdl_window, vk::PhysicalDevice* vk_physical_device, VirtualDevice * device, vk::SurfaceKHR* vk_surface,
		vk::ImageUsageFlags image_usage_bits,
		// This should be optionally user supplied:
		vk::PresentModeKHR setting_prefered_present_mode = vk::PresentModeKHR::eMailbox,
		bool setting_stereoscopic = false
	);

	void clean_up();

	// IMAGE AND FRAME BUFFERS

	// TODO : Create an abstraction that lets you create a single "image" to draw to then make these functions private.
	// For now for testing purposes these will be public.

	// Creates an image view dynamically for a sampler2D or sampler2DArray or something.
	vk::ImageView create_image_view(string label, vk::ImageViewType type, vk::Image vk_image);

	vk::ImageView& get_image_view(string label);

	vk::Framebuffer create_framebuffer(string label, std::shared_ptr<RenderPass> render_pass, std::span<const vk::ImageView> attachments, uint32_t layers);

	vk::Framebuffer& get_framebuffer(string label);

	Tritium::Engine* engine;

	vk::Format vk_image_format;
	vk::Extent2D vk_extent;

private:

	// FUNCTIONS

	vk::SurfaceFormatKHR choose_surface_format(const SwapChainSupportDetails& support_details);

	vk::PresentModeKHR choose_present_mode(const SwapChainSupportDetails& support_details, vk::PresentModeKHR prefered = vk::PresentModeKHR::eMailbox);

	vk::Extent2D choose_swap_extent(const SwapChainSupportDetails& support_details);

	void create_display_image_views();

	vk::ImageView create_image_view(vk::ImageViewType type, vk::Image vk_image);

	vk::Framebuffer create_framebuffer(std::shared_ptr<RenderPass> render_pass, std::span<const vk::ImageView> attachments, uint32_t layers);

	void create_display_framebuffers(std::shared_ptr<RenderPass> render_pass);


	bool create_graphics_pipelines();
	/// Creates the different graphics pipelines for different things such as depth buffering
	/// or lighting

	// ATTRIBUTES

	SDL_Window* sdl_window;

	vk::PhysicalDevice* vk_physical_device;
	VirtualDevice* device;
	vk::SurfaceKHR* vk_surface;

	vk::SwapchainKHR vk_swapchain;

	unordered_map<string, vk::ImageView> vk_image_view_map;
	vector<vk::ImageView> vk_display_image_views;
	vector<vk::Image> vk_images;

	vector<vk::Framebuffer> vk_display_framebuffers;
	unordered_map<string, vk::Framebuffer> vk_framebuffer_map;

	// GRAPHICS PIPELINE

	unordered_map<string, std::shared_ptr<GraphicsPipeline>> graphics_pipeline_map;
	unordered_map<string, std::shared_ptr<RenderPass>> render_pass_map;
};
