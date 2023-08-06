#pragma once
#include <Pulsar/Components/Component.h>
#include <PulsarEd/Assembly.h>
#include <Pulsar/Components/RendererComponent.h>

namespace pulsared
{
    class EditorComponent : public Component
    {
        CORELIB_DEF_TYPE(AssemblyObject_PulsarEd, pulsared::EditorComponent, Component);
    public:
        
    };

}