#pragma once

#include <CoreLib/Events.hpp>
#include <Pulsar/ObjectBase.h>
#include "AppInstance.h"
#include "Math.h"

namespace pulsar
{
    class Application
    {
    private:
        Application() = delete;
    public:
        static AppInstance* inst();
        //start
        static int Exec(AppInstance* instance, string_view title, Vector2f size);

    };

}