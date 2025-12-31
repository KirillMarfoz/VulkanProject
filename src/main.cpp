#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "graphics/instance.hpp"
#include "graphics/device.hpp"
#include "graphics/renderer.hpp"

//------------------------------LOAD JSON------------------------------
nlohmann::json loadJson() {
    std::ifstream file("./settings/windowSettings.json");

    if (!file.is_open()) {
        std::cerr << "Failed to open JSON config\n";
    }
    
    nlohmann::json j;
    file >> j;
    return j;
}

//------------------------------INITIALIZE GLFW------------------------------
GLFWwindow* initGLFW(const nlohmann::json& w) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    const auto& json = w.at("window");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(
        json.at("width").get<int>(),
        json.at("height").get<int>(),
        json.at("title").get<std::string>().c_str(),
        json.at("fullscreen").get<bool>() ? glfwGetPrimaryMonitor() : nullptr,
        nullptr
    );
}

int main() {
    try {
        nlohmann::json json = loadJson();
        GLFWwindow* window = initGLFW(json);

        if (!window)
            throw std::runtime_error("Failed to create GLFW window");

        {
            Graphics::Instance instance;
            instance.createSurface(window);
            Graphics::Device device(instance.getInstance(), instance.getSurface(), instance.getEnableValidationLayers(), instance.getValidationLayers());
            device.createSwapChain(window, instance.getSurface());
            device.createImageViews();
            Graphics::Renderer renderer(device.getLogicalDevice(), device.getSwapChainExtent(), device.getSwapChainImageFormat(), device.getSwapChainImageViews());

            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
            }
        }

        glfwDestroyWindow(window);
        glfwTerminate();

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return 0;
}