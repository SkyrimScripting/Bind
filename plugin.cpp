// This whole plugin intentionally implemented in 1 file (for a truly minimalistic v1 of BIND)

#include <spdlog/sinks/basic_file_sink.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace logger = SKSE::log;

namespace SkyrimScripting::Bind {

    class GameStartedEvent : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
    public:
        std::function<void()> callback;
        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent*,
                                              RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* source) override {
            callback();
            source->RemoveEventSink(this);
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    class PapyrusCallbackHandler : public RE::BSScript::IStackCallbackFunctor {
    public:
        std::function<void()> callback;
        ~PapyrusCallbackHandler() override = default;
        void operator()(RE::BSScript::Variable) override { callback(); }
        bool CanSave() const override { return false; }
        void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}
    };

    RE::FormID DefaultBaseFormForCreatingObjects_FormID = 0xAEBF3;  // DwarvenFork
    RE::FormID LocationForPlacingObjects_FormID = 0xBBCD1;          // The chest in WEMerchantChests
    RE::TESForm* DefaultBaseFormForCreatingObjects;
    RE::TESObjectREFR* LocationForPlacingObjects;

    std::string FilePath;
    unsigned int LineNumber;
    std::string ScriptName;
    RE::BSScript::IVirtualMachine* vm;
    GameStartedEvent GameStartedEventListener;
    // PapyrusCallbackHandler PapyrusCallbackHandlerInstance;

    constexpr auto BINDING_FILES_FOLDER_ROOT = "Data/Scripts/Bindings";

    void LowerCase(std::string& text) {
        std::transform(text.begin(), text.end(), text.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }

    void Bind_Form(RE::TESForm* form) {
        auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(form->GetFormType(), form);
        RE::BSTSmartPointer<RE::BSScript::Object> object;
        vm->CreateObject(ScriptName, object);
        vm->GetObjectBindPolicy()->BindObject(object, handle);
    }

    void Bind_GeneratedObject(RE::TESForm* baseForm = nullptr) {
        if (!baseForm) baseForm = DefaultBaseFormForCreatingObjects;
        // By default, simply puts a fork next to the WEMerchantChest in the WEMerchantChests cell. Forking awesome.
        auto niPointer =
            LocationForPlacingObjects->PlaceObjectAtMe(skyrim_cast<RE::TESBoundObject*, RE::TESForm>(baseForm), false);
        Bind_Form(niPointer.get());
    }
    void Bind_GeneratedObject_BaseEditorID(const std::string& baseEditorId) {}
    void Bind_GeneratedObject_BaseFormID(RE::FormID baseFormID) {}
    void Bind_GeneratedQuest(std::string editorId = "") {
        auto* form = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESQuest>()->Create();
        if (!editorId.empty()) form->SetFormEditorID(editorId.c_str());
        Bind_Form(form);
    }
    void Bind_FormID(RE::FormID formId) {
        auto* form = RE::TESForm::LookupByID(formId);
        if (!form) {
            logger::info("BIND ERROR [{}:{}] ({}) Form ID '{:x}' does not exist", FilePath, LineNumber, ScriptName,
                         formId);
            return;
        }
        Bind_Form(form);
    }
    void Bind_EditorID(const std::string& editorId) {
        auto* form = RE::TESForm::LookupByEditorID(editorId);
        if (!form) {
            logger::info(
                "BIND ERROR [{}:{}] ({}) Form Editor ID '{}' does not exist (You might want to use po3 Tweaks if "
                "you're not already!)",
                FilePath, LineNumber, ScriptName, editorId);
            return;
        }
        Bind_Form(form);
    }
    void AutoBindBasedOnScriptExtends() {}

    void ProcessBindingLine(std::string line) {
        logger::info("PROCESS [{}:{}] \"{}\"", FilePath, LineNumber, line);
        if (line.empty()) return;
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::istringstream lineStream{line};
        lineStream >> ScriptName;
        if (ScriptName.empty() || ScriptName.starts_with('#') || ScriptName.starts_with("//")) return;
        if (!vm->TypeIsValid(ScriptName)) {
            logger::info("BIND ERROR [{}:{}] Script '{}' does not exist", FilePath, LineNumber, ScriptName);
            return;
        }
        std::string bindTarget;
        lineStream >> bindTarget;
        LowerCase(bindTarget);
        if (bindTarget.empty())
            AutoBindBasedOnScriptExtends();
        else if (bindTarget.starts_with("0x"))
            Bind_FormID(std::stoi(bindTarget, 0, 16));
        else if (bindTarget == "$player")
            Bind_FormID(0x14);
        else if (bindTarget.starts_with("$quest("))
            Bind_GeneratedQuest(bindTarget.substr(7, bindTarget.size() - 8));
        else if (bindTarget == "$quest")
            Bind_GeneratedQuest();
        else if (bindTarget == "$object")
            Bind_GeneratedObject();
        else if (bindTarget.starts_with("$object(0x"))
            Bind_GeneratedObject_BaseFormID(std::stol(bindTarget.substr(8, bindTarget.size() - 9)));
        else if (bindTarget.starts_with("$object("))
            Bind_GeneratedObject_BaseEditorID(bindTarget.substr(8, bindTarget.size() - 9));
        else
            Bind_EditorID(bindTarget);
    }

    void ProcessBindingFile() {
        logger::info("Reading Binding File: {}", FilePath);
        LineNumber = 1;
        std::string line;
        std::ifstream file{FilePath, std::ios::in};
        while (std::getline(file, line)) {
            LineNumber++;
            try {
                ProcessBindingLine(line);
            } catch (...) {
                logger::info("BIND ERROR [{}:{}]", FilePath, LineNumber);
            }
        }
        file.close();
    }

    void ProcessAllBindingFiles() {
        for (auto& file : std::filesystem::directory_iterator(BINDING_FILES_FOLDER_ROOT)) {
            FilePath = file.path().string();
            ProcessBindingFile();
        }
    }

    void OnGameStart() {
        vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        DefaultBaseFormForCreatingObjects = RE::TESForm::LookupByID(DefaultBaseFormForCreatingObjects_FormID);
        LocationForPlacingObjects = RE::TESForm::LookupByID<RE::TESObjectREFR>(LocationForPlacingObjects_FormID);
        ProcessAllBindingFiles();
    }

    void SetupLog() {
        auto logsFolder = SKSE::log::log_directory();
        if (!logsFolder) {
            SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
            return;
        }
        auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
        auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
        spdlog::set_default_logger(std::move(loggerPtr));
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::info);
        spdlog::set_pattern("%v");
    }

    SKSEPluginLoad(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);
        SetupLog();
        GameStartedEventListener.callback = []() { OnGameStart(); };
        RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESCellFullyLoadedEvent>(
            &GameStartedEventListener);
        return true;
    }
}
