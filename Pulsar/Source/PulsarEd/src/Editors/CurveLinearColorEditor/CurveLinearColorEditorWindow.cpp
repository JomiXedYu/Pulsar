#include "PulsarEd/Editors/CurveLinearColorEditor/CurveLinearColorEditorWindow.h"

#include "ImCurveEdit.h"
#include "ImGuiExt.h"
#include "ImGradientHDR.h"
#include "PropertyControls/PropertyControl.h"
#include "imgui/imgui_internal.h"

namespace pulsared
{

    void CurveLinearColorEditorWindow::OnOpen()
    {
        base::OnOpen();
        m_colorCurve = cref_cast<CurveLinearColor>(m_assetObject);
    }
    void CurveLinearColorEditorWindow::OnClose()
    {
        base::OnClose();
    }

    struct RampEdit : ImCurveEdit::Delegate
    {
        CurveLinearColor* m_curve;
        array_list<array_list<ImVec2>> m_imData;
        ImVec2 m_min{};
        ImVec2 m_max{};

        RampEdit(CurveLinearColor* curve) : m_curve(curve)
        {
            BuildImData();
        }

        size_t GetCurveCount() override { return m_curve->GetCurveCount(); }
        ImVec2& GetMin() override { return m_min; }
        ImVec2& GetMax() override { return m_max; }
        size_t GetPointCount(size_t curveIndex) override
        {
            return m_curve->GetCurveData(curveIndex)->GetKeyCount();
        }

        uint32_t GetCurveColor(size_t curveIndex) override
        {
            return 0xFFFFFFFF;
        }

        void BuildImData()
        {
            m_imData.resize(m_curve->GetCurveCount());
            for (int curveIndex = 0; curveIndex < m_imData.size(); ++curveIndex)
            {
                auto& curve = m_curve->GetCurveData(curveIndex);
                m_imData[curveIndex].resize(curve->GetKeyCount());
                auto keyCount = curve->GetKeyCount();
                for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex)
                {
                    m_imData[curveIndex][keyIndex] = ImVec2( curve->GetKey(keyIndex).Time, curve->GetKey(keyIndex).Value );
                }
            }
        }

        ImVec2* GetPoints(size_t curveIndex) override
        {
            return m_imData[curveIndex].data();
        }

        int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
        {
            auto key = m_curve->GetCurveData(curveIndex)->GetKey(pointIndex);
            key.Time = value.x;
            key.Value = value.y;
            m_curve->GetCurveData(curveIndex)->SetKey(pointIndex, key);
            BuildImData();
            return 0;
        }

        void AddPoint(size_t curveIndex, ImVec2 value) override
        {
            auto key = CurveKey{};
            key.Time = value.x;
            key.Value = value.y;
            m_curve->GetCurveData(curveIndex)->AddKey(key);

            BuildImData();
        }
    };

    void CurveLinearColorEditorWindow::OnDrawImGui(float dt)
    {
        base::OnDrawImGui(dt);
        if (!m_colorCurve)
        {
            return;
        }

        auto id = ImGui::GetID("cx");
        ImGradientHDRState state;

        auto keyCount = m_colorCurve->GetCurveData(0)->GetKeyCount();
        auto curveCount = m_colorCurve->GetCurveCount();

        for (int i = 0; i < keyCount; ++i)
        {
            auto time = m_colorCurve->GetCurveData(0)->GetKey(i).Time;
            std::array<float, 3> color;
            color[0] = m_colorCurve->GetCurveData(0)->GetKey(i).Value;
            color[1] = m_colorCurve->GetCurveData(1)->GetKey(i).Value;
            color[2] = m_colorCurve->GetCurveData(2)->GetKey(i).Value;
            state.AddColorMarker(time, color, 1);;
        }

        auto oldKeyCount = state.ColorCount;

        if (ImGradientHDR(id,state, temporaryState))
        {
            auto newKeyCount = state.ColorCount;
            for (int i = oldKeyCount; i < newKeyCount; ++i)
            {
                m_colorCurve->InsertColorKey();
            }

            for (int keyIndex = 0; keyIndex < state.ColorCount; ++keyIndex)
            {
                auto keys = m_colorCurve->GetColorsKey(keyIndex);
                for (int colorChannelIndex = 0; colorChannelIndex < 3; ++colorChannelIndex)
                {
                    keys[colorChannelIndex].Time = state.Colors[keyIndex].Position;
                    keys[colorChannelIndex].Value = state.Colors[keyIndex].Color[colorChannelIndex];
                }
                m_colorCurve->SetColorsKey(keyIndex, keys);

            }
            m_colorCurve->PostEditChange(nullptr);
        }

        if (PImGui::PropertyGroup("Curve Color"))
        {
            auto type = cltypeof<CurveLinearColor>();
            PImGui::ObjectFieldProperties(type, type, m_colorCurve.GetPtr(), m_colorCurve.GetPtr(), false);
        }


    }
} // namespace pulsared