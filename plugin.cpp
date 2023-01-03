#include <SkyrimScripting/Plugin.h>

#include "SkyrimScripting/BIND/Bind.h"
#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStringSearch.h"

namespace SkyrimScripting::BIND {

    std::atomic<bool> BindingsApplied;
    std::vector<BindingDefinition> BindingDefinitions;

    EventHandlers {
        // To start off with, this only ever runs once.
        // In future versions, we will acknowledge save/load games, etc.
        On<RE::TESCellFullyLoadedEvent>([](const RE::TESCellFullyLoadedEvent*) {
            if (!BindingsApplied.exchange(true)) BindAll(BindingDefinitions);
        });
    }

    OnInit {
        spdlog::set_pattern("%v");
        DocStringSearch::Search(BindingDefinitions);
    }
}
