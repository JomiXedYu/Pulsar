#include "rdg/RDGGraph.h"

namespace rdg
{


    void RDGGraph::MakeLink(RDGNode::NodeId src, RDGNode::NodeId dst)
    {
        m_dependencies[src].push_back(dst);
    }
    int RDGGraph::GetRefCount(RDGNode::NodeId id) const
    {
        return m_dependencies.at(id).size();
    }
    bool RDGGraph::IsCulled(RDGNode::NodeId id) const
    {
        return GetRefCount(id) == 0;
    }
    void RDGGraph::SetTarget(RDGNode::NodeId id)
    {
        m_target = id;
    }

    RDGNode::NodeId RDGGraph::GenerateId()
    {
        static RDGNode::NodeId id = 0;
        ++id;
        return id;
    }
} // namespace rdg