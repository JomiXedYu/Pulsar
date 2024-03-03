#include "MoveEdTool.h"

#include "EditorSelection.h"
#include <imgui/imgui.h>
#include "ImGuizmo.h"
#include "Pulsar/Components/CameraComponent.h"
#include "Pulsar/Node.h"

namespace pulsared
{
    void MoveEdTool::Begin()
    {
        base::Begin();
    }
    void MoveEdTool::Tick(float dt)
    {
        m_enableSelect = !ImGuizmo::IsOver();
        base::Tick(dt);

        if (EditorSelection::Selection.IsEmpty())
        {
            return;
        }
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        auto windowWidth = (float)ImGui::GetWindowWidth();
        auto windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        auto viewMat = m_world->GetPreviewCamera()->GetViewMat();
        auto projMat = m_world->GetPreviewCamera()->GetProjectionMat();
        auto node = EditorSelection::Selection.GetSelected();
        auto matrix = node->GetTransform()->GetLocalToWorldMatrix();

        static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::LOCAL);


        if (ImGuizmo::Manipulate(
            viewMat.get_value_ptr(),
            projMat.get_value_ptr(),
            ImGuizmo::TRANSLATE,
            CurrentGizmoMode,
            matrix.get_value_ptr(),
            nullptr, nullptr, nullptr, nullptr))
        {
            node->GetTransform()->SetWorldPosition(matrix[3].xyz());
        }

        // ImGuizmo::Manipulate(
        //     viewMat.get_value_ptr(),
        //     projMat.get_value_ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, matrix.get_value_ptr(), NULL,
        // useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
    }
} // namespace pulsared
