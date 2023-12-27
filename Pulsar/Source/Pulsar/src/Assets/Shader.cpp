#include "Assets/Shader.h"

#include <Pulsar/Assets/Shader.h>

#include <Pulsar/Assets/Texture.h>
#include <CoreLib.Serialization/JsonSerializer.h>

#include <Pulsar/Logger.h>
#include "Application.h"
#include "Assets/StaticMesh.h"

namespace pulsar
{
    using namespace std;

    sptr<Shader> Shader::StaticCreate(string_view name, ShaderSourceData&& pass)
    {
        Shader_sp self = mksptr(new Shader);
        self->Construct();
        self->SetName(name);
        self->m_shaderSource = std::move(pass);

        return self;
    }

    static size_t BeginBinaryField(std::iostream& stream, bool write, string& name)
    {
        auto pos = stream.tellp();
        sser::ReadWriteStream(stream, write, name);
        return pos;
    }
    static void EndBinaryField(std::iostream& stream, bool write, std::streampos start)
    {
        auto pos = stream.tellp();
        stream.seekp(start);
        size_t size = pos - start;
        sser::ReadWriteStream(stream, write, size);
    }
    Shader::Shader()
    {
        m_passName = mksptr(new String);
        m_preDefines = mksptr(new List<String_sp>);
    }
    void Shader::Serialize(AssetSerializer* s)
    {
        base::Serialize(s);
        if (!s->IsWrite) // read
        {
            m_preDefines->clear();

            auto passes = s->Object->At("Passes");
            *m_passName = passes->AsString();
        }
        else
        {
            auto passNameString = s->Object->New(ser::VarientType::String);
            passNameString->Assign(*m_passName);

            s->Object->Add("Passes", passNameString);
        }
        if (s->ExistStream)
        {
            ReadWriteStream(s->Stream, s->IsWrite, m_shaderSource);
        }
    }

    void Shader::OnDestroy()
    {
        base::OnDestroy();

    }

    void Shader::ResetShaderSource(const ShaderSourceData& serData)
    {
        m_shaderSource = serData;
    }
    array_list<gfx::GFXApi> Shader::GetSupportedApi() const
    {
        array_list<gfx::GFXApi> ret;
        for (auto& [k, v] : m_shaderSource.ApiMaps)
        {
            ret.push_back(k);
        }
        return ret;

    }


    static std::iostream& ReadWriteStream(std::iostream& stream, bool write, ShaderSourceData::ApiPlatform& data)
    {
        sser::ReadWriteStream(stream, write, data.Config);
        sser::ReadWriteStream(stream, write, data.Sources);
        return stream;
    }
    std::iostream& ReadWriteStream(std::iostream& stream, bool write, ShaderSourceData& data)
    {
        using namespace sser;
        using namespace ser;

        sser::ReadWriteStream(stream, write, data.ApiMaps);
        return stream;
    }

}
