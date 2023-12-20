#include <CoreLib.Serialization/DataSerializer.h>
#include <Pulsar/Node.h>
#include <Pulsar/Scene.h>
#include <ranges>

namespace pulsar
{
    static void _BeginNode(Scene_ref scene, Node_ref node)
    {
        if (!IsValid(node->GetRuntimeOwnerScene()))
        {
            node->BeginNode(scene);
        }
        // for (auto& child : *node->GetTransform()->GetChildren())
        // {
        //     _BeginNode(scene, child->GetAttachedNode());
        // }
    }
    void Scene::OnAddNode(Node_ref node)
    {
        if (m_runtimeWorld)
        {
            _BeginNode(self_ref(), node);
        }
    }

    void Scene::OnRemoveNode(Node_ref node)
    {
    }

    void Scene::BeginScene(World* world)
    {
        m_runtimeWorld = world;
        for (auto& node : *m_nodes)
        {
            node->BeginNode(self_ref());
        }
    }
    void Scene::EndScene()
    {
        for (auto& node : *m_nodes)
        {
            node->EndNode();
        }

        m_runtimeWorld = nullptr;
    }
    void Scene::Tick(Ticker ticker)
    {
        for (auto& node : *GetNodes())
        {
            if (IsValid(node) && node->GetIsActive())
            {
                node->OnTick(ticker);
            }
        }
    }

    Scene::Scene()
    {
    }

    ObjectPtr<Scene> Scene::StaticCreate(string_view name)
    {
        auto self = mksptr(new Scene);
        self->Construct();
        self->SetIndexName(name);
        self->SetObjectFlags(self->GetObjectFlags() | OF_Persistent | OF_Instantiable);

        return self;
    }



    void Scene::OnDestroy()
    {
        base::OnDestroy();

        for (auto& node : *m_nodes)
        {
            DestroyObject(node);
        }
    }
} // namespace pulsar
