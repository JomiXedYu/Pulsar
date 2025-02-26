#pragma once
#include "ObjectBase.h"

namespace pulsar
{
    class CameraComponent;
    class CameraManager
    {
    public:

        void AddCamera(const ObjectPtr<CameraComponent>& camera, bool isMainCamera = false);
        void RemoveCamera(const ObjectPtr<CameraComponent>& camera);

        void SetMainCamera(const ObjectPtr<CameraComponent>& camera);
        ObjectPtr<CameraComponent> GetMainCamera() const { return m_mainCamera; }

        const array_list<ObjectPtr<CameraComponent>>& GetCameras() const { return m_cameras; }

    protected:
        array_list<ObjectPtr<CameraComponent>> m_cameras;
        ObjectPtr<CameraComponent> m_mainCamera;

    };
}