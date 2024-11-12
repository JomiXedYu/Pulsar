#pragma once
#include <CoreLib/Events.hpp>
#include <Pulsar/ObjectBase.h>
#include <Pulsar/World.h>
#include <Pulsar/Rendering/Pipelines/Pipeline.h>
#include <gfx/GFXApplication.h>
#include <uinput/InputManager.h>

namespace pulsar
{
    class Material;
    class AssetManager;



    class AppInstance
    {
        friend class Application;
    public:
        virtual ~AppInstance() = default;
        virtual const char* AppType() = 0;
        virtual void RequestQuit() = 0;
        virtual Vector2f GetOutputScreenSize() = 0;
        virtual void SetOutputScreenSize(Vector2f size) = 0;
        virtual string GetTitle() = 0;
        virtual void SetTitle(string_view title) = 0;
        virtual std::filesystem::path AppRootDir() = 0;
        virtual rendering::Pipeline* GetPipeline() = 0;

        virtual array_list<gfx::GFXApi> GetSupportedApis();

        Action<> QuittingEvents;
        Function<bool> RequestQuitEvents;

        template<typename T>
        SPtr<T> GetSubsystem()
        {
            return sptr_cast<T>(GetSubsystemByType(cltypeof<T>()));
        }

        virtual AssetManager* GetAssetManager() = 0;

        virtual void OnPreInitialize(gfx::GFXGlobalConfig* cfg) = 0;
        virtual void OnInitialized() = 0;
        virtual void OnTerminate() = 0;
        virtual void OnBeginRender(float dt) = 0;
        virtual void OnEndRender(float d4) = 0;
        virtual bool IsQuit() = 0;

        Action<float> OnRenderTick;

    protected:

    };
}