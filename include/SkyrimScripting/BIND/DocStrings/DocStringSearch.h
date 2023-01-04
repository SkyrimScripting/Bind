#pragma once

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStrings/DocStringSearchQueue.h"

namespace SkyrimScripting::BIND::DocStrings {

    void Search(std::vector<BindingDefinition>& bindingDefinitions) {
        PopulateFileSearchQueue();
        ProcessFileSearchQueue(bindingDefinitions);
    }
}
