#include "imgui_loop.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_sdl2.h"

#include "render/backend/vk_error.h"
#include "render/backend/command_buffer.h"

#include <vector>


ImGuiLoop::ImGuiLoop(
        Window* window,
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice logicalDevice,
        VkRenderPass renderPass,
        VkCommandPool commandPool,
        VkQueue graphicsQueue,
        u32 graphicsQueueFamily,
        u32 swapchainImageCount
)
    : m_window (window)
    , m_device (logicalDevice)
{
    ImGui::CreateContext();
	ImGui::StyleColorsDark();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    err(ImGui_ImplSDL2_InitForVulkan(window->m_sdlWindow));

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    int maxSets = 12;

    // numbers above are ????

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (u32)poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets * swapchainImageCount;

    vk(vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &m_pool));

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = logicalDevice;
    init_info.QueueFamily = graphicsQueueFamily;
    init_info.Queue = graphicsQueue;
    init_info.DescriptorPool = m_pool;
    init_info.MinImageCount = swapchainImageCount;
    init_info.ImageCount = swapchainImageCount;

    err(ImGui_ImplVulkan_Init(&init_info, renderPass));

    {
        CommandBuffer cmd = CommandBuffer(logicalDevice, commandPool);   
        cmd.begin();
        ImGui_ImplVulkan_CreateFontsTexture(cmd.m_commandBuffer);
        cmd.end();
        cmd.submit(graphicsQueue);
    }

    window->addEventListener([](SDL_Event event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    });
}

ImGuiLoop::~ImGuiLoop() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
 	ImGui::DestroyContext();

    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}

void ImGuiLoop::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window->m_sdlWindow);
    ImGui::NewFrame();
}

void ImGuiLoop::submitFrame(VkCommandBuffer commandBuffer) {
	ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

#ifdef _WIN32
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
#endif
}
