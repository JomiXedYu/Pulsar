#pragma once
#include "MeshDrawPass.h"

namespace pulsar
{
    class ShadowPass : public MeshDrawPass
    {
        using base = MeshDrawPass;
    public:
        void Draw(const MeshDrawPassParams& params) override;

    };
} // namespace pulsar