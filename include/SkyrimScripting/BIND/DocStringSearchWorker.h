#pragma once

#include <SkyrimScripting/Plugin.h>

#include <Champollion/Pex/FileReader.hpp>

#include "SkyrimScripting/BIND/BindingDefinition.h"
#include "SkyrimScripting/BIND/Util.h"

namespace SkyrimScripting::BIND::DocStringSearch {

    std::mutex Mutex_GetNextFilePath;
    std::mutex Mutex_AddBindingDefinitions;

    std::atomic<unsigned int> NextWorkerId;

    class DocStringSearchWorker {
        unsigned int _workerID;
        std::queue<std::filesystem::path>& _fileSearchQueue;
        std::vector<BindingDefinition>& _bindingDefinitions;

    public:
        DocStringSearchWorker(unsigned int workerID, std::queue<std::filesystem::path>& fileSearchQueue,
                              std::vector<BindingDefinition>& bindingDefinitions)
            : _workerID(workerID), _fileSearchQueue(fileSearchQueue), _bindingDefinitions(bindingDefinitions) {}

        std::filesystem::path GetNextPath() {
            std::lock_guard<std::mutex> guard(Mutex_GetNextFilePath);
            if (_fileSearchQueue.empty())
                return {};
            else {
                auto path = _fileSearchQueue.front();
                _fileSearchQueue.pop();
                return path;
            }
        }

        void DoWork() {
            logger::info("[Thread {}] Begin .pex queue work", _workerID);
            unsigned int processedCount = 0;

            auto path = GetNextPath();
            while (!path.empty()) {
                logger::info("[Thread {}] {}", _workerID, path.string());
                // ...
                path = GetNextPath();
            }

            logger::info("[Thread {}] End .pex queue work ({} processed)", _workerID, processedCount);
        }
    };
    static void RUN_WORKER(std::queue<std::filesystem::path>& fileSearchQueue,
                           std::vector<BindingDefinition>& bindingDefinitions) {
        DocStringSearchWorker(NextWorkerId++, fileSearchQueue, bindingDefinitions).DoWork();
    }
}
