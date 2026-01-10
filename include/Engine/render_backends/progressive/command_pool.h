#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <span>

using std::string;
using std::unordered_map;
using std::vector;

class VirtualDevice;

// TODO : Create a multithreaded "RenderJob" class which is linked to a thread and
// has its own set of command pools for all the queue family indexes.

class CommandPool {
public:
	CommandPool(
		vk::CommandPoolCreateFlags create_flags,
		uint32_t queue_family_index,
		VirtualDevice* virtual_device
	);

	void clean_up();
	
	void create_command_buffers(std::span<string> buffer_names, vk::CommandBufferLevel level);

	vk::CommandBuffer& get_command_buffer(string buffer_name);

	void start_recording_command_buffer(string buffer_name, vk::CommandBufferUsageFlags usage_flags = {}, vk::CommandBufferInheritanceInfo* inheritance_info = nullptr);

	void stop_recording_command_buffer(string buffer_name);

private:

	VirtualDevice* virtual_device;
	vk::CommandPool vk_command_pool;

	unordered_map<string, vk::CommandBuffer> vk_command_buffer_map;
	vector<vk::CommandBuffer> vk_command_buffers;
};