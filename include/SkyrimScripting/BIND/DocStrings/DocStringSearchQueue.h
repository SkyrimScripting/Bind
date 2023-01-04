
#pragma once

#include <SkyrimScripting/Plugin.h>

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/DocStrings/DocStringSearchWorker.h"

namespace SkyrimScripting::BIND::DocStrings {

    std::queue<std::filesystem::path> FileSearchQueue;

    void PopulateFileSearchQueue() {
        logger::trace("Loading List of loose .pex files to check for {{!BIND}} comments");
        auto start = std::chrono::high_resolution_clock::now();

        auto scriptsFolder = std::filesystem::current_path() / "Data" / "Scripts";

        for (auto& entry : std::filesystem::directory_iterator(scriptsFolder))
            if (entry.is_regular_file()) FileSearchQueue.push(entry.path());

        auto end = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        logger::info(".pex file discovery took {}ms", durationMs);
    }

    void ProcessFileSearchQueue(std::vector<BindingDefinition>& bindingDefinitions) {
        logger::trace("Processing {} .pex files, searching for {{!BIND}} comments", FileSearchQueue.size());
        auto start = std::chrono::high_resolution_clock::now();

        auto threadCount = FileSearchQueue.size() / 1000;
        auto processingUnitCount = std::thread::hardware_concurrency();
        if (threadCount > processingUnitCount) threadCount = processingUnitCount;
        if (threadCount < 1) threadCount = 4;  // 1; // test

        logger::info("Starting {} worker threads", threadCount);
        std::vector<std::thread> threads;
        for (size_t i = 0; i < threadCount; i++)
            threads.emplace_back(std::thread(RUN_WORKER, std::ref(FileSearchQueue), std::ref(bindingDefinitions)));
        for (auto& thread : threads) thread.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        logger::info(".pex file processing took {}ms", durationMs);
    }
}