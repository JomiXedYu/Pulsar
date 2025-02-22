#pragma once
#include "GFXInclude.h"

namespace gfx
{

    enum class GFXGpuProgramStageFlags : uint32_t
    {
        Vertex = 1,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        TessellationControl = 1 << 3,
        TessellationEvaluation = 1 << 4,
        Geometry = 1 << 5,
        Mesh = 1 << 6,
        VertexFragment = Vertex | Fragment,
    };
    inline const char* to_string(GFXGpuProgramStageFlags stage)
    {
        switch (stage)
        {
        case GFXGpuProgramStageFlags::Vertex: return "Vertex";
        case GFXGpuProgramStageFlags::Fragment: return "Fragment";
        case GFXGpuProgramStageFlags::VertexFragment: return "VertexFragment";
        case GFXGpuProgramStageFlags::Compute: return "Compute";
        case GFXGpuProgramStageFlags::TessellationControl: return "TessellationControl";
        case GFXGpuProgramStageFlags::TessellationEvaluation: return "TessellationEvaluation";
        case GFXGpuProgramStageFlags::Geometry: return "Geometry";
        case GFXGpuProgramStageFlags::Mesh: return "Mesh";
        }
        return nullptr;
    }

    class GFXGpuProgram
    {
    public:
        GFXGpuProgram() {}
        GFXGpuProgram(const GFXGpuProgram&) = delete;
        virtual ~GFXGpuProgram() {}

        virtual GFXGpuProgramStageFlags GetStage() const = 0;
    };
    GFX_DECL_SPTR(GFXGpuProgram);

}