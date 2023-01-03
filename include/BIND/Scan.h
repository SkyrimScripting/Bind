#pragma once

#include <SkyrimScripting/Plugin.h>

#include <Champollion/Pex/FileReader.hpp>
#include <atomic>
#include <filesystem>
#include <memory>
#include <queue>
#include <string>
#include <thread>

#include "BIND/BindingDefinition.h"
#include "BIND/Util.h"

namespace BIND {

    namespace Scan {
        std::atomic<unsigned int> threadId;
        std::mutex PopFileToInvestigateMutex;
        std::mutex PushDiscoveredBindingDefinitionsMutex;
        std::queue<std::filesystem::path> FilesToCheckForDocStrings;
        std::vector<std::string> ListOfDiscoveredBindingDefinitionsFromDocStrings;
        // std::vector<BindingDefinition> ListOfDiscoveredBindingDefinitionsFromDocStrings; // TODO :) Strings for now!

        void LoadListOfFilesIntoQueue() {
            logger::trace("Loading List of loose .pex files to check for {{!BIND}} comments");
            auto start = std::chrono::high_resolution_clock::now();
            auto scriptsFolder = std::filesystem::current_path() / "Data\\Scripts";
            for (auto& entry : std::filesystem::directory_iterator(scriptsFolder))
                if (entry.is_regular_file()) FilesToCheckForDocStrings.push(entry.path());
            auto end = std::chrono::high_resolution_clock::now();
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            logger::info(".pex file discovery took {}ms", durationMs);
        }

        std::filesystem::path Thread_GetNextFileToSearchForDocStrings() {
            std::lock_guard<std::mutex> guard(PopFileToInvestigateMutex);
            if (FilesToCheckForDocStrings.size()) {
                auto path = FilesToCheckForDocStrings.front();
                FilesToCheckForDocStrings.pop();
                return path;
            } else {
                return {};
            }
        }

        void Thread_AddDiscoveredBindingDefinitionsFromFile(std::vector<std::string> definitionsAsText) {
            std::lock_guard<std::mutex> guard(PushDiscoveredBindingDefinitionsMutex);
            ListOfDiscoveredBindingDefinitionsFromDocStrings.insert(
                ListOfDiscoveredBindingDefinitionsFromDocStrings.end(), definitionsAsText.begin(),
                definitionsAsText.end());
        }

        // JSON --> Data Structure that we can savely read/write to during the multithreaded processing

        void Thread_DocStringWorkerThread(unsigned int id) {
            logger::info("Start thread {}", id);
            unsigned int processedCount = 0;
            auto path = Thread_GetNextFileToSearchForDocStrings();
            while (!path.empty()) {
                processedCount++;

                auto pex = Pex::Binary();
                auto reader = Pex::FileReader(path.string());

                try {
                    reader.read(pex);
                    auto docString = pex.getObjects().front().getDocString().asString();
                    if (!docString.empty()) {
                        std::string line;
                        std::istringstream commentString{docString};
                        // std::atomic<bool> firstFoundBinding = true;
                        while (std::getline(commentString, line)) {
                            Util::Trim(line);
                            if (line.starts_with("!BIND")) {
                                // if (firstFoundBinding.exchange(false)) scriptBindComments[scriptName].clear();
                                // auto bindCommand = scriptName + " " + line.substr(5);
                                // BindingLinesFromComments.emplace_back(bindCommand);
                                // scriptBindComments[scriptName].append(line);
                                logger::info("{} BIND! {}", path.string(), line);
                            }
                        }
                    }
                } catch (...) {
                    logger::error("Champollion library failed to read .pex for script {}", path.string());
                }

                path = Thread_GetNextFileToSearchForDocStrings();
            }
            // Shutdown
            logger::info("Shutdown thread {}. Processed total of {} files.", id, processedCount);
        }

        void ReadAllDocStrings() {
            logger::trace("Processing {} .pex files, searching for {{!BIND}} comments",
                          FilesToCheckForDocStrings.size());
            auto start = std::chrono::high_resolution_clock::now();

            // Let's do one thread per thousand files? Sure, why not.
            auto threadCount = FilesToCheckForDocStrings.size() / 1000;
            auto processingUnitCount = std::thread::hardware_concurrency();
            if (threadCount > processingUnitCount) threadCount = processingUnitCount;
            if (threadCount < 1) threadCount = 3;  // 1 - more for testing

            // testing...
            // threadCount = 1;

            logger::info("Starting {} worker threads", threadCount);
            std::vector<std::thread> threads;
            for (size_t i = 0; i < threadCount; i++) {
                threadId++;
                threads.emplace_back(std::thread(Thread_DocStringWorkerThread, threadId.load()));
            }

            // For testing, let's block until they're all done.
            for (auto& thread : threads) thread.join();

            auto end = std::chrono::high_resolution_clock::now();
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            logger::info(".pex file processing took {}ms", durationMs);
        }

        void ScanForDocStringsInScripts() {
            LoadListOfFilesIntoQueue();
            ReadAllDocStrings();
        }
    }
}
