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

    void Bind_GeneratedObject_BaseEditorID(const std::string& baseEditorId) {}
    void Bind_GeneratedObject_BaseFormID(RE::FormID baseFormID) {}
    void Bind_GeneratedObject() {}
    void Bind_GeneratedQuest() {}
    void Bind_FormID(RE::FormID formId) {
        auto* form = RE::TESForm::LookupByID(formId);
        if (!form) {
            logger::info("BIND ERROR [{}:{}] ({}) Form ID '{:x}' does not exist", FilePath, LineNumber, ScriptName,
                         formId);
            return;
        }
        auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(form->GetFormType(), form);
        RE::BSTSmartPointer<RE::BSScript::Object> object;
        vm->CreateObject(ScriptName, object);
        vm->GetObjectBindPolicy()->BindObject(object, handle);
    }
    void Bind_EditorID(const std::string& editorId) {}
    void AutoBindBasedOnScriptExtends() {}

    void ProcessBindingLine(std::string line) {
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
            Bind_FormID(std::stol(bindTarget));
        else if (bindTarget == "$player")
            Bind_FormID(0x14);
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
        if (!line.empty()) {
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
