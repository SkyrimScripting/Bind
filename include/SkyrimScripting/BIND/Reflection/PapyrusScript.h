#pragma once

#include <string>

#include "SkyrimScripting/BIND/Util.h"

namespace SkyrimScripting::BIND::Reflection {

    enum class BindParentType { Form, Quest, Actor, ObjectReference };

    constexpr auto BIND_PARENT_TYPE_NAMES = {"form", "quest", "actor", "objectreference"};

    bool IsBindParentType(const std::string& scriptName) {
        auto comparison = std::string{scriptName};
        Util::LowerCase(comparison);
        return std::find(BIND_PARENT_TYPE_NAMES.begin(), BIND_PARENT_TYPE_NAMES.end(), comparison) !=
               BIND_PARENT_TYPE_NAMES.end();
    }

    class PapyrusScript {
    public:
        std::string ScriptName;
        BindParentType BindParentType;
    };

    std::unordered_map<std::string, PapyrusScript> _scriptCache;

    PapyrusScript& GetScript(const std::string& scriptName) {
        if (_scriptCache.contains(scriptName)) return _scriptCache.at(scriptName);

        auto script = PapyrusScript{.ScriptName = scriptName};

        auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> typeInfo;
        vm->GetScriptObjectType(scriptName, typeInfo);

        std::string parentName;
        auto parent = typeInfo->parentTypeInfo;
        while (parent && !IsBindParentType(parentName)) {
            parentName = parent->GetName();
            parent = parent->parentTypeInfo;
        }

        Util::LowerCase(parentName);

        if (parentName == "form") {
            script.BindParentType = BindParentType::Form;
        } else if (parentName == "quest") {
            script.BindParentType = BindParentType::Quest;
        } else if (parentName == "objectreference") {
            script.BindParentType = BindParentType::ObjectReference;
        } else if (parentName == "actor") {
            script.BindParentType = BindParentType::Actor;
        }

        _scriptCache.insert_or_assign(scriptName, script);
        return _scriptCache.at(scriptName);
    }
}
