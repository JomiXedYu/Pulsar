#pragma once
#include "GFXBuffer.h"
#include "GFXTexture.h"
#include "GFXInclude.h"
#include "GFXGpuProgram.h"
#include <string_view>

namespace gfx
{
    enum class GFXDescriptorType
    {
        ConstantBuffer,
        StructuredBuffer,
        CombinedImageSampler,
        Texture2D
    };


    struct GFXDescriptorSetLayoutInfo final
    {
    public:
        uint32_t BindingPoint;
        GFXDescriptorType Type;
        GFXGpuProgramStageFlags Stage;

        GFXDescriptorSetLayoutInfo(
            GFXDescriptorType type,
            GFXGpuProgramStageFlags stage,
            uint32_t bindingPoint = 0,
            uint32_t spacePoint = 0)
            : Type(type), Stage(stage), BindingPoint(bindingPoint)
        {
        }
    };

    class GFXDescriptorSetLayout
    {
    protected:
        GFXDescriptorSetLayout()
        {
        }
        virtual ~GFXDescriptorSetLayout() {}
    };
    GFX_DECL_SPTR(GFXDescriptorSetLayout);

    class GFXDescriptor
    {
    public:
        GFXDescriptor() {}
        virtual ~GFXDescriptor() {}
        GFXDescriptor(const GFXDescriptor&) = delete;

        virtual void SetConstantBuffer(GFXBuffer* buffer) = 0;
        virtual void SetStructuredBuffer(GFXBuffer* buffer) = 0;
        virtual void SetTextureSampler2D(GFXTexture2DView* texture) = 0;
        virtual void SetTexture2D(GFXTexture* texture) = 0;

        bool IsDirty;
        std::string name;
    };
    GFX_DECL_SPTR(GFXDescriptor);

    class GFXDescriptorSet
    {
    protected:
        GFXDescriptorSet() {}
        virtual ~GFXDescriptorSet() {}
    public:
        virtual GFXDescriptor* AddDescriptor(std::string_view name, uint32_t bindingPoint) = 0;
        virtual GFXDescriptor* GetDescriptorAt(int index) = 0;
        virtual int32_t GetDescriptorCount() const = 0;
        virtual GFXDescriptor* Find(std::string_view name) = 0;
        virtual GFXDescriptor* FindByBinding(uint32_t bindingPoint) = 0;
        virtual void Submit() = 0;
        virtual intptr_t GetId() = 0;
        virtual GFXDescriptorSetLayout_sp GetDescriptorSetLayout() const = 0;
    };
    GFX_DECL_SPTR(GFXDescriptorSet);

}