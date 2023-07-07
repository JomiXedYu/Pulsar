#pragma once
#include "EditorWindow.h"
#include <Pulsar/Components/CameraComponent.h>

namespace pulsared
{
    class SceneWindow : public EditorWindow
    {
        CORELIB_DEF_TYPE(AssemblyObject_PulsarEd, pulsared::SceneWindow, EditorWindow);
    public:
        virtual ImGuiWindowFlags GetGuiWindowFlags() const override {
            return ImGuiWindowFlags_None | ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;
        }
        SceneWindow();
        virtual void OnOpen() override;
        virtual void OnClose() override;
        static string_view StaticWindowName() { return "Scene"; }
        virtual string_view GetWindowName() const override { return StaticWindowName(); }
        virtual void OnDrawImGui() override;


        Node_sp GetSceneCameraNode() { return this->camera_node; }
        Node_sp GetSceneCameraControllerNode() { return this->camera_ctrl_node; }
        CameraComponent_sp GetSceneCamera() { return this->camera_node->GetComponent<CameraComponent>(); }
    private:
        virtual void OnWindowResize();
    private:
        int32_t drawmode_select_index = 0;
        Node_sp camera_node;
        Node_sp camera_ctrl_node;
        Vector2i win_size_;
    };
}