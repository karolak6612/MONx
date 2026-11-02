cmake_minimum_required(VERSION 3.20)
project(monx VERSION 0.1.133 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(wxWidgets REQUIRED COMPONENTS core base adv aui propgrid html xml net)
include(${wxWidgets_USE_FILE})

find_package(pugixml REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.4)
find_package(LibLZMA REQUIRED)

# Project paths
include_directories(
    cpp/include
    ${wxWidgets_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${LIBLZMA_INCLUDE_DIRS}
)

file(GLOB_RECURSE MONX_SOURCES "cpp/src/*.cpp")
list(FILTER MONX_SOURCES EXCLUDE REGEX "cpp/src/probe_main.cpp")
file(GLOB_RECURSE MONX_HEADERS "cpp/include/*.h" "cpp/include/*.hpp")

add_executable(monx ${MONX_SOURCES} ${MONX_HEADERS})

target_link_libraries(monx PRIVATE
    ${wxWidgets_LIBRARIES}
    pugixml::pugixml
    nlohmann_json::nlohmann_json
    ${LUA_LIBRARIES}
    LibLZMA::LibLZMA
    pthread
)

# Test probe executable
add_executable(monx_probe
    cpp/src/probe_main.cpp
    cpp/src/engine/engine_profile.cpp
    cpp/src/monster/monster_storage.cpp
    cpp/src/monster/lua_monster_parser.cpp
    cpp/src/formats/formats_manager.cpp
    cpp/src/workspace.cpp
)
target_link_libraries(monx_probe PRIVATE
    ${wxWidgets_LIBRARIES}
    pugixml::pugixml
    nlohmann_json::nlohmann_json
    ${LUA_LIBRARIES}
    LibLZMA::LibLZMA
    pthread
)
target_compile_definitions(monx_probe PRIVATE MONX_PROBE_ONLY=1)
