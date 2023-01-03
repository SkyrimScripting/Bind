#pragma once

namespace SkyrimScripting::BIND {

    struct BindingDefinition {
        enum class PropertyType { Bool, Int, Float, String, Form };

        struct PropertySetter {
            std::string Name;
            PropertyType Type;
            bool BoolValue;
            int IntValue;
            float FloatValue;
            std::string StringValue;
            RE::TESForm* FormValue;
        };

        enum class BindType { Existing, NewQuest, NewObject };

        std::string Identifier;
        BindType Type;
        std::string ScriptName;
        RE::FormID FormID;
        std::string EditorID;
        std::string PluginFile;
        std::vector<PropertySetter> Properties;
    };
}
