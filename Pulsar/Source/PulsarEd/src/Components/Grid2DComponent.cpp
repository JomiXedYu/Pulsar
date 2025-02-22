#include "Components/Grid2DComponent.h"

#include "Pulsar/Components/StaticMeshRendererComponent.h"

#include <Pulsar/AssetManager.h>
#include <Pulsar/Assets/StaticMesh.h>
#include <Pulsar/Rendering/LineRenderObject.h>
#include <PulsarEd/Windows/EditorWindowManager.h>

namespace pulsared
{

    void Grid2DComponent::BeginComponent()
    {
        base::BeginComponent();

        int line_count = 200;
        float detail_distance = 1;
        float total_width = detail_distance * line_count;

        Color4f detailLineColor = {0.2f, 0.2f, 0.2f, 1};
        float zOffset = 0.01f;
        for (int x = -line_count / 2; x <= line_count / 2; x++)
        {
            if (x == 0)
            {
                Color4f color = detailLineColor;
                color.r = 0.9f;
                m_vert.emplace_back(total_width / 2, detail_distance * x, zOffset);
                m_vert.emplace_back(0, detail_distance * x, 0);
                m_vert.emplace_back(0, detail_distance * x, 0);
                m_vert.emplace_back(-total_width / 2, detail_distance * x, zOffset);
                m_colors.push_back(color);
                m_colors.push_back(color);
                m_colors.push_back(color);
                m_colors.push_back(color);
            }
            else
            {
                m_vert.emplace_back(total_width / 2, detail_distance * x, zOffset);
                m_vert.emplace_back(-total_width / 2, detail_distance * x, zOffset);
                m_colors.push_back(detailLineColor);
                m_colors.push_back(detailLineColor);
            }
        }
        for (int z = -line_count / 2; z <= line_count / 2; z++)
        {
            if (z == 0)
            {
                Color4f color = detailLineColor;
                color.r = 0.0f;
                color.g = 1.f;
                color.b = 0.f;
                m_vert.emplace_back(detail_distance * z, total_width / 2, zOffset);
                m_colors.push_back(color);
                m_vert.emplace_back(detail_distance * z, 0, zOffset);
                m_colors.push_back(color);
                m_vert.emplace_back(detail_distance * z, 0, zOffset);
                m_colors.push_back(color);
                m_vert.emplace_back(detail_distance * z, -total_width / 2, zOffset);
                m_colors.push_back(color);
            }
            else
            {
                m_vert.emplace_back(detail_distance * z, total_width / 2, zOffset);
                m_vert.emplace_back(detail_distance * z, -total_width / 2, zOffset);
                m_colors.push_back(detailLineColor);
                m_colors.push_back(detailLineColor);
            }
        }

        // m_vert.emplace_back(0, 0, 0);
        // m_vert.emplace_back(0, 1, 0);
        // m_colors.push_back({0, 1, 0, 1});
        // m_colors.push_back({0, 1, 0, 1});

        m_renderObject = CreateRenderObject();
        GetNode()->GetRuntimeWorld()->AddRenderObject(m_renderObject);
        OnTransformChanged();
    }

    void Grid2DComponent::EndComponent()
    {
        base::EndComponent();
        GetNode()->GetRuntimeWorld()->RemoveRenderObject(m_renderObject);
        m_renderObject.reset();
    }

    void Grid2DComponent::OnTransformChanged()
    {
        base::OnTransformChanged();
        m_renderObject->SetTransform(GetNode()->GetTransform()->GetLocalToWorldMatrix());
    }

    SPtr<rendering::RenderObject> Grid2DComponent::CreateRenderObject()
    {
        auto ro = new LineRenderObject();
        ro->SetPoints(m_vert, m_colors);

        return mksptr(ro);
    }

} // namespace pulsared