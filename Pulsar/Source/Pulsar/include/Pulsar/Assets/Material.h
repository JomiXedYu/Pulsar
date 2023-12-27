#pragma once

#include <Pulsar/AssetObject.h>
#include <Pulsar/Assets/Shader.h>
#include <Pulsar/Assets/Texture.h>
#include <variant>

namespace pulsar
{
    class IMaterialParameter
    {
    public:
        virtual void SetFloat(const index_string& name, float value) = 0;
        virtual void SetVector4(const index_string& name, const Vector4f& vec) = 0;
        virtual void SetTexture(const index_string& name, Texture_ref) = 0;

        virtual float GetFloat(const index_string& name) = 0;
        virtual Vector4f GetVector4(const index_string& name) = 0;
        virtual Texture_ref GetTexture(const index_string& name) = 0;
    };


    struct MaterialParameterValue
    {
    public:
        enum
        {
            FLOAT,
            VECTOR,
            TEXTURE,
        } Type;

        std::string Name;
        int Offset;

        void SetValue(float value)
        {
            Value = value;
            Type = FLOAT;
        }

        void SetValue(Vector4f value)
        {
            Value = value;
            Type = VECTOR;
        }

    private:
        std::variant<float, Vector4f, Texture_ref> Value;

    public:

        float AsFloat() const
        {
            return std::get<float>(Value);
        }
        Vector4f AsVector() const
        {
            return std::get<Vector4f>(Value);
        }
        Texture_ref AsTexture() const
        {
            return std::get<Texture_ref>(Value);
        }
        int GetDataSize() const
        {
            switch (Type)
            {
                return sizeof(int);
            case FLOAT:
                return sizeof(float);
                return sizeof(Color4f);
            case VECTOR:
                return sizeof(Vector4f);
            default:
                break;
            }
            return 0;
        }
    };



    class Material final : public AssetObject, public IMaterialParameter, public IGPUResource
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::Material, AssetObject);
        CORELIB_CLASS_ATTR(new MenuItemCreateAssetAttribute);

    public:
        static ObjectPtr<Material> StaticCreate(string_view name, Shader_ref shader);

        virtual void Serialize(AssetSerializer* s) override;

    public:
        virtual bool CreateGPUResource() override;
        virtual void DestroyGPUResource() override;
        virtual bool IsCreatedGPUResource() const override;

    public:
        void OnConstruct() override;

        // IMaterialParameter
        void SetFloat(const index_string& name, float value) override;
        void SetTexture(const index_string& name, Texture_ref value) override;
        void SetVector4(const index_string& name, const Vector4f& value) override;
        float GetFloat(const index_string& name) override;
        Vector4f GetVector4(const index_string& name) override;
        Texture_ref GetTexture(const index_string& name) override;

        void SubmitParameters();

        gfx::GFXDescriptorSet_sp GetGfxDescriptorSet() const
        {
            return m_descriptorSet;
        }
        gfx::GFXDescriptorSetLayout_sp GetGfxDescriptorSetLayout() const
        {
            return m_descriptorSetLayout;
        }
        gfx::GFXShaderPass_sp GetGfxShaderPass();
    public:
        Shader_ref GetShader() const;
        void SetShader(Shader_ref value);

        Action<> OnShaderChanged;

    protected:
        void PostEditChange(FieldInfo* info) override;

    private:
        CORELIB_REFL_DECL_FIELD(m_shader);
        Shader_ref m_shader;

        gfx::GFXShaderPass_sp m_gfxShaderPasses;

        hash_map<index_string, MaterialParameterValue> m_parameterValues;
        std::vector<uint8_t> m_bufferData;

        gfx::GFXDescriptorSet_sp m_descriptorSet;
        gfx::GFXDescriptorSetLayout_sp m_descriptorSetLayout;
        gfx::GFXBuffer_sp m_materialConstantBuffer;

        bool m_createdGpuResource = false;
        bool m_isDirtyParameter{};
        int m_renderQueue{};


    public:
        struct ShaderConstantPropertInfo
        {
            size_t Offset;
            ShaderParameterType Type;
        };
        hash_map<index_string, ShaderConstantPropertInfo> m_propertyInfo;
    };

    DECL_PTR(Material);

} // namespace pulsar
