#pragma once
#include "Shape3DComponent.h"

namespace pulsar
{
    class SphereShape3DComponent : public Shape3DComponent
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::SphereShape3DComponent, Shape3DComponent);
        CORELIB_CLASS_ATTR(new CategoryAttribute("3D"));
    public:
        Shape3DType GetShapeType() const override { return Shape3DType::Sphere; }
        void OnDrawGizmo(GizmoPainter* painter, bool selected) override;
    };
} // namespace pulsar