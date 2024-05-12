#pragma once

#include "Component.h"

namespace pulsar
{
    enum class TransformCoordSystem
    {
        Local,
        Global
    };
    class TransformComponent : public Component
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::TransformComponent, Component);
        CORELIB_CLASS_ATTR(new AbstractComponentAttribute);
    public:
        void Serialize(ComponentSerializer* s) override;
        
        ObjectPtr<TransformComponent> FindByName(string_view name) const;
        ObjectPtr<TransformComponent> FindByPath(string_view path) const;
        ObjectPtr<TransformComponent> GetParent() const { return m_parent; }
        void SetParent(ObjectPtr<TransformComponent> parent);

        const List_sp<ObjectPtr<TransformComponent>>& GetChildren() const noexcept { return m_children; }
        size_t GetChildCount() const noexcept { return m_children->size(); }
        ObjectPtr<TransformComponent> GetChild(int32_t index) const noexcept { return m_children->at(index); }

        void SetPosition(Vector3f value) { m_position = value; MakeTransformChanged(); }
        Vector3f GetPosition() const noexcept { return m_position; }
        Vector3f GetWorldPosition();
        void SetWorldPosition(Vector3f value);
        void Translate(Vector3f value);

        void SetScale(Vector3f value) { m_scale = value; MakeTransformChanged(); }
        Vector3f GetScale() const noexcept { return m_scale; }
        Vector3f GetWorldScale();

        void SetRotation(Quat4f rotation);
        Quat4f GetRotation() const { return m_rotation; }
        Vector3f GetEuler() const;
        void SetEuler(Vector3f value);
        void RotateEuler(Vector3f value);
        void RotateQuat(Quat4f quat);


        void TranslateRotateEuler(Vector3f pos, Vector3f euler);

        Vector3f GetForward();
        Vector3f GetUp();
        Vector3f GetRight();

        TransformComponent();
        void BeginComponent() override;

        void OnTick(Ticker ticker) override;

        const Matrix4f& GetParentLocalToWorldMatrix();
        const Matrix4f& GetParentWorldToLocalMatrix();
        const Matrix4f& GetLocalToWorldMatrix();
        const Matrix4f& GetWorldToLocalMatrix();
    protected:
        void OnTransformChanged() override;
        void RebuildLocalToWorldMatrix();
        void PostEditChange(FieldInfo* info) override;
    public:
        void MakeTransformChanged();
    protected:
        CORELIB_REFL_DECL_FIELD(m_localToWorldMatrix, new NoSerializableAttribtue, new DebugPropertyAttribute, new ReadOnlyPropertyAttribute);
        Matrix4f m_localToWorldMatrix;
        Matrix4f m_worldToLocalMatrix;
        bool m_isDirtyMatrix = false;

        CORELIB_REFL_DECL_FIELD(m_position);
        Vector3f m_position{};
        CORELIB_REFL_DECL_FIELD(m_euler);
        Vector3f m_euler{};
        CORELIB_REFL_DECL_FIELD(m_scale);
        Vector3f m_scale;
        CORELIB_REFL_DECL_FIELD(m_rotation, new DebugPropertyAttribute, new ReadOnlyPropertyAttribute);
        Quat4f m_rotation{};

        CORELIB_REFL_DECL_FIELD(m_children, new HidePropertyAttribute);
        List_sp<ObjectPtr<TransformComponent>> m_children{};

        CORELIB_REFL_DECL_FIELD(m_parent, new HidePropertyAttribute);
        ObjectPtr<TransformComponent> m_parent{};
    };
    DECL_PTR(TransformComponent);
}