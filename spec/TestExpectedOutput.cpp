#define BANDIT_CONFIG_COLOR_ANSI

#include <bandit/bandit.h>
#include <snowhouse/snowhouse.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>

using namespace bandit;
using namespace snowhouse;

auto PapyrusLogPath() { return std::filesystem::path(std::getenv("USERPROFILE")) / "Documents" / "My Games" / "Skyrim Special Edition" / "Logs" / "Script" / "Papyrus.0.log"; }
auto ExpectedOutputPath() { return std::filesystem::current_path() / ".." / ".." / ".." / "spec" / "ExpectedOutput.txt"; }
auto ReadFile(std::filesystem::path path) {
    auto ifs = std::ifstream(path);
    return std::string(std::istreambuf_iterator<char>{ifs}, {});
}

go_bandit([]() {
    describe("Example Scripts", []() {
        it("should bind the way we expect them to!", []() {
            // ExpectedOutput.txt has EVERY line we expect to see in the logs (which begin with "[BIND]")
            // At the time that the tests are run, they grab all of these lines from Papyrus.0.log and compare.
            // The order does not matter. But (1) every line must be accounted for (2) no unexpected logs!
            std::cout << std::format("CONTENTS: {}", ReadFile(ExpectedOutputPath()));
            // std::cout << std::format("CONTENTS: {}", ReadFile(PapyrusLogPath()));
        });
    });
});

int main() {
    // ... because I like --reporter=spec
    std::vector<std::string> args = {"", "--reporter=spec"};
    std::vector<char*> argv;
    for (const auto& arg : args) argv.push_back((char*)arg.data());
    bandit::run((int)argv.size(), argv.data());
}
