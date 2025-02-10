#pragma once
#include <PulsarEd/Editors/CommonPanel/PropertiesWindow.h>
#include <Pulsar/Node.h>

namespace pulsared
{

    class PropertiesNodePanel : public PropertiesPanel
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::PropertiesNodePanel, PropertiesPanel);
        APATITEED_PROPERTIES_PANEL(cltypeof<Node>());

    public:

        virtual void OnDrawImGui() override;

    private:
        bool m_debugMode{false};
    };
}