#pragma once

#include <SkyrimScripting/Plugin.h>

#include "BIND/BindingDefinition.h"
#include "BIND/Scan.h"

namespace BIND {
    class System {
        System() = default;
        ~System() = default;
        System(const System&) = delete;
        System(System&&) = delete;
        System& operator=(const System&) = delete;
        System& operator=(System&&) = delete;

        // std::vector<BindingDefinition> _registeredBindingDefinitions;

    public:
        static System& GetSingleton() {
            static System singleton;
            return singleton;
        }

        void Startup() { Scan::ScanForDocStringsInScripts(); }
    };
}
