#include "GFXVulkanCommandBufferPool.h"
#include <gfx-vk/BufferHelper.h>
#include <gfx-vk/GFXVulkanCommandBuffer.h>
#include <cassert>
#include <stdexcept>

namespace gfx
{
    uint32_t BufferHelper::FindMemoryType(GFXVulkanApplication* app, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(app->GetVkPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void BufferHelper::CreateBuffer(
        GFXVulkanApplication* app,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(app->GetVkDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(app->GetVkDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(app, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(app->GetVkDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(app->GetVkDevice(), buffer, bufferMemory, 0);
    }

    void BufferHelper::TransferBuffer(GFXVulkanApplication* app, VkBuffer src, VkBuffer dest, VkDeviceSize size)
    {
        GFXVulkanCommandBuffer buffer(app);

        VkCommandBufferBeginInfo beginInfo{};
        {
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        buffer.Begin();
        {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(buffer.GetVkCommandBuffer(), src, dest, 1, &copyRegion);
        }
        buffer.End();

        VkSubmitInfo submitInfo{};
        {
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &buffer.GetVkCommandBuffer();
        }

        vkQueueSubmit(app->GetVkGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(app->GetVkGraphicsQueue());
    }

    void BufferHelper::DestroyBuffer(GFXVulkanApplication* app, VkBuffer buffer, VkDeviceMemory mem)
    {
        vkDestroyBuffer(app->GetVkDevice(), buffer, nullptr);
        vkFreeMemory(app->GetVkDevice(), mem, nullptr);
    }


    static VkCommandBuffer _GetVkCommandBuffer(const gfx::GFXVulkanCommandBufferScope& scope)
    {
        return static_cast<gfx::GFXVulkanCommandBuffer*>(scope.operator->())->GetVkCommandBuffer();
    }
    static bool _HasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void BufferHelper::TransitionImageLayout(GFXVulkanApplication* app,
        VkImage image, VkFormat format, VkImageAspectFlags aspect
        , VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        GFXVulkanCommandBufferScope commandBuffer(app);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;

        barrier.subresourceRange.aspectMask = aspect;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage = GetStageFlagsForLayout(oldLayout);
        VkPipelineStageFlags destinationStage = GetStageFlagsForLayout(newLayout);
        barrier.srcAccessMask = GetAccessMaskForLayout(oldLayout);
        barrier.dstAccessMask = GetAccessMaskForLayout(newLayout);

        vkCmdPipelineBarrier(
            _GetVkCommandBuffer(commandBuffer),
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

    }



    void BufferHelper::TransitionImageLayout(
        VkCommandBuffer cmd, GFXVulkanTexture* tex, VkImageLayout newLayout)
    {
        VkImageLayout oldLayout = tex->GetVkImageLayout();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = tex->GetVkImageLayout();
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = tex->GetVkImage();



        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (_HasStencilComponent(tex->GetVkImageFormat()))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = tex->GetMipLevels();
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = tex->GetArrayCount();

        VkPipelineStageFlags sourceStage = GetStageFlagsForLayout(oldLayout);
        VkPipelineStageFlags destinationStage = GetStageFlagsForLayout(newLayout);
        barrier.srcAccessMask = GetAccessMaskForLayout(oldLayout);
        barrier.dstAccessMask = GetAccessMaskForLayout(newLayout);

        vkCmdPipelineBarrier(
            cmd,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    VkPipelineStageFlags BufferHelper::GetStageFlagsForLayout(VkImageLayout layout)
    {
        VkPipelineStageFlags flags{};
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR:
            return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        default:
            assert(false);
        }
        return {};
    }
    VkAccessFlags BufferHelper::GetAccessMaskForLayout(VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_ACCESS_TRANSFER_READ_BIT;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_ACCESS_TRANSFER_WRITE_BIT;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR:
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_ACCESS_SHADER_READ_BIT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR:
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return 0;

        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return 0;
        default:
            assert(false);
        }
        return {};
    }

    void BufferHelper::CopyBufferToImage(GFXVulkanApplication* app, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        gfx::GFXVulkanCommandBufferScope commandBuffer(app);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1};

        vkCmdCopyBufferToImage(
            _GetVkCommandBuffer(commandBuffer),
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
    }

    VkSampler BufferHelper::CreateTextureSampler(
        GFXVulkanApplication* app,
        VkFilter filter, VkSamplerAddressMode addressMode)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = filter;
        samplerInfo.minFilter = filter;

        samplerInfo.addressModeU = addressMode;
        samplerInfo.addressModeV = addressMode;
        samplerInfo.addressModeW = addressMode;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(app->GetVkPhysicalDevice(), &properties);

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // range 0 ~ 1

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VkSampler sampler;
        if (vkCreateSampler(app->GetVkDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
        return sampler;
    }

    static std::vector<VkFormat> _FindSupportedFormats(
        GFXVulkanApplication* app,
        const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        std::vector<VkFormat> formats;

        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(app->GetVkPhysicalDevice(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                formats.push_back(format);
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                formats.push_back(format);
            }
        }

        return formats;
    }

    std::vector<VkFormat> BufferHelper::FindDepthFormats(GFXVulkanApplication* app, bool assertEmpty)
    {
        auto result = _FindSupportedFormats(app,
                                            {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        if (assertEmpty)
        {
            assert(("depth format not found.", result.size() != 0));
        }
        return result;
    }

    VkFilter BufferHelper::GetVkFilter(GFXSamplerFilter filter)
    {
        switch (filter)
        {
        case gfx::GFXSamplerFilter::Nearest:
            return VkFilter::VK_FILTER_NEAREST;
        case gfx::GFXSamplerFilter::Linear:
            return VkFilter::VK_FILTER_LINEAR;
        case gfx::GFXSamplerFilter::Cubic:
            return VkFilter::VK_FILTER_CUBIC_IMG;
        default:
            assert(false);
            break;
        }
        return {};
    }
    VkSamplerAddressMode BufferHelper::GetVkAddressMode(GFXSamplerAddressMode mode)
    {
        switch (mode)
        {
        case gfx::GFXSamplerAddressMode::Repeat:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case gfx::GFXSamplerAddressMode::MirroredRepeat:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case gfx::GFXSamplerAddressMode::ClampToEdge:
            return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        default:
            assert(false);
            break;
        }
        return {};
    }

    static auto _Tex2VkFormatMapping()
    {
        static auto ptr = new std::unordered_map<GFXTextureFormat, VkFormat>{
            {gfx::GFXTextureFormat::R8_UNorm, VK_FORMAT_R8_UNORM},
            {gfx::GFXTextureFormat::R8G8B8A8_UNorm, VK_FORMAT_R8G8B8A8_UNORM},
            {gfx::GFXTextureFormat::R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB},
            {gfx::GFXTextureFormat::BC3_SRGB , VK_FORMAT_BC3_SRGB_BLOCK},
            {gfx::GFXTextureFormat::BC5_UNorm, VK_FORMAT_BC5_UNORM_BLOCK},
            {gfx::GFXTextureFormat::BC6H_RGB_SFloat, VK_FORMAT_BC6H_SFLOAT_BLOCK},
            {gfx::GFXTextureFormat::D32_SFloat, VK_FORMAT_D32_SFLOAT},
            {gfx::GFXTextureFormat::D32_SFloat_S8_UInt, VK_FORMAT_D32_SFLOAT_S8_UINT},
            {gfx::GFXTextureFormat::D24_UNorm_S8_UInt, VK_FORMAT_D24_UNORM_S8_UINT},
            {gfx::GFXTextureFormat::R16G16B16A16_SFloat, VK_FORMAT_R16G16B16A16_SFLOAT},
            {gfx::GFXTextureFormat::R32G32B32A32_SFloat, VK_FORMAT_R32G32B32A32_SFLOAT},
            {gfx::GFXTextureFormat::B10G11R11_UFloat, VK_FORMAT_B10G11R11_UFLOAT_PACK32}
        };
        return ptr;
    }

    static auto _Vk2TexFormatMapping()
    {
        static struct Init
        {

            std::unordered_map<VkFormat, GFXTextureFormat>* ptr;
            Init()
            {
                ptr = new std::unordered_map<VkFormat, GFXTextureFormat>;
                for (auto& [k, v] : *_Tex2VkFormatMapping())
                {
                    ptr->emplace(v, k);
                }
            }
        } _Init;
        return _Init.ptr;
    }

    VkFormat BufferHelper::GetVkFormat(GFXTextureFormat format)
    {
        auto dict = _Tex2VkFormatMapping();
        auto it = dict->find(format);
        if (it != dict->end())
        {
            return it->second;
        }
        assert(("no texture format conversion function found.", false));
        return {};
    }

    GFXTextureFormat BufferHelper::GetTextureFormat(VkFormat format)
    {
        auto dict = _Vk2TexFormatMapping();
        auto it = dict->find(format);
        if (it != dict->end())
        {
            return it->second;
        }
        assert(("no texture format conversion function found.", false));
        return {};
    }
} // namespace gfx