project(BIND.Specs)

find_path(SNOWHOUSE_INCLUDE_DIRS snowhouse/snowhouse.h)
find_path(BANDIT_INCLUDE_DIRS    bandit/bandit.h)

add_executable(${PROJECT_NAME} TestExpectedOutput.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE ${SNOWHOUSE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BANDIT_INCLUDE_DIRS})
