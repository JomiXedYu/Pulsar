#pragma once
#include "RenderObject.h"

namespace pulsar::rendering
{
    class SceneRenderer
    {

    public:
        void            AddRenderObject_RenderThread(const RenderObject_sp& renderObject);
        void            RemoveRenderObject_RenderThread(RenderObject_rsp renderObject);


    };
}