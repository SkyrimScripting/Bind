// This whole plugin intentionally implemented in 1 file (for a truly minimalistic v1 of BIND) - Under 250 LOC

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <spdlog/sinks/basic_file_sink.h>

#define _Log_(...) SKSE::log::info(__VA_ARGS__)

namespace SkyrimScripting::Bind {

    bool AlreadyRanForThisLoadGame = false;
    RE::TESForm* DefaultBaseFormForCreatingObjects;
    RE::TESObjectREFR* LocationForPlacingObjects;
    std::string FilePath;
    unsigned int LineNumber;
    std::string ScriptName;
    RE::BSScript::IVirtualMachine* vm;
    constexpr auto BINDING_FILES_FOLDER_ROOT = "Data/Scripts/Bindings";
    std::unordered_map<std::string, RE::FormID> GeneratedQuestFormIDs;

    void LowerCase(std::string& text) {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }
    RE::TESForm* LookupFormID(RE::FormID formID) {
        auto* form = RE::TESForm::LookupByID(formID);
        if (form) return form;
        else _Log_("Error [{}:{}] ({}) Form ID '{:x}' does not exist", FilePath, LineNumber, ScriptName, formID);
        return {};
    }
    RE::TESForm* LookupEditorID(const std::string& editorID) {
        auto* form = RE::TESForm::LookupByEditorID(editorID);
        if (form) return form;
        else _Log_("Error [{}:{}] ({}) Form Editor ID '{}' does not exist (Are you using po3 Tweaks?)", FilePath, LineNumber, ScriptName, editorID);
        return {};
    }
    bool FormHasScriptAttached(RE::VMHandle handle, const std::string scriptName) {
        RE::BSScript::Internal::VirtualMachine* _vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        RE::BSSpinLockGuard lock{_vm->attachedScriptsLock};
        auto found = _vm->attachedScripts.find(handle);
        if (found != _vm->attachedScripts.end()) {
            RE::BSFixedString bsScriptName{scriptName};
            auto& scripts = found->second;
            for (auto& script : scripts)
                if (script->type->name == bsScriptName) return true;
        }
        return false;
    }
    void Bind_Form(RE::TESForm* form) {
        auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(form->GetFormType(), form);
        if (FormHasScriptAttached(handle, ScriptName)) {
            _Log_("Script {} already attached to form {:x}", ScriptName, form->GetFormID());
        } else {
            RE::BSTSmartPointer<RE::BSScript::Object> object;
            vm->CreateObject(ScriptName, object);
            vm->GetObjectBindPolicy()->BindObject(object, handle);
            _Log_("Bound script {} to form {:x}", ScriptName, form->GetFormID());
        }
    }
    void Bind_GeneratedObject(RE::TESForm* baseForm = nullptr) {
        if (!baseForm) baseForm = DefaultBaseFormForCreatingObjects;
        auto niPointer = LocationForPlacingObjects->PlaceObjectAtMe(skyrim_cast<RE::TESBoundObject*, RE::TESForm>(baseForm), false);
        if (niPointer) Bind_Form(niPointer.get());
        else _Log_("Error [{}:{}] ({}) Could not generate object ({:x}, {})", FilePath, LineNumber, ScriptName, baseForm->GetFormID(), baseForm->GetName());
    }
    void Bind_GeneratedObject_BaseEditorID(const std::string& baseEditorId) {
        auto* form = LookupEditorID(baseEditorId);
        if (form) Bind_GeneratedObject(form);
    }
    void Bind_GeneratedObject_BaseFormID(RE::FormID baseFormID) {
        auto* form = LookupFormID(baseFormID);
        if (form) Bind_GeneratedObject(form);
    }
    void Bind_GeneratedQuest(std::string editorID = "") {
        if (!editorID.empty() && GeneratedQuestFormIDs.contains(editorID)) {
            _Log_("Script {} already attached to quest {:x}", ScriptName, GeneratedQuestFormIDs[editorID]);
            return;
        }
        RE::TESQuest* form = nullptr;
        if (!editorID.empty()) form = RE::TESForm::LookupByEditorID<RE::TESQuest>(editorID);
        if (!form) {
            form = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESQuest>()->Create();
            if (!editorID.empty()) form->SetFormEditorID(editorID.c_str());
        }
        if (!editorID.empty()) GeneratedQuestFormIDs[editorID] = form->GetFormID();
        Bind_Form(form);
    }
    void Bind_FormID(RE::FormID formID) {
        auto* form = LookupFormID(formID);
        if (form) Bind_Form(form);
    }
    void Bind_EditorID(const std::string& editorID) {
        auto* form = LookupEditorID(editorID);
        if (form) Bind_Form(form);
    }
    bool IsUnderstoodScriptParentType(std::string parentTypeName) {
        LowerCase(parentTypeName);
        if (parentTypeName == "quest" || parentTypeName == "actor" || parentTypeName == "objectreference") return true;
        return false;
    }
    void AutoBindBasedOnScriptExtends() {
        RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> typeInfo;
        vm->GetScriptObjectType(ScriptName, typeInfo);
        std::string parentName;
        auto parent = typeInfo->parentTypeInfo;
        while (parent && !IsUnderstoodScriptParentType(parentName)) {
            parentName = parent->GetName();
            parent = parent->parentTypeInfo;
        }
        if (parentName.empty()) {
            _Log_("Error [{}:{}] ({}) Cannot auto-bind to a script which does not `extends` anything", FilePath, LineNumber, ScriptName);
            return;
        }
        LowerCase(parentName);
        if (parentName == "quest") Bind_GeneratedQuest();
        else if (parentName == "actor") Bind_FormID(0x14);
        else if (parentName == "objectreference") Bind_GeneratedObject();
        else _Log_("Error [{}:{}] ({}) No default BIND behavior available for script which `extends` {}", FilePath, LineNumber, ScriptName, parentName);
    }
    void ProcessBindingLine(std::string line) {
        if (line.empty()) return;
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::istringstream lineStream{line};
        lineStream >> ScriptName;
        if (ScriptName.empty() || ScriptName.starts_with('#') || ScriptName.starts_with("//")) return;
        _Log_("[{}:{}] {}", FilePath, LineNumber, line);
        if (!vm->TypeIsValid(ScriptName)) {
            _Log_("Error [{}:{}] Script '{}' does not exist", FilePath, LineNumber, ScriptName);
            return;
        }
        std::string bindTarget;
        lineStream >> bindTarget;
        LowerCase(bindTarget);
        if (bindTarget.empty()) {
            AutoBindBasedOnScriptExtends();
            return;
        } else if (bindTarget.starts_with("0x")) Bind_FormID(std::stoi(bindTarget, 0, 16));
        else if (bindTarget == "$player") Bind_FormID(0x14);
        else if (bindTarget.starts_with("$quest(")) Bind_GeneratedQuest(bindTarget.substr(7, bindTarget.size() - 8));
        else if (bindTarget == "$quest") Bind_GeneratedQuest();
        else if (bindTarget == "$object") Bind_GeneratedObject();
        else if (bindTarget.starts_with("$object(0x")) Bind_GeneratedObject_BaseFormID(std::stoi(bindTarget.substr(8, bindTarget.size() - 9), 0, 16));
        else if (bindTarget.starts_with("$object(")) Bind_GeneratedObject_BaseEditorID(bindTarget.substr(8, bindTarget.size() - 9));
        else Bind_EditorID(bindTarget);
    }
    void ProcessBindingFile() {
        _Log_("Reading Binding File: {}", FilePath);
        LineNumber = 1;
        std::string line;
        std::ifstream file{FilePath, std::ios::in};
        while (std::getline(file, line)) {
            LineNumber++;
            try {
                ProcessBindingLine(line);
            } catch (...) { _Log_("Error [{}:{}]", FilePath, LineNumber); }
        }
        file.close();
    }
    void ProcessAllBindingFiles() {
        if (!std::filesystem::is_directory(BINDING_FILES_FOLDER_ROOT)) {
            _Log_("{} folder not found", BINDING_FILES_FOLDER_ROOT);
            return;
        }
        for (auto& file : std::filesystem::directory_iterator(BINDING_FILES_FOLDER_ROOT)) {
            FilePath = file.path().string();
            ProcessBindingFile();
        }
    }
    void Start() {
        if (AlreadyRanForThisLoadGame) return;
        else AlreadyRanForThisLoadGame = true;
        GeneratedQuestFormIDs.clear();
        vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        DefaultBaseFormForCreatingObjects = RE::TESForm::LookupByID(0xAEBF3);            // DwarvenFork
        LocationForPlacingObjects = RE::TESForm::LookupByID<RE::TESObjectREFR>(0xBBCD1); // The chest in WEMerchantChests
        ProcessAllBindingFiles();
    }
    class CellFullyLoadedEvent : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
    public:
        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent*, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* source) override {
            Start();
            return RE::BSEventNotifyControl::kContinue;
        }
    };
    void SetupLog() {
        auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
        auto logFilePath = *SKSE::log::log_directory() / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
        spdlog::set_default_logger(std::move(loggerPtr));
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::info);
        spdlog::set_pattern("%v");
    }
    void SaveCallback(SKSE::SerializationInterface* save) {
        if (save->OpenRecord('QIDS', 1)) {
            save->WriteRecordData(GeneratedQuestFormIDs.size());
            for (auto& [editorID, formID] : GeneratedQuestFormIDs) {
                save->WriteRecordData(editorID);
                save->WriteRecordData(formID);
            }
        }
    }

    void LoadCallback(SKSE::SerializationInterface* save) {
        uint32_t type;
        uint32_t version;
        uint32_t length;
        while (save->GetNextRecordInfo(type, version, length)) {
            if (type == 'QIDS') {
                size_t size;
                save->ReadRecordData(size);
                for (size_t i = 0; i < size; ++i) {
                    std::string editorID;
                    save->ReadRecordData(editorID);
                    RE::FormID formID;
                    save->ReadRecordData(formID);
                    GeneratedQuestFormIDs[editorID] = formID;
                }
            }
        }
    }

    CellFullyLoadedEvent CellFullyLoadedEventSink;
    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();
        SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
            switch (message->type) {
            case SKSE::MessagingInterface::kNewGame:
            case SKSE::MessagingInterface::kPostLoadGame:
                AlreadyRanForThisLoadGame = false;
                Start();
                break;
            default: break;
            }
        });
        RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESCellFullyLoadedEvent>(&CellFullyLoadedEventSink);
        auto serialization = SKSE::GetSerializationInterface();
        serialization->SetUniqueID('BIND');
        serialization->SetSaveCallback(SaveCallback);
        serialization->SetLoadCallback(LoadCallback);
        return true;
    }
}
