#include "Components/LightComponent.h"

#include "World.h"

namespace pulsar
{

    void LightComponent::SetIntensity(float value)
    {
        m_intensity = value;
        OnIntensityChanged();
    }
    void LightComponent::SetColor(Color4f value)
    {
        m_lightColor = value;
        OnLightColorChanged();
    }

    void LightComponent::MarkRenderingDirty()
    {
        auto manager = GetWorld()->GetLightManager();
        manager->MarkDirty(manager->GetId(&m_runtimeLightData));
    }
    void LightComponent::PostEditChange(FieldInfo* info)
    {
        base::PostEditChange(info);
        const auto& name = info->GetName();
        if (name == NAMEOF(m_intensity))
        {
            OnIntensityChanged();
        }
        else if (name == NAMEOF(m_lightColor))
        {
            OnLightColorChanged();
        }
    }
} // namespace pulsar