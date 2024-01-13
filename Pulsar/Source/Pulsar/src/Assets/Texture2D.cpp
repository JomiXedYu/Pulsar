#include <CoreLib/File.h>
#include <Pulsar/Application.h>
#include <Pulsar/Assets/Texture2D.h>
#include <Pulsar/Util/TextureCompressionUtil.h>
#include <gfx/GFXImage.h>

namespace pulsar
{
    Texture2D::Texture2D()
    {
    }
    Texture2D::~Texture2D() = default;

    template <typename T>
    void AssignEnum(T& e, string_view name)
    {
        uint32_t value{};
        Enum::StaticTryParse(cltypeof<get_boxing_type_t<T>>(), name, &value);
        e = (T)value;
    }

    void Texture2D::Serialize(AssetSerializer* s)
    {
        base::Serialize(s);
        if (s->IsWrite)
        {
            assert(m_loadedNativeMemory);
            sser::ReadWriteStream(s->Stream, s->IsWrite, m_nativeMemory);

            s->Object->Add("IsCompressedNativeData", m_compressedNativeImage);

            auto size = s->Object->New(ser::VarientType::Object);
            size->Add("x", m_textureSize.x);
            size->Add("y", m_textureSize.y);
            s->Object->Add("Size", size);

            s->Object->Add("ChannelCount", m_channelCount);

            s->Object->Add("IsSRGB", m_isSRGB);
            s->Object->Add("CompressedFormat", mkbox(m_compressionFormat)->GetName());
        }
        else // read
        {
            m_nativeMemory.clear();
            sser::ReadWriteStream(s->Stream, s->IsWrite, m_nativeMemory);
            m_compressedNativeImage = s->Object->At("IsCompressedNativeData")->AsBool();

            auto size = s->Object->At("Size");
            m_textureSize.x = size->At("x")->AsInt();
            m_textureSize.y = size->At("y")->AsInt();

            m_channelCount = s->Object->At("ChannelCount")->AsInt();

            m_isSRGB = s->Object->At("IsSRGB")->AsBool();
            auto compressedFormat = s->Object->At("CompressedFormat")->AsString();
            AssignEnum(m_compressionFormat, compressedFormat);

            m_loadedNativeMemory = true;
        }
    }

    hash_map<TextureCompressionFormat, gfx::GFXTextureFormat>* Texture2D::StaticGetFormatMapping(OSPlatform platform)
    {
        static hash_map<TextureCompressionFormat, gfx::GFXTextureFormat> map{
                {TextureCompressionFormat::ColorSRGB_Compressed, gfx::GFXTextureFormat::BC3_SRGB},
                {TextureCompressionFormat::BitmapRGBA, gfx::GFXTextureFormat::R8G8B8A8_UNorm},
                {TextureCompressionFormat::Gray, gfx::GFXTextureFormat::R8_UNorm},
                {TextureCompressionFormat::NormalMap_Compressed, gfx::GFXTextureFormat::BC5_UNorm},
                {TextureCompressionFormat::HDR_Compressed, gfx::GFXTextureFormat::BC6H_RGB_SFloat}};
        return &map;
    }
    void Texture2D::OnDestroy()
    {
        base::OnDestroy();
        this->DestroyGPUResource();
    }
    void Texture2D::OnInstantiateAsset(AssetObject* obj)
    {
    }

    void Texture2D::FromNativeData(const uint8_t* data, size_t length, bool compressed, int width, int height, int channel)
    {
        m_nativeMemory.resize(length);
        std::memcpy(m_nativeMemory.data(), data, length);
        m_isSRGB = true;
        m_compressedNativeImage = compressed;
        m_textureSize.x = width;
        m_textureSize.y = height;
        m_channelCount = channel;
        m_loadedNativeMemory = true;
    }
    void Texture2D::PostEditChange(FieldInfo* info)
    {
        base::PostEditChange(info);
        auto name = info->GetName();
        if (name == NAMEOF(m_compressionFormat) ||
            name == NAMEOF(m_isSRGB))
        {
            if (IsCreatedGPUResource())
            {
                DestroyGPUResource();
                CreateGPUResource();
            }
        }

    }

    static gfx::GFXTextureFormat _GetTextureFormat(TextureCompressionFormat format)
    {
        return Texture2D::StaticGetFormatMapping(OSPlatform::Windows64)->at(format);
    }

    bool Texture2D::CreateGPUResource()
    {
        if (IsCreatedGPUResource())
        {
            return true;
        }

        auto targetGfxFormat = _GetTextureFormat(m_compressionFormat);

        array_list<uint8_t> data{};
#ifdef WITH_EDITOR
        {
            array_list<uint8_t> uncompressedData;
            if (m_compressedNativeImage)
            {
                uncompressedData = gfx::LoadImageFromMemory(m_nativeMemory.data(), m_nativeMemory.size(),
                                                               nullptr, nullptr, nullptr, m_channelCount, m_isSRGB);
            }
            else
            {
                uncompressedData = m_nativeMemory;
            }

            auto compressedData = TextureCompressionUtil::Compress(
                std::move(uncompressedData),
                m_textureSize.x,
                m_textureSize.y,
                m_channelCount,
                targetGfxFormat);
            data = std::move(compressedData);
        }
#else
        data = m_nativeMemory;
#endif

        m_isCreatedGPUResource = true;

        m_tex = Application::GetGfxApp()->CreateTexture2DFromMemory(
            data.data(),
            data.size(),
            m_textureSize.x, m_textureSize.y,
            targetGfxFormat,
            m_samplerConfig);

        return true;
    }

    void Texture2D::DestroyGPUResource()
    {
        if (!IsCreatedGPUResource())
        {
            return;
        }
        m_isCreatedGPUResource = false;
        m_tex.reset();
    }

    bool Texture2D::IsCreatedGPUResource() const
    {
        return m_isCreatedGPUResource;
    }

} // namespace pulsar
