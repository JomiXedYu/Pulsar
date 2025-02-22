#pragma once

#include "Pulsar/Application.h"

#include "Component.h"
#include "MeshRendererComponent.h"
#include "RendererComponent.h"
#include <Pulsar/Assets/Material.h>
#include <Pulsar/Assets/StaticMesh.h>
#include <Pulsar/Node.h>
#include <Pulsar/ObjectBase.h>

namespace pulsar
{
    class Mesh;
    class Material;
    class ShaderPass;
    class StaticMeshRenderObject;

    class StaticMeshRendererComponent : public MeshRendererComponent, public IRendererComponent
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsar, pulsar::StaticMeshRendererComponent, MeshRendererComponent);
        CORELIB_CLASS_ATTR(new CategoryAttribute("Renderer"));
        CORELIB_IMPL_INTERFACES(IRendererComponent);
    public:
        SPtr<rendering::RenderObject> CreateRenderObject() override;
    public:
        void GetDependencies(array_list<ObjectHandle>& out) override;
        List_sp<RCPtr<Material>> GetMaterials() const { return this->m_materials; }

        void PostEditChange(FieldInfo* info) override;

        StaticMeshRendererComponent();

        bool HasBounds() const override { return true; }
        BoxSphereBounds3f GetBoundsWS() override;

        RCPtr<StaticMesh> GetStaticMesh() const { return m_staticMesh; }
        void SetStaticMesh(RCPtr<StaticMesh> staticMesh);

        RCPtr<StaticMesh> GetMaterial(int index) const;
        void   SetMaterial(int index, RCPtr<Material> material);
        size_t AddMaterial(RCPtr<Material> material = nullptr);
        void   RemoveMaterial(size_t index);
        size_t GetMaterialCount() const { return m_materialsSize; }

        void BeginComponent() override;
        void EndComponent() override;

        void OnReceiveMessage(MessageId id) override;

        int32_t GetRenderQueuePriority() const { return m_renderQueuePriority; }
        void SetRenderQueuePriority(int32_t value) { m_renderQueuePriority = value; }
    protected:
        void OnDependencyMessage(ObjectHandle inDependency, DependencyObjectState msg) override;
        void ResizeMaterials(size_t size);
        // void BeginListenMaterialStateChanged(size_t index);
        // void EndListenMaterialStateChanged(size_t index);
        void OnMaterialStateChanged();
        void OnTransformChanged() override;
        void OnMeshChanged();
        void OnMaterialChanged();
    protected:
        CORELIB_REFL_DECL_FIELD(m_materials, new ListItemAttribute(cltypeof<Material>()));
        List_sp<RCPtr<Material>> m_materials;

        size_t m_materialsSize = 0;

        CORELIB_REFL_DECL_FIELD(m_staticMesh);
        RCPtr<StaticMesh> m_staticMesh;

        CORELIB_REFL_DECL_FIELD(m_isCastShadow);
        bool m_isCastShadow = true;

        CORELIB_REFL_DECL_FIELD(m_renderQueuePriority);
        int32_t m_renderQueuePriority{1000};

        CORELIB_REFL_DECL_FIELD(m_boundsScale, new RangePropertyAttribute(0.1f, 10.f));
        float m_boundsScale = 1;

        SPtr<StaticMeshRenderObject> m_renderObject;

    private:

    };
    DECL_PTR(StaticMeshRendererComponent);
}