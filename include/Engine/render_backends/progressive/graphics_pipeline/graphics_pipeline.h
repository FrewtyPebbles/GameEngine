#pragma once

#include "Engine/render_backends/progressive/virtual_device.h"
#include "Engine/logging/logger.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <memory>
#include <string>

using std::vector;
using std::string;

// === NOTES ===

// Shaders will be all compiled when the game is built.

// TODO : Add depth and stencil testing

class GraphicsPipeline;

// We use a builder to make graphics pipeline creation more modular,
// but also to clean up unnessecary variables after creation.
class GraphicsPipelineBuilder {
public:
	// Creates the graphics pipeline builder
	GraphicsPipelineBuilder(const string& name, Logger* logger, std::shared_ptr<VirtualDevice> device);

	// BUILDER FUNCTIONS
	// These are used to build the pipeline
	
	GraphicsPipelineBuilder* add_vertex_input_binding(uint32_t binding_index, size_t stride, vk::VertexInputRate input_rate);

	GraphicsPipelineBuilder* add_vertex_input_attribute(uint32_t binding_index, uint32_t location, vk::Format format, uint32_t offset);

	GraphicsPipelineBuilder* add_stage(
		string shader_path,
		string entry_point,
		vk::ShaderStageFlagBits stage
	);

	GraphicsPipelineBuilder* add_dynamic_state(vk::DynamicState dynamic_state);

	// input assembly

	GraphicsPipelineBuilder* set_primitive_topology(vk::PrimitiveTopology vk_primitive_topology);

	GraphicsPipelineBuilder* set_primitive_restart(bool vk_primitive_restart);

	// viewport
	
	GraphicsPipelineBuilder* set_viewport_count(uint32_t vk_viewport_count);

	GraphicsPipelineBuilder* set_scissor_count(uint32_t vk_scissor_count);

	// rasterizer

	GraphicsPipelineBuilder* set_depth_clamp_enable(bool vk_depth_clamp_enable);

	GraphicsPipelineBuilder* set_rasterizer_discard_enable(bool vk_rasterizer_discard_enable);

	GraphicsPipelineBuilder* set_polygon_mode(vk::PolygonMode vk_polygon_mode);

	GraphicsPipelineBuilder* set_line_width(const float& vk_request_line_width);

	GraphicsPipelineBuilder* set_cull_mode(vk::CullModeFlags vk_cull_mode);

	GraphicsPipelineBuilder* set_front_face(vk::FrontFace vk_front_face);

	GraphicsPipelineBuilder* set_depth_bias(
		bool vk_depth_bias_enable,
		float vk_depth_bias_constant_factor = 0.0f,
		float vk_depth_bias_clamp = 0.0f,
		float vk_depth_bias_slope_factor = 0.0f
	);

	// multisampling

	// TODO : add querying device support for multisampling
	GraphicsPipelineBuilder* set_multisampling(
		bool vk_sample_shading_enable = false,
		vk::SampleCountFlagBits vk_rasterization_samples = vk::SampleCountFlagBits::e1,
		float vk_min_sample_shading = 1.0f,
		vk::SampleMask* vk_p_sample_mask = nullptr,
		bool vk_alpha_to_coverage_enable = false,
		bool vk_alpha_to_one_enable = false
	);

	// this builds out the final pipeline
	std::shared_ptr<GraphicsPipeline> build();

protected:

	static vector<char> read_binary(const string& filename);

private:


	vk::ShaderModule create_shader_module(const vector<char>& spir_v);

	string name;

	Logger* logger;

	std::shared_ptr<VirtualDevice> device;

	// Pipeline create info

	vector<vk::PipelineShaderStageCreateInfo> stages;

	vector<vk::DynamicState> dynamic_states;

	
	vector<vk::VertexInputBindingDescription> vertex_input_bindings;
	
	vector<vk::VertexInputAttributeDescription> vertex_input_attributes;
	
	// input assembly

	vk::PrimitiveTopology vk_primitive_topology = vk::PrimitiveTopology::eTriangleList;

	bool vk_primitive_restart = false;

	// viewport

	uint32_t vk_viewport_count = 1;

	uint32_t vk_scissor_count = 1;

	// rasterizer

	// These defaults may change...

	bool vk_depth_clamp_enable = false;

	bool vk_rasterizer_discard_enable = false;

	vk::PolygonMode vk_polygon_mode = vk::PolygonMode::eFill;

	float vk_line_width = 1.0f;

	vk::CullModeFlags vk_cull_mode = vk::CullModeFlagBits::eBack;

	vk::FrontFace vk_front_face = vk::FrontFace::eClockwise;


	bool vk_depth_bias_enable = false;

	float vk_depth_bias_constant_factor = 0.0f;

	float vk_depth_bias_clamp = 0.0f;

	float vk_depth_bias_slope_factor = 0.0f;

	// multisampling

	// These defaults may change...

	bool vk_sample_shading_enable = false;

	vk::SampleCountFlagBits vk_multisampling_rasterization_samples = vk::SampleCountFlagBits::e1;

	float vk_min_sample_shading = 1.0f;

	vk::SampleMask* vk_p_sample_mask = nullptr;

	bool vk_alpha_to_coverage_enable = false;

	bool vk_alpha_to_one_enable = false;

	// depth test

	bool vk_depth_test_enabled = true;

	// stencil test

	bool vk_stencil_test_enabled = true;
};


class GraphicsPipeline {
public:
	using Builder = GraphicsPipelineBuilder;

	GraphicsPipeline() = default;

};