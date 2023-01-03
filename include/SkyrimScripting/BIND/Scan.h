

// namespace BIND {

//     namespace Scan {

//         // void Thread_AddDiscoveredBindingDefinitionsFromFile(std::vector<std::string> definitionsAsText) {
//         //     std::lock_guard<std::mutex> guard(PushDiscoveredBindingDefinitionsMutex);
//         //     ListOfDiscoveredBindingDefinitionsFromDocStrings.insert(
//         //         ListOfDiscoveredBindingDefinitionsFromDocStrings.end(), definitionsAsText.begin(),
//         //         definitionsAsText.end());
//         // }

//         // JSON --> Data Structure that we can savely read/write to during the multithreaded processing

//         void Thread_DocStringWorkerThread(unsigned int id) {
//             logger::info("Start thread {}", id);
//             unsigned int processedCount = 0;
//             auto path = Thread_GetNextFileToSearchForDocStrings();
//             while (!path.empty()) {
//                 processedCount++;

//                 auto pex = Pex::Binary();
//                 auto reader = Pex::FileReader(path.string());

//                 try {
//                     reader.read(pex);
//                     auto docString = pex.getObjects().front().getDocString().asString();
//                     if (!docString.empty()) {
//                         std::string line;
//                         std::istringstream commentString{docString};
//                         // std::atomic<bool> firstFoundBinding = true;
//                         while (std::getline(commentString, line)) {
//                             Util::Trim(line);
//                             if (line.starts_with("!BIND")) {
//                                 // if (firstFoundBinding.exchange(false)) scriptBindComments[scriptName].clear();
//                                 // auto bindCommand = scriptName + " " + line.substr(5);
//                                 // BindingLinesFromComments.emplace_back(bindCommand);
//                                 // scriptBindComments[scriptName].append(line);
//                                 logger::info("{} BIND! {}", path.string(), line);
//                             }
//                         }
//                     }
//                 } catch (...) {
//                     logger::error("Champollion library failed to read .pex for script {}", path.string());
//                 }

//                 path = Thread_GetNextFileToSearchForDocStrings();
//             }
//             // Shutdown
//             logger::info("Shutdown thread {}. Processed total of {} files.", id, processedCount);
//         }

//     }
// }
