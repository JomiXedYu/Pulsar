#pragma once
#include "PanelWindow.h"
#include <Pulsar/AssetObject.h>
#include <PulsarEd/EditorAssetManager.h>

namespace pulsared
{
    class AssetEditorMenuContext : public MenuContextBase
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::AssetEditorMenuContext, MenuContextBase);

    public:
        explicit AssetEditorMenuContext(AssetObject_ref asset)
            : Asset(asset)
        {
        }
        AssetObject_ref Asset;
    };

    class AssetEditorWindow : public PanelWindow
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::AssetEditorWindow, PanelWindow);
    public:

        virtual ImGuiWindowFlags GetGuiWindowFlags() const override;
        AssetEditorWindow();
        virtual bool IsDirtyAsset() const;
        virtual string GetWindowName() const override;
    public:
        virtual void OnDrawAssetPropertiesUI(float dt);
        virtual void OnDrawAssetPreviewUI(float dt);

        AssetObject_ref GetAssetObject() const { return m_assetObject; }
        void SetAssetObject(AssetObject_ref asset) { m_assetObject = asset; }

        void OnDrawImGui(float dt) override;
        virtual void OnRefreshMenuContexts();
    protected:
        AssetObject_ref m_assetObject;
        MenuContexts_sp m_menuBarCtxs;
    };


}