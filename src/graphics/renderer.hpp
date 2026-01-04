#pragma once

#include "../includes/graphics.hpp"
#include <fstream>
#include <cassert>
#include <iostream>
#include <vector>

namespace Graphics {

    class Renderer {

        public:
            Renderer(VkDevice device, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, std::vector<VkImageView> swapChainImageViews, VkCommandPool commandPool);
            ~Renderer();
            void drawFrame(VkSwapchainKHR swapChain, VkExtent2D swapChainExtent, VkQueue graphicsQueue, VkQueue presentQueue);

        private:
            VkDevice vk_logicalDevice;
            VkPipeline vk_graphicsPipeline;
            VkRenderPass vk_renderPass;
            VkPipelineLayout vk_pipelineLayout;
            VkShaderModule vk_vertShaderModule;
            VkShaderModule vk_fragShaderModule;
            VkCommandBuffer vk_commandBuffer;
            VkSemaphore vk_imageAvailableSemaphore;
            VkSemaphore vk_renderFinishedSemaphore;
            VkFence vk_inFlightFence;
            std::vector<VkSemaphore> vk_renderFinishedSemaphores;
            std::vector<VkFramebuffer> vk_swapChainFramebuffers;
            std::vector<VkDynamicState> vk_dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            std::vector<char> readShaderFile(const std::string& filename);

            void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkExtent2D swapChainExtent);
            VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device, VkExtent2D swapChainExtent);
    };
}