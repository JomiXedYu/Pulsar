#pragma once
#include <gfx/GFXApplication.h>

#include <gfx/GFXSurface.h>
#include "VulkanInclude.h"
// #define GLFW_INCLUDE_VULKAN
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <glfw/include/GLFW/glfw3.h>
// #include <glfw/include/GLFW/glfw3native.h>

#include "GFXVulkanCommandBuffer.h"
#include "GFXVulkanRenderPass.h"
#include "GFXVulkanSwapchain.h"
#include "gfx/GFXTextureView.h"
#include <chrono>

namespace gfx
{

    class GFXVulkanApplication : public GFXApplication
    {
    public:
        explicit GFXVulkanApplication(GFXGlobalConfig config)
        {
            m_config = config;
        }
        ~GFXVulkanApplication() override = default;

    public:

        virtual GFXRenderer* GetRenderer() override;

        virtual void Initialize() override;
        virtual void ExecLoop() override;
        virtual void RequestStop() override;
        virtual void Terminate() override;

        virtual GFXApi GetApiType() const override { return GFXApi::Vulkan; }
        virtual const char* GetApiLevelName() const override { return "Vulkan 1.3"; }

        void TickRender(float deltaTime);
        virtual GFXBuffer_sp CreateBuffer(GFXBufferUsage usage, size_t bufferSize) override;
        virtual GFXCommandBuffer_sp CreateCommandBuffer() override;
        virtual GFXVertexLayoutDescription_sp CreateVertexLayoutDescription() override;
        virtual GFXGpuProgram_sp CreateGpuProgram(GFXGpuProgramStageFlags stage, const uint8_t* code, size_t length) override;


        virtual GFXGraphicsPipelineManager* GetGraphicsPipelineManager() const override
        {
            return m_graphicsPipelineManager;
        }

        virtual GFXTexture_sp CreateTexture2DFromMemory(
            const uint8_t* imageData, size_t length,
            int width, int height,
            GFXTextureFormat format,
            const GFXSamplerConfig& samplerConfig
            ) override;


        virtual GFXFrameBufferObject_sp CreateFrameBufferObject(
            const array_list<GFXTexture2DView_sp>& renderTargets,
            const GFXRenderPassLayout_sp& renderPassLayout) override;

        virtual GFXRenderPassLayout_sp CreateRenderPassLayout(const std::vector<GFXTexture2DView*>& renderTargets) override;

        virtual GFXTexture_sp CreateTextureCube(int32_t size) override;

        virtual GFXTexture_sp CreateRenderTarget(
            int32_t width, int32_t height, GFXTextureTargetType type,
            GFXTextureFormat format, const GFXSamplerConfig& samplerCfg) override;

        virtual GFXDescriptorManager* GetDescriptorManager() override;

        virtual GFXDescriptorSetLayout_sp CreateDescriptorSetLayout(
            const GFXDescriptorSetLayoutInfo* layouts,
            size_t layoutCount) override;

        virtual array_list<GFXTextureFormat> GetSupportedDepthFormats() override;

        class GFXVulkanDescriptorManager* GetVulkanDescriptorManager() const { return m_descriptorManager; }
        virtual GFXExtensions GetExtensionNames() override;
        virtual intptr_t GetWindowHandle() override;

        GFXSurface* GetWindow() const { return m_window; }
    public:
        const VkDevice& GetVkDevice() const { return m_device; }
        const VkPhysicalDevice& GetVkPhysicalDevice() const { return m_physicalDevice; }
        const VkInstance& GetVkInstance() const { return m_instance; }
        const VkSurfaceKHR& GetVkSurface() const { return m_surface; }
        const VkQueue& GetVkGraphicsQueue() const { return m_graphicsQueue; }
        const VkQueue& GetVkPresentQueue() const { return m_presentQueue; }
        //const VkCommandPool& GetVkCommandPool() const { return m_commandPool; }

        virtual GFXSwapchain* GetViewport() override { return m_viewport; }
        GFXVulkanSwapchain* GetVulkanViewport() { return m_viewport; }

        virtual void SetRenderPipeline(GFXRenderPipeline* pipeline) override
        {
            m_renderPipeline = pipeline;
        }
        virtual GFXRenderPipeline* GetRenderPipeline() const override
        {
            return m_renderPipeline;
        }
        class GFXVulkanCommandBufferPool* GetCommandBufferPool() const
        {
            return m_cmdPool;
        }
    private:
        void InitVkInstance();

        void InitPickPhysicalDevice();
        void InitLogicalDevice();

    public:

    protected:

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        GFXSurface* m_window = nullptr;
        // bool m_framebufferResized = false;

        VkInstance m_instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        //VkCommandPool m_commandPool = VK_NULL_HANDLE;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;

        GFXVulkanSwapchain* m_viewport = nullptr;

        GFXRenderPipeline* m_renderPipeline = nullptr;

        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        class GFXVulkanDescriptorManager* m_descriptorManager = nullptr;
        class GFXVulkanRenderer* m_renderer = nullptr;

        class GFXVulkanCommandBufferPool* m_cmdPool = nullptr;

        GFXGraphicsPipelineManager* m_graphicsPipelineManager = nullptr;

        array_list<const char*> m_extensions;
        size_t m_count = 0;

        bool m_isAppEnding = false;

        uint32_t m_framecount = 0;

        std::chrono::steady_clock::time_point m_startTime;
        std::chrono::steady_clock::time_point m_lastTime;


        std::vector<GFXTextureFormat> m_depthFormatCache;
    };


}