#pragma once
#include "MeshDrawPassProcessor.h"
#include <rdg/RDGGraphBuilder.h>
#include <rdg/RDGPassNode.h>

namespace pulsar
{
    class DepthPass : public rdg::RDGPassNode
    {
        void Build(rdg::GraphBuilder& builder)
        {
            builder.CreateTexture("depth");
        }
        void Compile();
        void Execute();
    };

    class DepthPassProcessor : public MeshDrawPassProcessor
    {

    };
}