#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

namespace Tritium {
	class Engine;
};

class GraphicsPipeline;
class VirtualDevice;

class RenderPass;

class RenderPassBuilder {
public:
	// SUBPASS CLASS

	class SubpassBuilder {
	public:
		friend class RenderPassBuilder;
		SubpassBuilder(const string& name, Tritium::Engine* engine, VirtualDevice* device);
		
		// attachments

		SubpassBuilder* add_input_attachment_reference(
			uint32_t attachment_index,
			vk::ImageLayout layout
		);

		SubpassBuilder* add_color_attachment_reference(
			uint32_t attachment_index,
			vk::ImageLayout layout
		);

		SubpassBuilder* add_resolve_attachment_reference(
			uint32_t attachment_index,
			vk::ImageLayout layout
		);

		SubpassBuilder* set_depth_stencil_attachment_reference(
			uint32_t attachment_index,
			vk::ImageLayout layout
		);

		SubpassBuilder* preserve_attachment(uint32_t index);

		// dependencies
		
		SubpassBuilder* add_dependency(
			SubpassBuilder* src_subpass_builder,
			SubpassBuilder* dst_subpass_builder,
			vk::PipelineStageFlags      src_stage_mask,
			vk::PipelineStageFlags      dst_stage_mask,
			vk::AccessFlags             src_access_mask,
			vk::AccessFlags             dst_access_mask,
			vk::DependencyFlags         dependency_flags = {}
		);

		// subpass description

		SubpassBuilder* set_bind_point(vk::PipelineBindPoint vk_pipeline_bind_point);
		
		// TODO: add other attribute setters

	private:

		// This is called by the render pass builder
		vk::SubpassDescription build();

		string name;
		Tritium::Engine* engine;
		VirtualDevice* device;

		// attachments
		
		vector<vk::AttachmentReference> input_attachment_references;

		vector<vk::AttachmentReference> color_attachment_references;
		
		vector<vk::AttachmentReference> resolve_attachment_references;

		vk::AttachmentReference depth_stencil_attachment_reference;
		bool has_depth_stencil_attachment_reference = false;

		vector<uint32_t> preserve_attachment_indexes;

		// subpass description

		vk::PipelineBindPoint vk_pipeline_bind_point;

		// dependencies

		struct SubpassDependencyBuilder {
			SubpassDependencyBuilder(
				SubpassBuilder* src_subpass_builder,
				SubpassBuilder* dst_subpass_builder,
				vk::PipelineStageFlags src_stage_mask,
				vk::PipelineStageFlags dst_stage_mask,
				vk::AccessFlags src_access_mask,
				vk::AccessFlags dst_access_mask,
				vk::DependencyFlags dependency_flags = {}
			);

			vk::SubpassDependency build();

		private:
			SubpassBuilder* src_subpass_builder;
			SubpassBuilder* dst_subpass_builder;
			vk::PipelineStageFlags src_stage_mask;
			vk::PipelineStageFlags dst_stage_mask;
			vk::AccessFlags src_access_mask;
			vk::AccessFlags dst_access_mask;
			vk::DependencyFlags dependency_flags = {};
		};

		vector<SubpassDependencyBuilder> subpass_dependency_builders;

		uint32_t subpass_index;
	};

	// END SUBPASS CLASS


	RenderPassBuilder(const string& name, Tritium::Engine* engine, VirtualDevice* device);

	RenderPassBuilder* add_attachment_description(
		vk::AttachmentDescriptionFlags flags,
		vk::Format format,
		vk::SampleCountFlagBits samples,
		vk::AttachmentLoadOp load_op,
		vk::AttachmentStoreOp store_op,
		vk::AttachmentLoadOp stencil_load_op,
		vk::AttachmentStoreOp stencil_store_op,
		vk::ImageLayout initial_layout,
		vk::ImageLayout final_layout
	);

	RenderPassBuilder* add_sub_pass_builder(RenderPassBuilder::SubpassBuilder sub_pass_builder);

	// this builds out the final render_pass
	std::shared_ptr<RenderPass> build();

	// this is called after build
	void clean_up();
private:

	string name;

	Tritium::Engine* engine;

	VirtualDevice* device;

	// attachments

	vector<vk::AttachmentDescription> attachment_descriptions;

	// subpasses

	vector<RenderPassBuilder::SubpassBuilder> sub_pass_builders;
};

class RenderPass {
public:
	using Builder = RenderPassBuilder;
	RenderPass(Tritium::Engine* engine, VirtualDevice* device, vk::RenderPass vk_render_pass);

	void clean_up();

	vk::RenderPass vk_get_render_pass();

	map<uint32_t, std::shared_ptr<GraphicsPipeline>> graphics_pipeline_subpasses;
private:

	Tritium::Engine* engine;

	VirtualDevice* device;

	vk::RenderPass vk_render_pass;

	// sorted by subpass index.
	
};