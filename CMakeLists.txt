cmake_minimum_required(VERSION 3.21)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Vulkan ----------
find_package(Vulkan REQUIRED)

# ---------- GLFW ----------
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ---------- Executable ----------
add_executable(VulkanApp
    src/main.cpp
    graphics/instance.hpp
    graphics/instance.cpp
    includes/graphics.hpp
)

target_link_libraries(VulkanApp
    Vulkan::Vulkan
    glfw
    nlohmann_json::nlohmann_json
)
