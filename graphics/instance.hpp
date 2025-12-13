#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Graphics::Instance {

    class Instance {

        public:
            Instance();
            ~Instance();

        private:
            VkInstance vk_instance;
        
    };
}