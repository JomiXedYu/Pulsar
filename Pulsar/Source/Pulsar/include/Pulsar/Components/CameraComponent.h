#pragma once
#include "Component.h"
#include <Pulsar/Assets/RenderTexture.h>

namespace pulsar
{
    CORELIB_DEF_ENUM(AssemblyObject_pulsar, pulsar,
        CameraProjectionMode,
        Perspective,
        Orthographic);
}

CORELIB_DECL_BOXING(pulsar::CameraProjectionMode, pulsar::BoxingCameraProjectionMode);

namespace pulsar
{
    class CameraComponent : public Component
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::CameraComponent, Component);
    public:
        ~CameraComponent() override = default;
        void Render();
    public:
        Matrix4f GetViewMat() const;
        Matrix4f GetProjectionMat() const;

        virtual void BeginComponent() override;
        virtual void EndComponent() override;

        virtual void PostEditChange(FieldInfo* info) override;

        void OnTick(Ticker ticker) override;
    public:
        float GetFOV() const { return m_fov; }
        void  SetFOV(float value) { m_fov = value; }
        float GetNear() const { return m_near; }
        void  SetNear(float value) { m_near = value; }
        float GetFar() const { return m_far; }
        void  SetFar(float value) { m_far = value; }
        Color4f  GetBackgroundColor() const { return m_backgroundColor; }
        void     SetBackgroundColor(const Color4f& value);
        CameraProjectionMode  GetProjectionMode() const { return m_projectionMode; }
        void                  SetProjectionMode(CameraProjectionMode mode);
        const RenderTexture_ref&  GetRenderTarget() const { return m_renderTarget; }
        void                      SetRenderTarget(const RenderTexture_ref& value);

    private:
        void UpdateRTBackgroundColor();
        void UpdateRT();
    protected:

        CORELIB_REFL_DECL_FIELD(m_fov);
        float m_fov{};

        CORELIB_REFL_DECL_FIELD(m_near);
        float m_near{};

        CORELIB_REFL_DECL_FIELD(m_far);
        float m_far{};

        CORELIB_REFL_DECL_FIELD(m_projectionMode);
        CameraProjectionMode m_projectionMode{};

        CORELIB_REFL_DECL_FIELD(m_backgroundColor);
        Color4f m_backgroundColor{};

        CORELIB_REFL_DECL_FIELD(m_renderTarget);
        RenderTexture_ref m_renderTarget;
#ifdef WITH_EDITOR
        CORELIB_REFL_DECL_FIELD(debug_view_mat, new ReadOnlyPropertyAttribute);
        Matrix4f debug_view_mat;
#endif

    };
    DECL_PTR(CameraComponent);
}