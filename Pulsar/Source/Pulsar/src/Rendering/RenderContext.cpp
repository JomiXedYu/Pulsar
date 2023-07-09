#include "Rendering/RenderContext.h"
#include <stack>

namespace pulsar
{
    static std::stack<CameraComponent_sp> _cameras;

    void RenderContext::PushCamera(CameraComponent_sp cam)
    {
        _cameras.push(cam);
    }
    void RenderContext::PopCamera()
    {
        _cameras.pop();
    }

    CameraComponent_sp RenderContext::GetCurrentCamera()
    {
        return _cameras.top();
    }
}