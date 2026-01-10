#include "Engine/render_backends/progressive/command_pool.h"
#include "Engine/render_backends/progressive/virtual_device.h"

CommandPool::CommandPool(
	vk::CommandPoolCreateFlags create_flags,
	uint32_t queue_family_index,
	VirtualDevice* virtual_device
) : virtual_device(virtual_device) {
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo(
		create_flags,
		queue_family_index
	);

	this->vk_command_pool = virtual_device->get_vulkan_device()->createCommandPool(commandPoolCreateInfo);
}

void CommandPool::clean_up() {
	this->virtual_device->get_vulkan_device()->destroyCommandPool(this->vk_command_pool);
}

void CommandPool::create_command_buffers(std::span<string> buffer_names, vk::CommandBufferLevel level) {
	vk::CommandBufferAllocateInfo commandBufferAllocInfo = vk::CommandBufferAllocateInfo(
		this->vk_command_pool, level, buffer_names.size()
	);

	this->vk_command_buffers = this->virtual_device->get_vulkan_device()->allocateCommandBuffers(commandBufferAllocInfo);
	for (size_t i = 0; i < this->vk_command_buffers.size(); i++) {
		vk_command_buffer_map.insert(std::make_pair(buffer_names[i], this->vk_command_buffers[i]));
	}
}

vk::CommandBuffer& CommandPool::get_command_buffer(string buffer_name) {
	return this->vk_command_buffer_map[buffer_name];
}

void CommandPool::start_recording_command_buffer(string buffer_name, vk::CommandBufferUsageFlags usage_flags, vk::CommandBufferInheritanceInfo* inheritance_info) {
	vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo(
		usage_flags, inheritance_info
	);

	this->vk_command_buffer_map[buffer_name].begin(commandBufferBeginInfo);
}

void CommandPool::stop_recording_command_buffer(string buffer_name) {
	this->vk_command_buffer_map[buffer_name].end();
}