#include "GFXVulkanGraphicsPipelineManager.h"
#include "GFXVulkanGraphicsPipeline.h"

namespace gfx
{
    GFXVulkanGraphicsPipelineManager::~GFXVulkanGraphicsPipelineManager()
    {

    }
    GFXVulkanGraphicsPipelineManager::GFXVulkanGraphicsPipelineManager(GFXVulkanApplication* app)
        : m_app(app)
    {

    }


    inline intptr_t HashPointer2(void* p, void* q)
    {
        constexpr intptr_t HashS0 = 2166136261;
        constexpr intptr_t HashS1 = 16777619;
        return ((HashS0 * HashS1) ^ (intptr_t)p) * HashS1 ^ (intptr_t)q;
    }

    std::shared_ptr<GFXGraphicsPipeline> GFXVulkanGraphicsPipelineManager::GetGraphicsPipeline(
        const std::shared_ptr<GFXShaderPass>& shaderPass,
        const std::shared_ptr<GFXRenderPassLayout>& renderPass)
    {
        auto hash = HashPointer2(shaderPass.get(), renderPass.get());

        auto v = m_caches.find(hash);
        if (v != m_caches.end())
        {
            return v->second;
        }

        auto gpipeline = std::shared_ptr<GFXGraphicsPipeline>(new GFXVulkanGraphicsPipeline(m_app, shaderPass, *renderPass.get()));
        m_caches.insert({ hash, gpipeline });
        return gpipeline;
    }

    void GFXVulkanGraphicsPipelineManager::GCollect()
    {

    }
}
