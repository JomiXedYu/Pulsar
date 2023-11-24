#include "PropertyControls/BoolPropertyControl.h"
#include <imgui/imgui.h>

namespace pulsared
{
    bool BoolPropertyControl::OnDrawImGui(const string& name, Object* prop)
    {
        assert(prop && prop->GetType() == GetPropertyType());
        Type* type = prop->GetType();
        auto b = static_cast<Boolean*>(prop);
        ImGui::PushItemWidth(-1);
        bool changed = ImGui::Checkbox(("##" + name).c_str(), &b->value);
        ImGui::PopItemWidth();
        return changed;
    }
}