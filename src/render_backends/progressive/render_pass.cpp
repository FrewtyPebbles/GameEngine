#include "Engine/render_backends/progressive/render_pass.h"

/////////////////////////
// Render Pass Builder //
/////////////////////////

RenderPassBuilder::RenderPassBuilder(const string& name, Logger* logger, VirtualDevice* device)
: name(name), logger(logger), device(device) {

}

std::shared_ptr<RenderPass> RenderPassBuilder::build() {
	// TODO : BUILD THE SUBPASSES AND RENDER PASS

	vector<vk::SubpassDescription> subpassDescriptions;

	vector<vk::SubpassDependency> allSubpassDependencies;

	for (auto& subpassBuilder : this->sub_pass_builders) {
		subpassDescriptions.push_back(subpassBuilder.build());
		for (auto& subpassDependencyBuilder : subpassBuilder.subpass_dependency_builders) {
			allSubpassDependencies.push_back(subpassDependencyBuilder.build());
		}
	}

	vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo(
		{},
		this->attachment_descriptions,
		subpassDescriptions,
		allSubpassDependencies
	);

	auto vk_renderPass = this->device->get_vulkan_device()->createRenderPass(renderPassCreateInfo);

	return std::make_shared<RenderPass>(this->logger, this->device, vk_renderPass);
}

RenderPassBuilder* RenderPassBuilder::add_attachment_description(
	vk::AttachmentDescriptionFlags flags,
	vk::Format format,
	vk::SampleCountFlagBits samples,
	vk::AttachmentLoadOp load_op,
	vk::AttachmentStoreOp store_op,
	vk::AttachmentLoadOp stencil_load_op,
	vk::AttachmentStoreOp stencil_store_op,
	vk::ImageLayout initial_layout,
	vk::ImageLayout final_layout
) {
	// TODO : figure out how to determine if we are using depth and stencil tests here rather than just assuming false:
	samples = this->device->get_multisampling_samples_fallback(samples, false, false);

	this->attachment_descriptions.push_back(vk::AttachmentDescription(
		flags,
		format,
		samples,
		load_op,
		store_op,
		stencil_load_op,
		stencil_store_op,
		initial_layout,
		final_layout
	));
	return this;
}

RenderPassBuilder* RenderPassBuilder::add_sub_pass_builder(SubpassBuilder sub_pass_builder) {
	sub_pass_builder.subpass_index = this->sub_pass_builders.size();
	this->sub_pass_builders.push_back(sub_pass_builder);
	return this;
}

//////////////
// Sub Pass //
//////////////

RenderPassBuilder::SubpassBuilder::SubpassBuilder(const string& name, Logger* logger, VirtualDevice* device)
: name(name), logger(logger), device(device) {

}

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::add_input_attachment_reference(
	uint32_t attachment_index,
	vk::ImageLayout layout
) {
	this->input_attachment_references.push_back(vk::AttachmentReference(
		attachment_index,
		layout
	));
	return this;
}

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::add_color_attachment_reference(
	uint32_t attachment_index,
	vk::ImageLayout layout
) {
	this->color_attachment_references.push_back(vk::AttachmentReference(
		attachment_index,
		layout
	));
	return this;
}

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::add_resolve_attachment_reference(
	uint32_t attachment_index,
	vk::ImageLayout layout
) {
	this->resolve_attachment_references.push_back(vk::AttachmentReference(
		attachment_index,
		layout
	));
	return this;
}

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::set_depth_stencil_attachment_reference(
	uint32_t attachment_index,
	vk::ImageLayout layout
) {
	this->has_depth_stencil_attachment_reference = true;
	this->depth_stencil_attachment_reference = vk::AttachmentReference(
		attachment_index,
		layout
	);
	return this;
}

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::preserve_attachment(uint32_t index) {
	this->preserve_attachment_indexes.push_back(index);
	return this;
}

// dependecy

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::add_dependency(
	SubpassBuilder* src_subpass_builder,
	SubpassBuilder* dst_subpass_builder,
	vk::PipelineStageFlags src_stage_mask,
	vk::PipelineStageFlags dst_stage_mask,
	vk::AccessFlags src_access_mask,
	vk::AccessFlags dst_access_mask,
	vk::DependencyFlags dependency_flags
) {
	subpass_dependency_builders.push_back(RenderPassBuilder::SubpassBuilder::SubpassDependencyBuilder(
		src_subpass_builder,
		dst_subpass_builder,
		src_stage_mask,
		dst_stage_mask,
		src_access_mask,
		dst_access_mask,
		dependency_flags
	));
	return this;
}

RenderPassBuilder::SubpassBuilder::SubpassDependencyBuilder::SubpassDependencyBuilder(
	SubpassBuilder* src_subpass_builder,
	SubpassBuilder* dst_subpass_builder,
	vk::PipelineStageFlags      src_stage_mask,
	vk::PipelineStageFlags      dst_stage_mask,
	vk::AccessFlags             src_access_mask,
	vk::AccessFlags             dst_access_mask,
	vk::DependencyFlags         dependency_flags
) : src_subpass_builder(src_subpass_builder), dst_subpass_builder(dst_subpass_builder),
src_stage_mask(src_stage_mask), dst_stage_mask(dst_stage_mask), src_access_mask(src_access_mask),
dst_access_mask(dst_access_mask), dependency_flags(dependency_flags){

}

vk::SubpassDependency RenderPassBuilder::SubpassBuilder::SubpassDependencyBuilder::build() {
	return vk::SubpassDependency(
		this->src_subpass_builder->subpass_index,
		this->dst_subpass_builder->subpass_index,
		this->src_stage_mask,
		this->dst_stage_mask,
		this->src_access_mask,
		this->dst_access_mask,
		this->dependency_flags
	);
}

// subpass description

RenderPassBuilder::SubpassBuilder* RenderPassBuilder::SubpassBuilder::set_bind_point(vk::PipelineBindPoint vk_pipeline_bind_point) {
	this->vk_pipeline_bind_point = vk_pipeline_bind_point;
	
	return this;
}

vk::SubpassDescription RenderPassBuilder::SubpassBuilder::build() {
	return vk::SubpassDescription(
		{},
		this->vk_pipeline_bind_point,
		this->input_attachment_references,
		this->color_attachment_references,
		this->resolve_attachment_references,
		has_depth_stencil_attachment_reference ? &this->depth_stencil_attachment_reference : VK_NULL_HANDLE,
		this->preserve_attachment_indexes
	);
}

/////////////////
// Render Pass //
/////////////////

RenderPass::RenderPass(Logger* logger, VirtualDevice* device, vk::RenderPass vk_render_pass)
: logger(logger), device(device), vk_render_pass(vk_render_pass) {

}

void RenderPass::clean_up() {
	this->device->get_vulkan_device()->destroyRenderPass(vk_render_pass);
}

vk::RenderPass RenderPass::vk_get_render_pass() {
	return this->vk_render_pass;
}