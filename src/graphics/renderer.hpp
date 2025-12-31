#pragma once

#include "../includes/graphics.hpp"
#include <fstream>
#include <vector>

namespace Graphics {

    class Renderer {

        public:
            Renderer(VkDevice device, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, std::vector<VkImageView> swapChainImageViews);
            ~Renderer();

        private:
            VkDevice vk_logicalDevice;
            VkPipeline vk_graphicsPipeline;
            VkRenderPass vk_renderPass;
            VkPipelineLayout vk_pipelineLayout;
            VkShaderModule vk_vertShaderModule;
            VkShaderModule vk_fragShaderModule;
            std::vector<VkFramebuffer> vk_swapChainFramebuffers;
            std::vector<VkDynamicState> vk_dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            std::vector<char> readShaderFile(const std::string& filename);

            VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device, VkExtent2D swapChainExtent);
    };
}