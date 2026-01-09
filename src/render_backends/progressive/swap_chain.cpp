#include "Engine/render_backends/progressive/swap_chain.h"
#include "Engine/render_backends/progressive/constants.h"
#include "Engine/render_backends/progressive/graphics_pipeline.h"
#include "Engine/render_backends/progressive/virtual_device.h"
#include "Engine/render_backends/progressive/render_pass.h"
#include <SDL2/SDL_vulkan.h>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <array>

SwapChainSupportDetails::SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	vk_surface_capabilities = vk_physical_device.getSurfaceCapabilitiesKHR(vk_surface);
	vk_surface_formats = vk_physical_device.getSurfaceFormatsKHR(vk_surface);
	vk_present_modes = vk_physical_device.getSurfacePresentModesKHR(vk_surface);
}

SwapChain::SwapChain(Logger* logger, ApplicationConfig* application_config, SDL_Window* sdl_window, vk::PhysicalDevice* vk_physical_device, VirtualDevice* device, vk::SurfaceKHR* vk_surface,
	vk::ImageUsageFlags image_usage_bits,
	// Settings:
	vk::PresentModeKHR setting_prefered_present_mode,
	bool setting_stereoscopic
)
: sdl_window(sdl_window), vk_physical_device(vk_physical_device), device(device), vk_surface(vk_surface), application_config(application_config), logger(logger) {
	SwapChainSupportDetails support_details = SwapChainSupportDetails(*vk_physical_device, *vk_surface);

	vk::SurfaceFormatKHR vkSurfaceFormat = choose_surface_format(support_details);
	vk::PresentModeKHR vkPresentMode = choose_present_mode(support_details, setting_prefered_present_mode);
	this->vk_extent = choose_swap_extent(support_details);
	
	// get the image count
	uint32_t imageCount = support_details.vk_surface_capabilities.minImageCount + 1;

	if (support_details.vk_surface_capabilities.maxImageCount > 0 &&
		imageCount > support_details.vk_surface_capabilities.maxImageCount) {
		imageCount = support_details.vk_surface_capabilities.maxImageCount;
	}

	// create info
	vk::SwapchainCreateInfoKHR createInfo;

	// set queue family indices

	QueueFamilyIndices queueFamilyIndicesStruct = QueueFamilyIndices(*this->vk_physical_device, *this->vk_surface);
	std::array<uint32_t, 2> queueFamilyIndices = {
		queueFamilyIndicesStruct.graphics.value(),
		queueFamilyIndicesStruct.present.value()
	};

	// different settings for graphics and present being the same queue vs different:

	if (queueFamilyIndicesStruct.graphics != queueFamilyIndicesStruct.present) { // queues are different
		createInfo = vk::SwapchainCreateInfoKHR(
			{},
			*this->vk_surface,
			imageCount,
			vkSurfaceFormat.format,
			vkSurfaceFormat.colorSpace,
			this->vk_extent,
			setting_stereoscopic ? 2 : 1,
			image_usage_bits,
			vk::SharingMode::eConcurrent,
			queueFamilyIndices,
			support_details.vk_surface_capabilities.currentTransform, // pretransform (might be useful later if we need to cheaply transform images.)
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vkPresentMode,
			vk::True,
			VK_NULL_HANDLE
			// TODO: ^ make ptr to old swap chain incase window is resized or something invalidating or deoptimizing
			// the swapchain and the swapchain needs to be recreated.
		);
	}
	else { // Queues are the same
		createInfo = vk::SwapchainCreateInfoKHR(
			{},
			*this->vk_surface,
			imageCount,
			vkSurfaceFormat.format,
			vkSurfaceFormat.colorSpace,
			this->vk_extent,
			setting_stereoscopic ? 2 : 1,
			image_usage_bits,
			vk::SharingMode::eExclusive,
			nullptr, // queue family indices
			support_details.vk_surface_capabilities.currentTransform, // pretransform (might be useful later if we need to cheaply transform images.)
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vkPresentMode,
			vk::True,
			VK_NULL_HANDLE
			// TODO: ^ make ptr to old swap chain incase window is resized or something invalidating or deoptimizing
			// the swapchain and the swapchain needs to be recreated.
		);
	}

	// create the actual swap chain internally in vulkan:
	this->vk_swapchain = this->device->get_vulkan_device()->createSwapchainKHR(createInfo);

	// Getting the handles to the images in the swap chain:
	this->vk_images = this->device->get_vulkan_device()->getSwapchainImagesKHR(this->vk_swapchain);

	this->vk_image_format = vkSurfaceFormat.format;

	this->create_display_image_views();
	
	if (!this->create_graphics_pipelines()) {
		throw std::runtime_error("Failed to create graphics pipelines.");
	}
}

void SwapChain::create_display_image_views() {
	this->vk_display_image_views.resize(this->vk_images.size());

	for (size_t i = 0; i < this->vk_images.size(); i++) {
		this->vk_display_image_views[i] = this->create_image_view(vk::ImageViewType::e2D, this->vk_images[i]);
	}
}

vk::ImageView SwapChain::create_image_view(vk::ImageViewType type, vk::Image vk_image) {
	vk::ImageSubresourceRange createInfoSubresourceRange;
	createInfoSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	createInfoSubresourceRange.baseMipLevel = 0;
	createInfoSubresourceRange.levelCount = 1;
	createInfoSubresourceRange.baseArrayLayer = 0;
	createInfoSubresourceRange.layerCount = 1;
	vk::ImageViewCreateInfo createInfo = vk::ImageViewCreateInfo(
		{},
		vk_image, // TODO: add logic to select images
		type,
		this->vk_image_format,
		{
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity
		},
		createInfoSubresourceRange
	);
	return this->device->get_vulkan_device()->createImageView(createInfo);
}

vk::ImageView SwapChain::create_image_view(string label, vk::ImageViewType type, vk::Image vk_image) {
	vk::ImageView imageView = this->create_image_view(type, vk_image);
	this->vk_image_view_map.insert(std::make_pair(label, imageView));
	return imageView;
}

vk::ImageView& SwapChain::get_image_view(string label) {
	return this->vk_image_view_map[label];
}

vk::Framebuffer SwapChain::create_framebuffer(std::shared_ptr<RenderPass> render_pass, std::span<const vk::ImageView> attachments, uint32_t layers) {
	vk::FramebufferCreateInfo frameBufferCreateInfo = vk::FramebufferCreateInfo(
		{},
		render_pass->vk_get_render_pass(),
		attachments,
		this->vk_extent.width,
		this->vk_extent.height,
		layers
	);

	return this->device->get_vulkan_device()->createFramebuffer(frameBufferCreateInfo);
}

vk::Framebuffer SwapChain::create_framebuffer(string label, std::shared_ptr<RenderPass> render_pass, std::span<const vk::ImageView> attachments, uint32_t layers) {
	vk::Framebuffer framebuffer = this->create_framebuffer(render_pass, attachments, layers);
	this->vk_framebuffer_map.insert(std::make_pair(label, framebuffer));
	return framebuffer;
}

void SwapChain::create_display_framebuffers(std::shared_ptr<RenderPass> render_pass) {
	this->vk_display_framebuffers.resize(this->vk_display_image_views.size());
	for (size_t i = 0; i < this->vk_display_image_views.size(); i++) {
		std::array<vk::ImageView, 1> attachments = {
			this->vk_display_image_views[i]
		};

		this->vk_display_framebuffers[i] = this->create_framebuffer(render_pass, attachments, 1);
	}
}

vk::Framebuffer& SwapChain::get_framebuffer(string label) {
	return this->vk_framebuffer_map[label];
}

vk::SurfaceFormatKHR SwapChain::choose_surface_format(const SwapChainSupportDetails& support_details) {
	for (const auto& availableFormat : support_details.vk_surface_formats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb
		&& availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
		// Rank scoring might happen here.
	}

	// TODO: rank and pick surface formats based on how "good" they are
	// but for now we will just pick the first one that pops up:
	return support_details.vk_surface_formats[0];
}

vk::PresentModeKHR SwapChain::choose_present_mode(const SwapChainSupportDetails& support_details, vk::PresentModeKHR prefered) {
	for (const auto& availablePresentMode : support_details.vk_present_modes) {
		if (availablePresentMode == prefered) {
			return availablePresentMode;
		}
	}

	// This mode is always available on all hardware so we use it as a default fallback:
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::choose_swap_extent(const SwapChainSupportDetails& support_details) {
	if (support_details.vk_surface_capabilities.currentExtent.width != std::numeric_limits<uint16_t>::max()) {
		return support_details.vk_surface_capabilities.currentExtent;
	} else {
		int width, height;
		SDL_Vulkan_GetDrawableSize(this->sdl_window, &width, &height);

		vk::Extent2D actualExtent = vk::Extent2D(
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		);

		actualExtent.width = std::clamp(actualExtent.width,
			support_details.vk_surface_capabilities.minImageExtent.width,
			support_details.vk_surface_capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(actualExtent.height,
			support_details.vk_surface_capabilities.minImageExtent.height,
			support_details.vk_surface_capabilities.maxImageExtent.height
		);

		return actualExtent;
	}
}

void SwapChain::clean_up() {
	// clean up frame buffers
	for (auto& framebuffer : this->vk_display_framebuffers) {
		this->device->get_vulkan_device()->destroyFramebuffer(framebuffer);
	}

	for (auto& [framebufferName, framebuffer] : this->vk_framebuffer_map) {
		this->device->get_vulkan_device()->destroyFramebuffer(framebuffer);
	}

	// clean up graphics pipelines
	for (const auto& [pipeName, graphicsPipeline] : this->graphics_pipeline_map) {
		graphicsPipeline->clean_up();
	}

	for (const auto& [renderPassName, renderPass] : this->render_pass_map) {
		renderPass->clean_up();
	}

	for (auto& imageView : this->vk_display_image_views) {
		this->device->get_vulkan_device()->destroyImageView(imageView);
	}
	for (auto& [imageViewName, imageView] : this->vk_image_view_map) {
		this->device->get_vulkan_device()->destroyImageView(imageView);
	}
	this->device->get_vulkan_device()->destroySwapchainKHR(this->vk_swapchain);
}

// GRAPHICS PIPELINES

bool SwapChain::create_graphics_pipelines() {
	// TODO: Create a different graphics pipeline for each scenario

	// Build the render pipeline:
	// this renders graphics onto the screen.

	/// Create Render Pass:

	/// Create Sub Pass:

	auto subpass1 = RenderPass::Builder::SubpassBuilder("render", this->logger, this->device);
	subpass1.add_color_attachment_reference(
		0,
		vk::ImageLayout::eColorAttachmentOptimal
	)
	->set_bind_point(vk::PipelineBindPoint::eGraphics);

	this->render_pass_map.insert(std::make_pair(
		"render",
		RenderPass::Builder("render", this->logger, this->device)
		.add_attachment_description(
			{},
			this->vk_image_format,
			vk::SampleCountFlagBits::e1, // TODO : Change this based on user defined multisampling settings.
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore, // Store the image to be shown on the screen
			vk::AttachmentLoadOp::eDontCare, // nostencil
			vk::AttachmentStoreOp::eDontCare, // no stencil
			vk::ImageLayout::eUndefined, // input layout doesnt matter since we are clearing the image anyways
			vk::ImageLayout::ePresentSrcKHR
		)
		->add_sub_pass_builder(subpass1)
		->build()
	));

	/// Create Graphics Pipeline:

	this->graphics_pipeline_map.insert(std::make_pair(
		"render",
		GraphicsPipeline::Builder("render", this->logger, this->device)
		.add_stage(
			"shaders/.testing/vert.spv", // for now we are just using .testing since it is gitignored
			"main",
			vk::ShaderStageFlagBits::eVertex
		)->add_stage(
			"shaders/.testing/frag.spv", // for now we are just using .testing since it is gitignored
			"main",
			vk::ShaderStageFlagBits::eFragment
		)->add_dynamic_state(vk::DynamicState::eViewport)
		->add_dynamic_state(vk::DynamicState::eScissor)
		/*->add_dynamic_state(vk::DynamicState::eDepthBias)
		->add_dynamic_state(vk::DynamicState::eBlendConstants)
		->add_dynamic_state(vk::DynamicState::eStencilReference)
		->add_dynamic_state(vk::DynamicState::eStencilCompareMask)
		->add_dynamic_state(vk::DynamicState::eStencilWriteMask)
		->add_dynamic_state(vk::DynamicState::eCullMode)
		->add_dynamic_state(vk::DynamicState::eFrontFace)
		->add_dynamic_state(vk::DynamicState::eDepthTestEnable)
		->add_dynamic_state(vk::DynamicState::eDepthWriteEnable)
		->add_dynamic_state(vk::DynamicState::eDepthCompareOp)*/
		->set_primitive_topology(vk::PrimitiveTopology::eTriangleList)
		->set_primitive_restart(false)
		->set_viewport_count(1)
		->set_scissor_count(1)
		->set_multisampling(false, vk::SampleCountFlagBits::e1)
		->add_color_blend_attachment()
		->set_subpass(this->render_pass_map["render"], 0)// subpass 0
		->build()
	));

	return true;
}