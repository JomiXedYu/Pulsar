#pragma once
#include <cstdint>

namespace rdg
{
    class RDGGraph;

    class RDGNode
    {
    public:
        using NodeId = uint64_t;

        RDGNode() {}
        virtual ~RDGNode() = default;
        RDGNode(const RDGNode&) = delete;
        RDGNode(RDGNode&&) = delete;
        RDGNode& operator=(const RDGNode&) = delete;

        void Initialize(RDGGraph* graph, NodeId id);

        NodeId GetId() const { return m_nodeId; }
        virtual const char* GetName() const { return "Node"; }

    protected:
        RDGGraph* m_graph = nullptr;
        NodeId m_nodeId = 0;
    };
}