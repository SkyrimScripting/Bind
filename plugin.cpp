#include <SkyrimScripting/Plugin.h>

#include "SkyrimScripting/BIND/Bind.h"
#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStrings/DocStringSearch.h"

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
        // FIND ALL DOC STRINGS ---> vector<string> (no VM info available here)
    }

    OnDataLoaded {
        // PROCESS ALL DOC STRINGS, vector<string> ---> vector<BindingDefinition> (using the VM's info)
        //
        //
        //
        DocStrings::Search(BindingDefinitions);
    }

    // OnGameLoad {
    //      EVALUATE ALL BINDINGS
    // }
}
