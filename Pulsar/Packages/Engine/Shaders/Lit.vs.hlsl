
#include "SurfaceVS.inc.hlsl"


void SurfaceVertexMain(inout InPixelAssembly v2f)
{
    v2f.WorldPosition += float4(0,0,0,0);
}
