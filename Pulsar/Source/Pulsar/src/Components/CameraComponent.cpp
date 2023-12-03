#include "Components/CameraComponent.h"
#include <Pulsar/Application.h>
#include <Pulsar/AppInstance.h>
#include <Pulsar/TransformUtil.h>
#include <Pulsar/Node.h>
#include "Scene.h"
#include "Logger.h"

namespace pulsar
{
    void CameraComponent::Render()
    {

    }

    Matrix4f CameraComponent::GetViewMat()
    {
        return jmath::Inverse(GetAttachedNode()->GetTransform()->GetLocalToWorldMatrix());
    }

    Matrix4f CameraComponent::GetProjectionMat()
    {
        const Vector2f& size = this->m_renderTarget ? this->m_renderTarget->GetSize2df() : this->size_;
        Matrix4f ret{1};
        if (this->cameraMode == CameraMode::Perspective)
        {
            math::Perspective(ret,
                math::Radians(this->fov),
                size.x / size.y,
                this->near,
                this->far);
        }
        else
        {
            math::Ortho(ret,
                0.0f,
                size.x,
                0.0f,
                size.y,
                this->near,
                this->far);
        }
        return ret;
    }
    Matrix4f CameraComponent::LookAtRH(Vector3f const& eye, Vector3f const& center, Vector3f const& up)
    {
        return {};
    }

    void CameraComponent::PostEditChange(FieldInfo* info)
    {
        base::PostEditChange(info);
        if (info->GetName() == "m_backgroundColor")
        {
            UpdateRTBackgroundColor();
        }
    }
    void CameraComponent::SetBackgroundColor(const Color4f& value)
    {
        m_backgroundColor = value;
        UpdateRTBackgroundColor();
    }

    void CameraComponent::UpdateRTBackgroundColor()
    {
        if (!m_renderTarget)
        {
            return;
        }
        auto rt0 = m_renderTarget->GetGfxRenderTarget0().get();
        rt0->ClearColor[0] = m_backgroundColor.r;
        rt0->ClearColor[1] = m_backgroundColor.g;
        rt0->ClearColor[2] = m_backgroundColor.b;
        rt0->ClearColor[3] = m_backgroundColor.a;
    }
    void CameraComponent::UpdateRT()
    {
        UpdateRTBackgroundColor();
    }

    void CameraComponent::SetRenderTarget(const RenderTexture_ref& value)
    {
        m_renderTarget = value;
        UpdateRT();
    }

    void CameraComponent::LookAt(const Vector3f& pos)
    {
        //transutil::LookAt(this->GetAttachedNode()->get_world_position(), pos, transutil::Vector3Up());
    }

    void CameraComponent::BeginComponent()
    {
        base::BeginComponent();
        GetAttachedNode()->GetRuntimeOwnerScene()->GetWorld()->GetCameraManager().AddCamera(THIS_REF);
    }

    void CameraComponent::EndComponent()
    {
        base::EndComponent();
        GetAttachedNode()->GetRuntimeOwnerScene()->GetWorld()->GetCameraManager().RemoveCamera(THIS_REF);
    }
}
