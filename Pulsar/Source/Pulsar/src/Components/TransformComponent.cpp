#include "Components/TransformComponent.h"

#include "TransformUtil.h"

#include <Pulsar/Node.h>

namespace pulsar
{
    ObjectPtr<TransformComponent> TransformComponent::FindByName(string_view name) const
    {
        for (auto& comp : *m_children)
        {
            if (IsValid(comp))
            {
                if (comp->GetAttachedNode()->GetName() == name)
                {
                    return comp;
                }
            }
        }
        return nullptr;
    }

    ObjectPtr<TransformComponent> TransformComponent::FindByPath(string_view path) const
    {
        array_list<string> paths = StringUtil::Split(path, u8char('/'));

        TransformComponent_ref target = this->GetObjectHandle();

        for (auto& comp : *m_children)
        {
            auto targetComp = comp;
            bool success = true;
            for (size_t i = 0; i < paths.size(); i++)
            {
                if (auto find = target->FindByName(paths[i]))
                {
                    targetComp = find;
                }
                else
                {
                    success = false;
                    break;
                }
            }
            if (!success)
            {
                break;
            }
            return targetComp;
        }

        return nullptr;
    }

    Vector3f TransformComponent::GetWorldPosition()
    {
        return GetParentLocalToWorldMatrix() * m_position;
    }
    void TransformComponent::SetWorldPosition(Vector3f value)
    {
        SetPosition(GetParentWorldToLocalMatrix() * value);
    }
    void TransformComponent::Translate(Vector3f value)
    {
        SetPosition(GetPosition() + value);
    }

    Vector3f TransformComponent::GetWorldScale()
    {
        return GetParentLocalToWorldMatrix() * m_scale;
    }
    void TransformComponent::SetRotation(Quat4f rotation)
    {
        m_rotation = rotation;
        m_isDirtyMatrix = true;
        m_euler = rotation.GetEuler(jmath::EulerOrder::YXZ);
        MakeTransformChanged();
    }

    Vector3f TransformComponent::GetEuler() const
    {
        auto euler = m_rotation.GetEuler(jmath::EulerOrder::YXZ);
        return {euler.x, euler.y, euler.z};
    }

    void TransformComponent::SetEuler(Vector3f value)
    {
        m_euler = value;
        m_rotation = Quat4f::FromEuler(value, jmath::EulerOrder::YXZ);
        MakeTransformChanged();
    }
    void TransformComponent::RotateEuler(Vector3f value)
    {
        RotateQuat(Quat4f::FromEuler(value));
    }
    void TransformComponent::RotateQuat(Quat4f quat)
    {
        m_rotation *= quat;
        // m_rotation *= jmath::Inverse(quat) * quat * m_rotation;
        MakeTransformChanged();
    }
    Vector3f TransformComponent::GetForward()
    {
        return GetLocalToWorldMatrix() * Vector3f{0, 0, 1};
    }
    Vector3f TransformComponent::GetUp()
    {
        Matrix4f mat = GetLocalToWorldMatrix();
        mat[3] = Vector4f{0,0,0,1};
        auto up = mat * Vector3f{0, 1, 0};
        up.Normalized();
        return up;
    }
    Vector3f TransformComponent::GetRight()
    {
        Matrix4f mat = GetLocalToWorldMatrix();
        mat[3] = Vector4f{0,0,0,1};
        auto right = mat * Vector3f{1, 0, 0};
        right.Normalized();
        return right;
    }

    TransformComponent::TransformComponent()
        : m_localToWorldMatrix{1.f}, m_worldToLocalMatrix{1.f},
          m_scale(1.f, 1.f, 1.f)
    {
        m_flags |= OF_DontDestroy;
        m_children = mksptr(new List<ObjectPtr<TransformComponent>>);
    }
    void TransformComponent::OnTick(Ticker ticker)
    {
        base::OnTick(ticker);
        RebuildLocalToWorldMatrix();
    }

    static Matrix4f RootIdentMat{1};
    const Matrix4f& TransformComponent::GetParentLocalToWorldMatrix()
    {
        if (m_parent)
        {
            return m_parent->GetLocalToWorldMatrix();
        }
        else
        {
            return RootIdentMat;
        }
    }
    const Matrix4f& TransformComponent::GetParentWorldToLocalMatrix()
    {
        if (m_parent)
        {
            return m_parent->GetWorldToLocalMatrix();
        }
        else
        {
            return RootIdentMat;
        }
    }
    const Matrix4f& TransformComponent::GetLocalToWorldMatrix()
    {
        if (m_isDirtyMatrix)
            RebuildLocalToWorldMatrix();
        return m_localToWorldMatrix;
    }
    const Matrix4f& TransformComponent::GetWorldToLocalMatrix()
    {
        if (m_isDirtyMatrix)
            RebuildLocalToWorldMatrix();
        return m_worldToLocalMatrix;
    }

    void TransformComponent::RebuildLocalToWorldMatrix()
    {
        transutil::NewTRS(m_localToWorldMatrix, m_position, m_rotation, m_scale);
        if (m_parent)
        {
            m_localToWorldMatrix = m_parent->GetLocalToWorldMatrix() * m_localToWorldMatrix;
        }
        m_worldToLocalMatrix = Inverse(m_localToWorldMatrix);
        m_isDirtyMatrix = false;
    }
    void TransformComponent::PostEditChange(FieldInfo* info)
    {
        base::PostEditChange(info);
        if (info->GetName() == NAMEOF(m_euler))
        {
            SetEuler(m_euler);
        }
        else if(info->GetName() == NAMEOF(m_rotation))
        {
            SetRotation(m_rotation);
            //refresh data on editor inspector
            m_euler = m_rotation.GetEuler();
        }
    }

    void TransformComponent::MakeTransformChanged()
    {
        m_isDirtyMatrix = true;
        GetAttachedNode()->SendMessage(MessageId_OnChangedTransform);
    }

    void TransformComponent::SetParent(ObjectPtr<TransformComponent> parent)
    {
        if (m_parent == parent)
        {
            return;
        }

        if (parent == nullptr)
        {
            // set empty
            const auto it = std::ranges::find(*m_parent->m_children, THIS_REF);
            m_parent->m_children->erase(it);
            m_parent = nullptr;
        }
        else
        {
            m_parent = parent;
            m_parent->m_children->push_back(THIS_REF);
        }
    }
} // namespace pulsar