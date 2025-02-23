#pragma once
#include <string_view>

namespace rdg
{
    using RDGTextureHandle = int;

    class GraphBuilder final
    {
    public:
        GraphBuilder(const GraphBuilder&) = delete;
        GraphBuilder(GraphBuilder&&) = delete;
        GraphBuilder& operator=(const GraphBuilder&) = delete;
        GraphBuilder& operator=(GraphBuilder&&) = delete;

        RDGTextureHandle CreateTexture(std::string_view name);

    };
}