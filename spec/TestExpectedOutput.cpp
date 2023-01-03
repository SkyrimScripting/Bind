// ExpectedOutput.txt has EVERY line we expect to see in the logs (which begin with "[BIND]")
// At the time that the tests are run, they grab all of these lines from Papyrus.0.log and compare.
// The order does not matter. But (1) every line must be accounted for (2) no unexpected logs!

#define BANDIT_CONFIG_COLOR_ANSI

#include <bandit/bandit.h>
#include <snowhouse/snowhouse.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <regex>
#include <vector>

using namespace bandit;
using namespace snowhouse;

auto PapyrusLogPath() {
    auto papyrusLogPath = std::getenv("PAPYRUS_LOG_PATH");
    if (papyrusLogPath)
        return std::filesystem::path(papyrusLogPath);
    else
        return std::filesystem::path(std::getenv("USERPROFILE")) / "Documents" / "My Games" / "Skyrim Special Edition" / "Logs" / "Script" / "Papyrus.0.log";
}
auto ExpectedOutputPath() { return std::filesystem::current_path() / ".." / ".." / ".." / "spec" / "ExpectedOutput.txt"; }
void LowerCase(std::string& text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}
auto GetAllLinesFromFile(std::filesystem::path path) {
    std::vector<std::string> lines;
    std::ifstream file{path};
    std::string line;
    while (std::getline(file, line)) {
        LowerCase(line);
        auto startIndex = line.find("[!bind] ");
        if (startIndex != ::std::string::npos) lines.push_back(line.substr(startIndex + 7));  // 8 = "[!bind] "
    }
    return lines;
}

go_bandit([]() {
    describe("Example Scripts", []() {
        it("should bind the way we expect them to!", []() {
            auto expectedLines = GetAllLinesFromFile(ExpectedOutputPath());
            if (expectedLines.empty()) {
                std::cout << "Expected Lines were empty!" << std::endl;
                AssertThat(expectedLines, Is().Not().Empty());
            }

            auto realLines = GetAllLinesFromFile(PapyrusLogPath());
            if (realLines.empty()) {
                std::cout << "Papyrus.0.log Lines were empty!" << std::endl;
                AssertThat(realLines, Is().Not().Empty());
            }

            std::vector<std::string> notFoundLines;
            for (auto& expectedLine : expectedLines)
                if (std::find(realLines.begin(), realLines.end(), expectedLine) == realLines.end()) notFoundLines.push_back(expectedLine);
            if (!notFoundLines.empty())
                for (auto& notFoundLine : notFoundLines) std::cout << std::format("[NOT FOUND] {}", notFoundLine) << std::endl;

            std::vector<std::string> unexpectedLines;
            for (auto& realLine : realLines)
                if (std::find(expectedLines.begin(), expectedLines.end(), realLine) == expectedLines.end()) unexpectedLines.push_back(realLine);
            if (!unexpectedLines.empty())
                for (auto& unexpectedLine : unexpectedLines) std::cout << std::format("[UNEXPECTED] {}", unexpectedLine) << std::endl;

            bool ok = expectedLines.size() == realLines.size() && notFoundLines.empty() && unexpectedLines.empty();
            AssertThat(ok, IsTrue());
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
