#pragma once
#include "Pass.h"

namespace pulsar
{
    struct MeshDrawPassParams
    {

    };
    class MeshDrawPass : public Pass
    {
    public:
        virtual void Draw(const MeshDrawPassParams& params) {}
    };
} // namespace pulsar