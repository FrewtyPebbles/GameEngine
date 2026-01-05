#pragma once

#include "Engine/render_backends/progressive/virtual_device.h"
#include "Engine/logging/logger.h"
#include "Engine/render_backends/progressive/render_pass.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <memory>
#include <deque>

using std::vector;
using std::string;
using std::deque;

// === NOTES ===

// Shaders will be all compiled when the game is built.

// TODO : Add depth and stencil testing

class GraphicsPipeline;

// We use a builder to make graphics pipeline creation more modular,
// but also to clean up unnessecary variables after creation.
class GraphicsPipelineBuilder {
public:
	// Creates the graphics pipeline builder
	GraphicsPipelineBuilder(const string& name, Logger* logger, VirtualDevice* device);

	// BUILDER FUNCTIONS
	// These are used to build the pipeline

	// TODO ADD Depth stencil state.

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

	GraphicsPipelineBuilder* set_line_width(float vk_request_line_width);

	GraphicsPipelineBuilder* set_cull_mode(vk::CullModeFlags vk_cull_mode);

	GraphicsPipelineBuilder* set_front_face(vk::FrontFace vk_front_face);

	GraphicsPipelineBuilder* set_depth_bias(
		bool vk_depth_bias_enable,
		float vk_depth_bias_constant_factor = 0.0f,
		float vk_depth_bias_clamp = 0.0f,
		float vk_depth_bias_slope_factor = 0.0f
	);

	// multisampling

	GraphicsPipelineBuilder* set_multisampling(
		bool vk_sample_shading_enable = false,
		vk::SampleCountFlagBits vk_rasterization_samples = vk::SampleCountFlagBits::e1,
		float vk_min_sample_shading = 1.0f,
		vk::SampleMask* vk_p_sample_mask = nullptr,
		bool vk_alpha_to_coverage_enable = false,
		bool vk_alpha_to_one_enable = false
	);

	// depth testing

	GraphicsPipelineBuilder* set_depth_test_enable(bool value);

	GraphicsPipelineBuilder* set_depth_write_enable(bool value);
	
	GraphicsPipelineBuilder* set_depth_compare_op(vk::CompareOp value);

	GraphicsPipelineBuilder* set_depth_bounds_test_enable(bool value);

	// stencil testing

	GraphicsPipelineBuilder* set_stencil_test_enable(bool value);

	GraphicsPipelineBuilder* set_stencil_op_state_front(vk::StencilOpState value);

	GraphicsPipelineBuilder* set_stencil_op_state_back(vk::StencilOpState value);

	GraphicsPipelineBuilder* set_stencil_min_depth_bounds(float value);

	GraphicsPipelineBuilder* set_stencil_max_depth_bounds(float value);

	// tessellation

	GraphicsPipelineBuilder* set_tessellation_vertices_per_patch(uint32_t count);

	// color blending

	// default is alpha blending
	GraphicsPipelineBuilder* add_color_blend_attachment(
		vk::ColorComponentFlags vk_color_write_mask =
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
		bool vk_blend_enable = true,
		vk::BlendFactor vk_src_color_blend_factor = vk::BlendFactor::eSrcAlpha,
		vk::BlendFactor vk_dst_color_blend_factor = vk::BlendFactor::eOneMinusSrcAlpha,
		vk::BlendOp vk_color_blend_op = vk::BlendOp::eAdd,
		vk::BlendFactor vk_src_alpha_blend_factor = vk::BlendFactor::eOne,
		vk::BlendFactor vk_dst_alpha_blend_factor = vk::BlendFactor::eZero,
		vk::BlendOp vk_alpha_blend_op = vk::BlendOp::eAdd
	);

	GraphicsPipelineBuilder* set_color_blend_logical_op(
		bool enabled = false,
		vk::LogicOp op = vk::LogicOp::eCopy
	);

	GraphicsPipelineBuilder* set_color_blend_constants(
		float r = 0.0f,
		float g = 0.0f,
		float b = 0.0f,
		float a = 0.0f
	);

	// derivative pipelines

	GraphicsPipelineBuilder* set_base_pipeline(std::shared_ptr<GraphicsPipeline> base_pipeline);

	// render pass

	GraphicsPipelineBuilder* set_subpass(std::shared_ptr<RenderPass> render_pass, uint32_t vk_subpass_index);

	// this builds out the final pipeline
	std::shared_ptr<GraphicsPipeline> build();

	// this is called after build
	void clean_up();

protected:

	// TODO: Make builder compile the shader from the file and parse/find any
	// uniforms to register.
	static vector<char> read_binary(const string& filename);

private:


	vk::ShaderModule create_shader_module(const vector<char>& spir_v);

	string name;

	Logger* logger;

	VirtualDevice* device;

	// Pipeline create info

	vector<vk::PipelineShaderStageCreateInfo> shader_stages;
	vector<vk::ShaderModule> shader_modules;
	// we use a deque because vector is not guaranteed to store its memory in the same address on resize/reallocation.
	deque<string> shader_entry_points;

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

	bool vk_depth_test_enable = true;

	bool vk_depth_write_enable = false;

	vk::CompareOp vk_depth_compare_op = vk::CompareOp::eNever;

	bool vk_depth_bounds_test_enable = true;


	// stencil test

	bool vk_stencil_test_enable = true;

	vk::StencilOpState vk_stencil_op_state_front = {};

	vk::StencilOpState vk_stencil_op_state_back = {};

	float vk_stencil_min_depth_bounds = {};

	float vk_stencil_max_depth_bounds = {};

	// tessellation

	uint32_t vk_tessellation_vertices_per_patch;

	// Color Blending

	vector<vk::PipelineColorBlendAttachmentState> vk_color_blend_attachments;

	bool vk_color_blend_logical_op_enable = false;

	vk::LogicOp vk_color_blend_logical_op = vk::LogicOp::eCopy;

	float vk_color_blend_constants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Pipeline Layout
	// TODO : Create api to add uniforms and push constants.
	vector<vk::DescriptorSetLayout> vk_descriptor_set_layouts;

	vector<vk::PushConstantRange> vk_push_constant_ranges;

	// derivative pipelines

	std::shared_ptr<GraphicsPipeline> base_graphics_pipeline = nullptr;

	// Render pass

	std::shared_ptr<RenderPass> render_pass;

	uint32_t vk_subpass_index;
};


class GraphicsPipeline {
public:
	using Builder = GraphicsPipelineBuilder;

	GraphicsPipeline(Logger* logger, VirtualDevice* device, vk::Pipeline vk_pipeline, vk::PipelineLayout vk_pipeline_layout);

	void clean_up();

	vk::Pipeline vk_get_pipeline();

private:

	Logger* logger;
	VirtualDevice* device;

	vk::Pipeline vk_pipeline;
	vk::PipelineLayout vk_pipeline_layout;
};