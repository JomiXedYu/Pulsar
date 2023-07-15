﻿#include <Pulsar/Application.h>
#include <gfx-vk/GFXVulkanApplication.h>

namespace pulsar
{

    static AppInstance* g_currentInst;

    AppInstance* Application::inst()
    {
        return g_currentInst;
    }
    static gfx::GFXVulkanApplication* g_gfxApp = nullptr;
    gfx::GFXApplication* Application::GetGfxApp()
    {
        return g_gfxApp;
    }
    int Application::Exec(AppInstance* instance, string_view title, Vector2f size)
    {
        g_currentInst = instance;

        gfx::GFXGlobalConfig gfxConfig;
        instance->OnPreInitialize(&gfxConfig);

        if (gfxConfig.WindowWidth < 0 || gfxConfig.WindowHeight < 0)
        {
            assert(false);
        }

        g_gfxApp = new gfx::GFXVulkanApplication(gfxConfig);
        g_gfxApp->Initialize();
        instance->OnInitialized();

        g_gfxApp->OnPreRender = [](float dt)
        {
            g_currentInst->OnBeginRender(dt);
        };
        g_gfxApp->OnPostRender = [](float dt)
        {
            g_currentInst->OnEndRender(dt);
        };
        g_gfxApp->ExecLoop();
        instance->OnTerminate();
        g_gfxApp->Terminate();
        delete g_gfxApp;

        return 0;
    }

}


