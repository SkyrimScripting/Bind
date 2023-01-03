cmake_minimum_required(VERSION 3.21)

project(SkyrimScripting.Bind VERSION 0.2.0 LANGUAGES CXX)

foreach(package jsoncpp CommonLibSSE Champollion SkyrimScripting.Plugin SkyrimScripting.CMake)
    find_package(${package} CONFIG REQUIRED)
endforeach()

add_skse_plugin(${PROJECT_NAME} SOURCES plugin.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)

foreach(library JsonCpp::JsonCpp Champollion::Champollion SkyrimScriptingPlugin::SkyrimScripting.Plugin)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${library})    
endforeach()

add_subdirectory(spec)
