#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

void cleanup(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
//------------------------------------json initialization------------------------------------------------

    using json = nlohmann::json;
    json j;
    std::ifstream file("./settings/windowSettings.json");

    if (!file.is_open()) {
        std::cerr << "Failed to open JSON config\n";
    } else {
        file >> j;
    };
    auto& w = j["window"];
//------------------------------------GLFW initialization----------------------------------------------------
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(
    w.value("width", 80),
    w.value("height", 60),
    w.value("title", "Vulkan App").c_str(),
    w.value("fullscreen", false) ? glfwGetPrimaryMonitor() : nullptr,
    nullptr
);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

//------------------------------------Cleanup--------------------------------------------------------------
    cleanup(window);
    return 0;
}