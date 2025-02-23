#pragma once
#include "RDGNode.h"

#include <memory>
#include <unordered_map>

namespace rdg
{
    class RDGGraph
    {
    public:
        template<typename T> requires std::is_base_of_v<RDGNode, T>
        std::shared_ptr<RDGNode> NewNode()
        {
            auto node = std::make_shared<T>();
            node->Initialize(this, GenerateId());
            return node;
        }
        void MakeLink(RDGNode::NodeId src, RDGNode::NodeId dst);
        int GetRefCount(RDGNode::NodeId id) const;
        bool IsCulled(RDGNode::NodeId id) const;
        void SetTarget(RDGNode::NodeId id);
        RDGNode::NodeId GetTarget() const { return m_target;}
    protected:
        static RDGNode::NodeId GenerateId();
    protected:
        RDGNode::NodeId m_target{};
        std::unordered_map<RDGNode::NodeId, std::shared_ptr<RDGNode>> m_nodes;
        std::unordered_map<RDGNode::NodeId, std::vector<RDGNode::NodeId>> m_dependencies;
    };
}