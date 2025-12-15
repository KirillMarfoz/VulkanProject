#pragma once

#include <stdexcept>
#include <cstring>
#include <vector>
#include "../includes/graphics.hpp"

namespace Graphics::Instance {

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    class Instance {

        public:
            Instance();
            ~Instance();

        private:
            VkInstance vk_instance;
            const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

            bool checkValidationLayerSupport();
    };
}