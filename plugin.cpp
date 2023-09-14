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

    void LowerCase(std::string& text) {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }
    RE::TESForm* LookupFormID(RE::FormID formID) {
        auto* form = RE::TESForm::LookupByID(formID);
        if (form)
            return form;
        else
            _Log_("[BIND] Error [{}:{}] ({}) Form ID '{:x}' does not exist", FilePath, LineNumber, ScriptName, formID);
        return {};
    }
    RE::TESForm* LookupEditorID(const std::string& editorID) {
        auto* form = RE::TESForm::LookupByEditorID(editorID);
        if (form)
            return form;
        else
            _Log_("[BIND] Error [{}:{}] ({}) Form Editor ID '{}' does not exist (Are you using po3 Tweaks?)", FilePath, LineNumber, ScriptName, editorID);
        return {};
    }
    bool FormHasScriptAttached(RE::VMHandle handle, const std::string scriptName) {
        RE::BSScript::Internal::VirtualMachine* _vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        RE::BSSpinLockGuard lock{_vm->attachedScriptsLock};
        auto found = _vm->attachedScripts.find(handle);
        if (found != _vm->attachedScripts.end()) {
            RE::BSFixedString bsScriptName{scriptName};
            auto& scripts = found->second;
            for (auto& script : scripts) {
                if (script->type->name == bsScriptName) return true;
            }
        }
        return false;
    }
    void Bind_Form(RE::TESForm* form) {
        if (!form) {
            _Log_("[BIND] Bind_Form called with nullptr (no form)");
            return;
        }
        auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(form->GetFormType(), form);
        if (FormHasScriptAttached(handle, ScriptName)) {
            _Log_("[BIND] Script {} already attached to form {:x} ({})", ScriptName, form->GetFormID(), form->GetName());
        } else {
            RE::BSTSmartPointer<RE::BSScript::Object> object;
            vm->CreateObject(ScriptName, object);
            vm->GetObjectBindPolicy()->BindObject(object, handle);
            auto* reference = form->As<RE::TESObjectREFR>();
            if (reference)
                _Log_("[BIND] Bound script {} to form {:x} {} (base type: {:x} {})", ScriptName, form->GetFormID(), form->GetName(), reference->GetBaseObject()->GetFormID(), reference->GetBaseObject()->GetName());
            else
                _Log_("[BIND] Bound script {} to form {:x} {}", ScriptName, form->GetFormID(), form->GetName());
        }
    }
    void PrintNearbyObjects() {
        _Log_("Printing nearby objects close to {} ({:x}) {}", LocationForPlacingObjects->GetFormEditorID(), LocationForPlacingObjects->GetFormID(), LocationForPlacingObjects->GetName());
        // void ForEachReferenceInRange(TESObjectREFR* a_origin, float a_radius, std::function<BSContainer::ForEachResult(TESObjectREFR& a_ref)> a_callback);
        RE::TES::GetSingleton()->ForEachReferenceInRange(LocationForPlacingObjects, 100, [](RE::TESObjectREFR& ref) {
            _Log_("[BIND] Nearby Object {:x} {}", ref.GetFormID(), ref.GetFormEditorID());
            return RE::BSContainer::ForEachResult::kContinue;
        });
    }
    void Bind_GeneratedObject(RE::TESForm* baseForm = nullptr, const std::string& newObjectEditorID = "") {
        RE::TESObjectREFR* objectRef = nullptr;
        if (objectRef) {
            _Log_("[BIND] Object already generated {:x} {}", objectRef->GetFormID(), objectRef->GetFormEditorID());
            Bind_Form(objectRef);
            return;
        }
        if (!baseForm) baseForm = DefaultBaseFormForCreatingObjects;  // By default, simply puts a fork next to the WEMerchantChest in the WEMerchantChests cell. Forking awesome.
        auto niPointer = LocationForPlacingObjects->PlaceObjectAtMe(skyrim_cast<RE::TESBoundObject*, RE::TESForm>(baseForm), false);

        //
        PrintNearbyObjects();
        //

        if (niPointer) {
            auto* newObjectRef = niPointer.get();
            _Log_("[BIND] Generated object {:x} (base type: {:x} {}) {}", newObjectRef->GetFormID(), baseForm->GetFormID(), baseForm->GetName(), baseForm->GetFormEditorID());
            Bind_Form(newObjectRef);
        } else
            _Log_("[BIND] Error [{}:{}] ({}) Could not generate object ({:x}, {})", FilePath, LineNumber, ScriptName, baseForm->GetFormID(), baseForm->GetName());
    }
    static RE::AlchemyItem* newBaseForm = nullptr;
    void Bind_GeneratedObject_WithEditorID(RE::TESForm* baseBaseForm, const std::string& editorID = "") {
        auto* baseForm = RE::TESForm::LookupByID<RE::AlchemyItem>(baseBaseForm->GetFormID());
        if (!baseForm) {
            _Log_("[BIND] Error [{}:{}] ({}) Base Form {:x} {} does not exist", FilePath, LineNumber, ScriptName, baseBaseForm->GetFormID(), baseBaseForm->GetName());
            return;
        }
        RE::AlchemyItem* existingForm = nullptr;
        if (!editorID.empty()) existingForm = RE::TESForm::LookupByEditorID<RE::AlchemyItem>(editorID);
        if (!existingForm) {
            newBaseForm = nullptr;
            // newBaseForm = static_cast<RE::AlchemyItem*>(baseForm->CreateDuplicateForm(false, newBaseForm));  // (void*)newBaseForm));  // Second argument not RE'd maybe?
            baseForm->CreateDuplicateForm(true, newBaseForm);  // (void*)newBaseForm));  // Second argument not RE'd maybe?
            if (newBaseForm) {
                _Log_("A new form was created with form ID {} and editor ID {}", newBaseForm->GetFormID(), newBaseForm->GetFormEditorID());
                auto* newNewForm = RE::TESForm::LookupByID<RE::AlchemyItem>(newBaseForm->GetFormID());
                if (!newNewForm) {
                    _Log_("Error: newNewForm is nullptr");
                    return;
                }
                newNewForm->SetFormEditorID(editorID.c_str());
                _Log_("xxx I legit just set the form editor ID of the new form to {} and it is {}", editorID.c_str(), newNewForm->GetFormEditorID());
                newNewForm->SetFormEditorID("ThisIsATest");

                _Log_("(Again) I legit just set the form editor ID of the new form to {} and it is {}", editorID.c_str(), newNewForm->GetFormEditorID());
                _Log_("[BIND] Generated New Base Form {:x} {}", newNewForm->GetFormID(), newNewForm->GetFormEditorID());
                Bind_GeneratedObject(newNewForm);
            } else {
                _Log_("[BIND] Error [{}:{}] ({}) Could not generate new base form ({:x}, {})", FilePath, LineNumber, ScriptName, baseForm->GetFormID(), baseForm->GetName());
            }
        } else
            _Log_("[BIND] Object already generated (created new base form: {:x} {})", baseForm->GetFormID(), baseForm->GetFormEditorID());
    }
    void Bind_GeneratedObject_BaseEditorID(const std::string& baseEditorId, const std::string& newObjectEditorID = "") {
        auto* form = LookupEditorID(baseEditorId);
        if (form) {
            if (!newObjectEditorID.empty()) {
                Bind_GeneratedObject_WithEditorID(form, newObjectEditorID);  // base form is actually ignored here. - We'll change syntax...
            } else {
                Bind_GeneratedObject(form, newObjectEditorID);
            }
        } else
            _Log_("[BIND] Error [{}:{}] ({}) Base Editor ID '{}' does not exist (Are you using po3 Tweaks?)", FilePath, LineNumber, ScriptName, baseEditorId);
    }
    void Bind_GeneratedObject_BaseFormID(RE::FormID baseFormID, const std::string& newObjectEditorID = "") {
        auto* form = LookupFormID(baseFormID);
        if (form) {
            if (!newObjectEditorID.empty())
                Bind_GeneratedObject_WithEditorID(form, newObjectEditorID);  // base form is actually ignored here. - We'll change syntax...
            else
                Bind_GeneratedObject(form, newObjectEditorID);
        } else
            _Log_("[BIND] Error [{}:{}] ({}) Base Form ID '{:x}' does not exist", FilePath, LineNumber, ScriptName, baseFormID);
    }
    void Bind_GeneratedQuest(const std::string& editorID = "") {
        RE::TESQuest* quest = nullptr;
        if (!editorID.empty()) quest = RE::TESForm::LookupByEditorID<RE::TESQuest>(editorID);
        if (!quest) {
            quest = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESQuest>()->Create();
            if (!editorID.empty()) quest->SetFormEditorID(editorID.c_str());
            _Log_("[BIND] Generated Quest {:x} {}", quest->GetFormID(), quest->GetFormEditorID());
        } else {
            _Log_("[BIND] Quest already generated {:x} {}", quest->GetFormID(), quest->GetFormEditorID());
        }
        Bind_Form(quest);
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
            _Log_("[BIND] Error [{}:{}] ({}) Cannot auto-bind to a script which does not `extends` anything", FilePath, LineNumber, ScriptName);
            return;
        }
        LowerCase(parentName);
        if (parentName == "quest")
            Bind_GeneratedQuest();
        else if (parentName == "actor")
            Bind_FormID(0x14);
        else if (parentName == "objectreference")
            Bind_GeneratedObject();
        else
            _Log_("[BIND] Error [{}:{}] ({}) No default BIND behavior available for script which `extends` {}", FilePath, LineNumber, ScriptName, parentName);
    }
    void ProcessBindingLine(std::string line) {
        if (line.empty()) return;
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::istringstream lineStream{line};
        lineStream >> ScriptName;
        if (ScriptName.empty() || ScriptName.starts_with('#') || ScriptName.starts_with("//")) return;
        _Log_("[BIND] [{}:{}] {}", FilePath, LineNumber, line);
        if (!vm->TypeIsValid(ScriptName)) {
            _Log_("[BIND] Error [{}:{}] Script '{}' does not exist", FilePath, LineNumber, ScriptName);
            return;
        }
        std::string bindTarget;
        std::getline(lineStream >> std::ws, bindTarget);
        LowerCase(bindTarget);
        if (bindTarget.empty()) {
            AutoBindBasedOnScriptExtends();
            return;
        } else if (bindTarget.starts_with("0x"))
            Bind_FormID(std::stoi(bindTarget, 0, 16));
        else if (bindTarget == "$player")
            Bind_FormID(0x14);
        else if (bindTarget.starts_with("$quest("))
            Bind_GeneratedQuest(bindTarget.substr(7, bindTarget.size() - 8));
        else if (bindTarget == "$quest")
            Bind_GeneratedQuest();
        else if (bindTarget == "$object")
            Bind_GeneratedObject();
        else if (bindTarget.starts_with("$object(0x")) {
            std::string beforeComma, afterComma;
            size_t commaIndex = bindTarget.find(',', 10);
            if (commaIndex != std::string::npos) {
                beforeComma = bindTarget.substr(10, commaIndex - 10);
                afterComma = bindTarget.substr(commaIndex + 1, bindTarget.size() - commaIndex - 2);  // -2 to remove the closing parenthesis
                afterComma.erase(0, afterComma.find_first_not_of(' '));
            } else {
                beforeComma = bindTarget.substr(10, bindTarget.size() - 11);  // -11 to remove the "0x" and the closing parenthesis
                afterComma = "";
            }
            auto formID = std::stoi(beforeComma, 0, 16);
            Bind_GeneratedObject_BaseFormID(formID, afterComma);
        } else if (bindTarget.starts_with("$object(")) {
            std::string beforeComma, afterComma;
            size_t commaIndex = bindTarget.find(',', 10);
            if (commaIndex != std::string::npos) {
                beforeComma = bindTarget.substr(8, commaIndex - 8);
                afterComma = bindTarget.substr(commaIndex + 1, bindTarget.size() - commaIndex - 2);  // -2 to remove the closing parenthesis
                afterComma.erase(0, afterComma.find_first_not_of(' '));
            } else {
                beforeComma = bindTarget.substr(8, bindTarget.size() - 9);  // -9 to remove the closing parenthesis
                afterComma = "";
            }
            Bind_GeneratedObject_BaseEditorID(beforeComma, afterComma);
        } else
            Bind_EditorID(bindTarget);
    }
    void ProcessBindingFile() {
        _Log_("[BIND] Reading Binding File: {}", FilePath);
        LineNumber = 1;
        std::string line;
        std::ifstream file{FilePath, std::ios::in};
        while (std::getline(file, line)) {
            LineNumber++;
            try {
                ProcessBindingLine(line);
            } catch (const std::exception& e) {
                _Log_("[BIND] Error [{}:{}] {}", FilePath, LineNumber, e.what());
            } catch (...) {
                _Log_("[BIND] Error [{}:{}] Unknown error", FilePath, LineNumber);
            }
        }
        file.close();
    }
    void ProcessAllBindingFiles() {
        if (!std::filesystem::is_directory(BINDING_FILES_FOLDER_ROOT)) {
            _Log_("[BIND] {} folder not found", BINDING_FILES_FOLDER_ROOT);
            return;
        }
        _Log_("[BIND] Processing Bind Scripts");
        for (auto& file : std::filesystem::directory_iterator(BINDING_FILES_FOLDER_ROOT)) {
            FilePath = file.path().string();
            ProcessBindingFile();
        }
    }
    void RunBindings() {
        if (AlreadyRanForThisLoadGame)
            return;
        else
            AlreadyRanForThisLoadGame = true;
        vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        DefaultBaseFormForCreatingObjects = RE::TESForm::LookupByID(0xAEBF3);             // DwarvenFork
        LocationForPlacingObjects = RE::TESForm::LookupByID<RE::TESObjectREFR>(0xBBCD1);  // The chest in WEMerchantChests
        ProcessAllBindingFiles();
    }
    class CellFullyLoadedEvent : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
    public:
        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent*, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* source) override {
            if (!AlreadyRanForThisLoadGame) {
                _Log_("[BIND] CellFullyLoadedEvent");
                RunBindings();
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };
    void SetupLog() {
        auto logsFolder = SKSE::log::log_directory();
        auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
        auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
        spdlog::set_default_logger(std::move(loggerPtr));
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::info);
        spdlog::set_pattern("%v");
    }
    CellFullyLoadedEvent _cellFullyLoadedEventSink;
    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();
        SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
            switch (message->type) {
                case SKSE::MessagingInterface::kNewGame:
                    _Log_("[BIND] New game started");
                    AlreadyRanForThisLoadGame = false;
                    break;
                case SKSE::MessagingInterface::kPostLoadGame:
                    AlreadyRanForThisLoadGame = false;
                    _Log_("[BIND] Game loaded");
                    RunBindings();
                    break;
                default:
                    break;
            }
        });
        RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESCellFullyLoadedEvent>(&_cellFullyLoadedEventSink);
        return true;
    }
}
