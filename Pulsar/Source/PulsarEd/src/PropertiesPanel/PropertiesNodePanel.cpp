#include "PropertiesPanel/PropertiesNodePanel.h"

#include "EditorWorld.h"
#include "Menus/Menu.h"
#include "Menus/MenuRenderer.h"


#include <PulsarEd/PropertyControls/PropertyControl.h>

namespace pulsared
{

    static void _Property2Name()
    {
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 120);
        ImGui::AlignTextToFramePadding();
    }
    static void _Property2Value()
    {
        ImGui::NextColumn();
    }
    static void _Property2End()
    {
        ImGui::Columns(1);
    }


    static constexpr int kTableRowHeight = 30;

    void PropertiesNodePanel::OnDrawImGui()
    {
        auto world = dynamic_cast<EditorWorld*>(EditorWorld::GetPreviewWorld());
        assert(world);

        auto selectedObj = world->GetSelection().GetSelected();
        if (!selectedObj)
        {
            return;
        }
        Node_ref selected;
        if (cltypeof<Node>()->IsInstanceOfType(selectedObj.GetPtr()))
        {
            selected = selectedObj;
        }
        else
        {
            return;
        }

        static char name[255];
        StringUtil::strcpy(name, selected->GetName());

        ImGui::Spacing();
        {
            bool is_active = selected->GetIsActiveSelf();
            ImGui::Checkbox("##active", &is_active);
            if (is_active != selected->GetIsActiveSelf())
            {
                selected->SetIsActiveSelf(is_active);
            }
            ImGui::SameLine();
            //ImGui::PushItemWidth(-1);
            if (ImGui::InputText("##Name", name, 255))
            {
                if (selected->GetName() != name)
                {
                    selected->SetName(name);
                }
            }

            ImGui::SameLine();
            ImGui::Checkbox("##debug", &m_debugMode);
            ImGui::SameLine();
            ImGui::Text("DebugMode");

            //ImGui::PopItemWidth();
        }

        ImGui::Spacing();

        ImGui::AlignTextToFramePadding();

        if(ImGui::Button("Add Component", {-FLT_MIN, 20}))
        {
            ImGui::OpenPopup("SceneEditor.Components");

        }
        if (ImGui::BeginPopup("SceneEditor.Components"))
        {
            auto menu = MenuManager::GetMenu("SceneEditor")->FindSubMenuEntry("Components");
            MenuRenderer::RenderMenu(menu.get(), mksptr(new MenuContexts));
            ImGui::EndPopup();
        }

        auto componentArr = selected->GetAllComponentArray();
        for (auto& comp : componentArr)
        {
            if(!comp) continue;

            bool opened = true;
            const bool dontDestroy = comp->GetType()->IsSubclassOf(cltypeof<TransformComponent>());
            string componentFriendlyName = ComponentInfoManager::GetFriendlyComponentName(comp->GetType());

            ImGui::PushID(comp.GetPtr());
            if (ImGui::CollapsingHeader(
                componentFriendlyName.c_str(),
                dontDestroy ? nullptr : &opened,
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto componentGuid = comp->GetObjectHandle().to_string();
                Type* componentType = comp->GetType();
                auto fields = componentType->GetFieldInfos(TypeBinding::NonPublic);

                PImGui::ObjectFieldProperties(
                    comp->GetType(),
                    comp->GetType(),
                    comp.GetPtr(),
                    comp.GetPtr(), m_debugMode);

                if (m_debugMode)
                {
                    if (PImGui::BeginPropertyLines())
                    {
                        PImGui::PropertyLineText("Handle", comp.GetHandle().to_string());
                        PImGui::EndPropertyLines();
                    }
                }
            }
            ImGui::PopID();

            if(!opened)
            {
                selected->DestroyComponent(comp);
            }
        }

        if (PImGui::PropertyGroup("Node MetaInfo"))
        {
            if (PImGui::BeginPropertyLines())
            {
                PImGui::PropertyLineText("Handle", selected->GetObjectHandle().to_string().c_str());
                PImGui::EndPropertyLines();
            }
        }

        ImGui::Spacing();
    }
} // namespace pulsared