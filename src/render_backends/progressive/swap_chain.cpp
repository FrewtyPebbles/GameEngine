#include "Engine/render_backends/progressive/swap_chain.h"
#include "Engine/render_backends/progressive/constants.h"

SwapChainSupportDetails::SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	vk_surface_capabilities = vk_physical_device.getSurfaceCapabilitiesKHR(vk_surface);
	vk_surface_formats = vk_physical_device.getSurfaceFormatsKHR(vk_surface);
	vk_present_modes = vk_physical_device.getSurfacePresentModesKHR(vk_surface);
}

SwapChain::SwapChain(vk::PhysicalDevice* vk_physical_device, vk::SurfaceKHR* vk_surface,
	vk::PresentModeKHR prefered_present_mode)
: vk_physical_device(vk_physical_device), vk_surface(vk_surface), support_details(*vk_physical_device, *vk_surface) {
	choose_surface_format();
	choose_present_mode(prefered_present_mode);
	choose_swap_extent();
}

void SwapChain::choose_surface_format() {
	for (const auto& availableFormat : this->support_details.vk_surface_formats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb
		&& availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			this->vk_surface_format = availableFormat;
			return;
		}
		// Rank scoring might happen here.
	}

	// TODO: rank and pick surface formats based on how "good" they are
	// but for now we will just pick the first one that pops up:
	this->vk_surface_format = this->support_details.vk_surface_formats[0];
}

void SwapChain::choose_present_mode(vk::PresentModeKHR prefered) {
	for (const auto& availablePresentMode : this->support_details.vk_present_modes) {
		if (availablePresentMode == prefered) {
			this->vk_present_mode = availablePresentMode;
			return;
		}
	}

	// This mode is always available on all hardware so we use it as a default fallback:
	this->vk_present_mode = vk::PresentModeKHR::eFifo;
}

void SwapChain::choose_swap_extent() {

}

