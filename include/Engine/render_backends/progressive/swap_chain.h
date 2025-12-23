#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

using std::vector;

struct SwapChainSupportDetails {
	SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	vk::SurfaceCapabilitiesKHR vk_surface_capabilities;
	vector<vk::SurfaceFormatKHR> vk_surface_formats;
	vector<vk::PresentModeKHR> vk_present_modes;
};

class SwapChain {
public:
	SwapChain(vk::PhysicalDevice* vk_physical_device, vk::SurfaceKHR* vk_surface,
		// This should be optionally user supplied:
		vk::PresentModeKHR prefered_present_mode = vk::PresentModeKHR::eMailbox
	);

private:

	// FUNCTIONS

	void choose_surface_format();

	void choose_present_mode(vk::PresentModeKHR prefered = vk::PresentModeKHR::eMailbox);

	void choose_swap_extent();

	// ATTRIBUTES

	vk::PhysicalDevice* vk_physical_device;
	vk::SurfaceKHR* vk_surface;

	vk::SurfaceFormatKHR vk_surface_format;
	vk::PresentModeKHR vk_present_mode;

	SwapChainSupportDetails support_details;
};
