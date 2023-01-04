#include <SkyrimScripting/Plugin.h>

// #include "SkyrimScripting/BIND/Bind.h"               // Rename?
// #include "SkyrimScripting/BIND/BindingDefinition.h"  // Make this BindingDefinitions.h
// #include "SkyrimScripting/BIND/BindingDefinitions.h"
// #include "SkyrimScripting/BIND/DocStrings/DocStringSearch.h"  // make this DocStrings.h

namespace SkyrimScripting::BIND {

    // // These are the Binding Definitions discovered from doc strings (and transformed into BindingDefinition
    // structs). std::vector<BindingDefinition> Definitions;

    // // All of the discovered {!BIND} doc strings.
    // std::vector<std::string

    // // This is a Queue of file paths which is searched for {!BIND} doc strings.
    // std::queue<std::filesystem::path> FilePathsToSearchForDocStrings;

    // // Did the search for doc strings in files complete? If so, kDataLoaded, it will transform into
    // BindingDefinitions. std::atomic<bool> IsDocStringSearchComplete;

    // // Has kDataLoaded occurred? If so, when DocString search completes, it will transform into BindingDefinitions.
    // std::atomic<bool> HasDataLoaded;

    // OnInit { DocStrings::BeginSearch(); }

    // OnDataLoaded { BindingDefinitions::ConvertDocStringsToBindingDefinitions(); }
}
