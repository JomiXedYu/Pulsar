#include "rdg/RDGNode.h"

namespace rdg
{

    void RDGNode::Initialize(RDGGraph* graph, NodeId id)
    {
        m_graph = graph;
        m_nodeId = id;
    }
} // namespace rdg