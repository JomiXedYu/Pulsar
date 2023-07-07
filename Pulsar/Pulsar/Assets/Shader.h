#pragma once

#include <Pulsar/ObjectBase.h>
#include <Pulsar/AssetObject.h>
#include <Pulsar/Assets/Texture.h>
#include <Pulsar/IBindGPU.h>
#include <Pulsar/Math.h>
#include <Pulsar/Rendering/ShaderPass.h>

namespace pulsar
{
    class Texture2D;


    class Shader : public AssetObject, public IBindGPU
    {
        CORELIB_DEF_TYPE(AssemblyObject_Pulsar, pulsar::Shader, AssetObject);
    public:
        uint32_t get_id() const { return this->id_; }
    public:
        explicit Shader();
        virtual ~Shader() override;
        virtual string ToString() const override;
    public:
        void UseShader(int32_t pass_index);
    public:
        ShaderPass* GetPass(int32_t index);
    public:
        virtual void BindGPU() override;
        virtual void UnBindGPU() override;
        virtual bool GetIsBindGPU() override;
    public:
        int32_t GetUniformLocaltion(std::string_view name);
        void SetUniformInt(std::string_view name, const int32_t& i);
        void SetUniformFloat(std::string_view name, const float& f);
        void SetUniformMatrix4fv(std::string_view name, const float* value);
        void SetUniformMatrix4fv(std::string_view name, const Matrix4f& mat);
        void SetUniformVector3(std::string_view name, const Vector3f& value);
        void SetUniformColor(std::string_view name, const LinearColorf& value);
        void SetUniformColor(std::string_view name, const Vector3f& value);

        void SetUniformTexture(std::string_view name, Texture_rsp tex);
    protected:
    public:
        static sptr<Shader> StaticCreate(string_view name, array_list<ShaderPass*>&& pass);
        static sptr<Shader> StaticCreate(const string& shader_source);
    protected:
        uint32_t id_;
        array_list<ShaderPass*> pass_;
    private:
    };
    CORELIB_DECL_SHORTSPTR(Shader);



    class ShaderCompileException : public EngineException
    {
        
    public:
        virtual const char* name() const override { return "ShaderCompileException"; }

        ShaderCompileException(const string& name, const string& msg) : EngineException(msg), name_(name)
        {
            this->message_.insert(0, "filename: " + name + "");
        }

        string name_;
    };


}
