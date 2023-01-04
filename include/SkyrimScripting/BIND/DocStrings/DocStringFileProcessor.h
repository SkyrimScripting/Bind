#pragma once

#include <SkyrimScripting/Plugin.h>

#include <Champollion/Pex/FileReader.hpp>

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStrings/DocStringParser.h"
#include "SkyrimScripting/BIND/Util.h"

namespace SkyrimScripting::BIND::DocStrings::DocStringFileProcessor {

    void ProcessFile(std::filesystem::path path, std::vector<BindingDefinition>& bindingDefinitions) {
        auto scriptName = path.filename().replace_extension().string();
        auto pex = Pex::Binary();
        auto reader = Pex::FileReader(path.string());

        try {
            reader.read(pex);
        } catch (...) {
            logger::error("Champollion library failed to read .pex for script {}", scriptName);
            return;
        }

        std::string docString{};
        try {
            docString = pex.getObjects().front().getDocString().asString();
            if (!docString.empty())
                DocStrings::DocStringParser::ParseDocStringIntoBindings(scriptName, docString, bindingDefinitions);
        } catch (...) {
            logger::error("Champollion library failed to read {{docstring comment}} for script {}", scriptName);
            return;
        }
    }
}
