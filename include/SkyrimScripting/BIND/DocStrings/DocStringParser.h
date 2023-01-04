#pragma once

#include <SkyrimScripting/Plugin.h>

#include <iomanip>

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/Reflection/PapyrusScript.h"
#include "SkyrimScripting/BIND/Util.h"

namespace SkyrimScripting::BIND::DocStrings::DocStringParser {

    std::mutex Mutex_AddBindingDefinitions;
    constexpr auto BIND_COMMENT_PREFIX = "!BIND";

    void AddDefaultBindingForScript(const Reflection::PapyrusScript& script,
                                    std::vector<BindingDefinition>& bindingDefinitions) {
        auto binding = BindingDefinition{.ScriptName = script.ScriptName};
        // if (script.BindParentType == "actor") {

        // } else if (script.BindParentType)
    }

    // BindingDefinition::BindType::Existing

    // !BIND <target>|<target> PROP=VAL PROP=VAL
    void ParseDocStringLineIntoBindings(const Reflection::PapyrusScript& script, const std::string& line,
                                        std::vector<BindingDefinition>& bindingDefinitions) {
        std::stringstream text(line);

        std::string bindPrefix;  // !BIND
        text >> std::quoted(bindPrefix);

        std::string targets;
        text >> std::quoted(targets);

        logger::info("{}", line);

        if (targets.empty()) {
            AddDefaultBindingForScript(script, bindingDefinitions);
            return;
        }
    }

    void ParseDocStringIntoBindings(const std::string& scriptName, const std::string& docString,
                                    std::vector<BindingDefinition>& bindingDefinitions) {
        std::vector<BindingDefinition> discoveredDefinitions;
        const Reflection::PapyrusScript& script = Reflection::GetScript(scriptName);
        std::string line;
        std::istringstream commentString{docString};
        while (std::getline(commentString, line)) {
            Util::Trim(line);
            if (line.starts_with(BIND_COMMENT_PREFIX)) {
                try {
                    ParseDocStringLineIntoBindings(script, line, discoveredDefinitions);
                } catch (...) {
                    logger::error("[ERROR] Failed to parse !BIND line: '{}'", line);
                }
            }
        }
        if (!discoveredDefinitions.empty()) {
            logger::info("FOUND DEFINITIONS!");
            // lock_guard and add
        }
    }
}
