#pragma once

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStringSearchQueue.h"


namespace SkyrimScripting::BIND::DocStringSearch {

    void Search(std::vector<BindingDefinition>& bindingDefinitions) {
        PopulateFileSearchQueue();
        ProcessFileSearchQueue(bindingDefinitions);
    }
}
