#pragma once
#include "MeshDrawPassProcessor.h"

namespace pulsar
{
    class BassPassProcessor : public MeshDrawPassProcessor
    {
        using base = MeshDrawPassProcessor;
    public:
        void Draw(const MeshDrawPassParams& params) override;
    };
} // namespace pulsar