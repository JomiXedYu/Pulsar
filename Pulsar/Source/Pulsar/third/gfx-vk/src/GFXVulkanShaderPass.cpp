#include "GFXVulkanShaderPass.h"
#include <gfx-vk/GFXVulkanShaderPass.h>
#include <gfx-vk/GFXVulkanApplication.h>
#include <gfx-vk/GFXVulkanVertexLayoutDescription.h>
#include <gfx-vk/GFXVulkanDescriptorSet.h>
#include <gfx-vk/GFXVulkanGpuProgram.h>
#include <memory>
#include <stdexcept>

namespace gfx
{

    GFXVulkanShaderPass::GFXVulkanShaderPass(
        GFXVulkanApplication* app,
        const GFXShaderPassConfig& config,
        const std::shared_ptr<GFXVulkanGpuProgram>& gpuProgram,
        //const array_list<GFXDescriptorSetLayout_sp>& descriptorSetLayout,
        const array_list<GFXVertexLayoutDescription_sp>& vertexLayout
    ) :
        m_app(app),
        m_passConfig(config),
        m_gpuProgram(gpuProgram),
        //m_descriptorSetLayout(descriptorSetLayout),
        m_vertexLayoutDescription(vertexLayout)
    {

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        {
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = gpuProgram->GetVkVertShaderModule();
            vertShaderStageInfo.pName = "main";
        }
        m_stages.push_back(vertShaderStageInfo);

        VkPipelineShaderStageCreateInfo pixelShaderStageInfo{};
        {
            pixelShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pixelShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            pixelShaderStageInfo.module = gpuProgram->GetVkPixelShaderModule();
            pixelShaderStageInfo.pName = "main";
        }
        m_stages.push_back(pixelShaderStageInfo);
    }

    GFXVulkanShaderPass::~GFXVulkanShaderPass()
    {
    }
    // array_list<VkDescriptorSetLayout> GFXVulkanShaderPass::GetVkDescriptorSetLayout() const
    // {
    //     array_list<VkDescriptorSetLayout> list;
    //     for (auto& layout : m_descriptorSetLayout)
    //     {
    //         auto l = static_cast<GFXVulkanDescriptorSetLayout*>(layout.get())->GetVkDescriptorSetLayout();
    //         list.push_back(l);
    //     }
    //     return list;
    // }

} // namespace gfx