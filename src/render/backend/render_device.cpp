#include "render_device.h"

#include "render/backend/type/platform/vk_error.h"
#include "SDL_vulkan.h"

#include <vector>
#include <stdexcept>
#include <optional>
#include <unordered_set>
#include <unordered_map>

#include "imgui.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "render/backend/type/platform/command_buffer_vulkan.h"
#include "render/backend/type/platform/render_pass_vulkan.h"

struct VulkanSwapChainAvailableConfigs {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanSurfaceAvailableConfigs {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndex {
    std::optional<u32> graphics;
    std::optional<u32> present;

    bool hasRequiredQueues() const {
        return graphics.has_value() 
            && present.has_value();
    }

    std::unordered_set<u32> toSet() const {
        return {
            graphics.value(),
            present.value()
        };
    }
};

VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    printf("vk: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

VkResult vulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    return func == nullptr
        ? VK_ERROR_EXTENSION_NOT_PRESENT
        : func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void vulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkDebugUtilsMessengerCreateInfoEXT vulkanGetDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT debugCallbackInfo{};
    debugCallbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCallbackInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCallbackInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCallbackInfo.pfnUserCallback = vulkanDebugMessageCallback;

    return debugCallbackInfo;
}

QueueFamilyIndex vulkanGetQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndex index{};

    for (u32 i = 0; i < (u32)queueFamilies.size(); i++) {
        const VkQueueFamilyProperties& props = queueFamilies.at(i);

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            index.graphics = i;
        }

        VkBool32 hasPresentSupport = false;
        vk(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &hasPresentSupport));

        if (hasPresentSupport) {
            index.present = i;
        }

        if (index.hasRequiredQueues()) {
            break;
        }
    }

    return index;
}

VulkanSwapChainAvailableConfigs vulkanGetSwapChainAvailableConfigs(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    VulkanSwapChainAvailableConfigs details{};
    
    vk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities));
    
    u32 formatCount;
    vk(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));
    details.formats.resize(formatCount);
    vk(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data()));

    u32 presentModeCount;
    vk(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    details.presentModes.resize(presentModeCount);
    vk(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data()));

    return details;
}

RenderDevice::RenderDevice(Window* window, bool useDebug) 
    : m_window   (window)
    , m_useDebug (useDebug)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Lith Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "lith";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;

    // Add layers

    std::vector<const char*> layers;

    if (m_useDebug) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    instInfo.enabledLayerCount = layers.size();
    instInfo.ppEnabledLayerNames = layers.data();

    // Add extensions

    std::vector<const char*> instanceExtensions;

    // get the extensions that sdl requires
    uint sdlExtCount = 0;
    SDL_Vulkan_GetInstanceExtensions(m_window->m_sdlWindow, &sdlExtCount, nullptr);
    instanceExtensions.resize(sdlExtCount);
    SDL_Vulkan_GetInstanceExtensions(m_window->m_sdlWindow, &sdlExtCount, instanceExtensions.data());

    // on apple we need to add this extension
    #ifdef __APPLE__
        instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        instanceExtensions.push_back("VK_KHR_get_physical_device_properties2");
        instInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif

    // for the debug callback we need this extension
    if (m_useDebug) {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instInfo.enabledExtensionCount = instanceExtensions.size();
    instInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // Enable debug for the create instance call

    if (m_useDebug) {
        VkDebugUtilsMessengerCreateInfoEXT debugCallbackInfo = vulkanGetDebugMessengerCreateInfo();
        instInfo.pNext = &debugCallbackInfo;
    }

    // Create the instance

    vk(vkCreateInstance(&instInfo, nullptr, &m_instance));

    // Create the debug messenger

    if (m_useDebug) {
        VkDebugUtilsMessengerCreateInfoEXT debugCallbackInfo = vulkanGetDebugMessengerCreateInfo();
        vk(vulkanCreateDebugUtilsMessengerEXT(m_instance, &debugCallbackInfo, nullptr, &m_debugMessenger));
    }

    SDL_Vulkan_CreateSurface(m_window->m_sdlWindow, m_instance, &m_surface);
    
    // Find all graphics devices connected to the system
    
    u32 deviceCount = 0;
    vk(vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr));
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vk(vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()));

    if (devices.size() == 0) {
        throw std::runtime_error("Failed to find GPU with Vulkan support");
    }

    // For this application, only a single card that supports drawing to the screen is needed

    std::vector<const char*> physicalDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    #ifdef __APPLE__
        // VUID-VkDeviceCreateInfo-pProperties-04451
        // "VK_KHR_get_physical_device_properties" is required in instanceExtensions
        physicalDeviceExtensions.push_back("VK_KHR_portability_subset");
    #endif

    for (const VkPhysicalDevice& device : devices) {
        // check props
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        // this only matters if there are more than 1 gpu
        if (   deviceCount > 1 
            && props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
        {
            continue;
        }

        // checks features
        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(device, &feats);
        if (!feats.samplerAnisotropy) {
            continue;
        }

        // check index
        QueueFamilyIndex index = vulkanGetQueueFamilyIndex(device, m_surface);
        if (!index.hasRequiredQueues()) {
            continue;
        }

        // check extensions
        u32 extensionCount;
        vk(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vk(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

        bool hasAllExtensions = true;

        for (const char* ext : physicalDeviceExtensions) {
            auto itr = std::find_if(availableExtensions.begin(), availableExtensions.end(), 
                [ext](const VkExtensionProperties& props) { return strcmp(props.extensionName, ext) == 0; });
                
            if (itr == availableExtensions.end()) {
                hasAllExtensions = false;
                break;
            }
        }

        if (!hasAllExtensions) {
            continue;
        }

        // check swap chain
        VulkanSwapChainAvailableConfigs swapChainConfig = vulkanGetSwapChainAvailableConfigs(device, m_surface);
        if (swapChainConfig.formats.empty() || swapChainConfig.presentModes.empty()) {
            continue;
        }

        // If the device has passed all checks, use it.

        m_physicalDevice = device;
        m_graphicsQueueIndex = index.graphics.value();
        m_presentQueueIndex = index.present.value();

        break;
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find GPU with the required Vulkan support");
    }

    // Create queues

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    float queuePriority = 1.0f;

    // iterate unique queues
    for (u32 index : std::unordered_set<u32>{m_graphicsQueueIndex, m_presentQueueIndex}) {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = index;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = queueInfos.size();
    deviceInfo.pQueueCreateInfos = queueInfos.data();
    deviceInfo.enabledExtensionCount = physicalDeviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = physicalDeviceExtensions.data();
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    
    vk(vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_logicalDevice));

    vkGetDeviceQueue(m_logicalDevice, m_graphicsQueueIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, m_presentQueueIndex, 0, &m_presentQueue);

    // It seems like there needs to be a single command pool for each thread
    // Create a command pool for each thread, for now just one (and not in a list)

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = m_graphicsQueueIndex;

    vk(vkCreateCommandPool(m_logicalDevice, &commandPoolInfo, nullptr, &m_commandPool));

    // Create factories

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.instance = m_instance;
    allocatorInfo.physicalDevice = m_physicalDevice;
    allocatorInfo.device = m_logicalDevice;

    vk(vmaCreateAllocator(&allocatorInfo, &m_allocator));

    // factories

    DescriptorPoolLayout descriptorPoolLayout{};
    descriptorPoolLayout.maxSetCount = 3;
    descriptorPoolLayout.pools = {
        { DescriptorType_UniformBuffer, 1 },
        { DescriptorType_ImageSampler, 2 }
    };

    bufferFactory = new BufferFactoryVulkan(m_logicalDevice, m_allocator);
    commandBufferFactory = new CommandBufferFactoryVulkan(m_logicalDevice, m_commandPool, m_graphicsQueue, getFrameSyncInfo());
    descriptorSetFactory = new DescriptorSetFactoryVulkan(m_logicalDevice, descriptorPoolLayout, getFrameSyncInfo());
    descriptorSetLayoutFactory = new DescriptorSetLayoutFactoryVulkan(m_logicalDevice);
    frameBufferFactory = new FrameBufferFactoryVulkan(m_logicalDevice);
    imageFactory = new ImageFactoryVulkan(m_logicalDevice, m_allocator, m_commandPool, m_graphicsQueue, bufferFactory, commandBufferFactory);
    imageSamplerFactory = new ImageSamplerFactoryVulkan(m_logicalDevice);
    renderPassFactory = new RenderPassFactoryVulkan(m_logicalDevice);

    m_renderPass = renderPassFactory->createRenderPass();
    shaderFactory = new ShaderFactoryVulkan(m_logicalDevice, m_renderPass);

    // Create the swap chain
    chooseSurfaceConfig(); // This has to be done before creating the render pass
    createSwapchain();

    // Create frames in flight

    for (u32 i = 0; i < m_framesInFlight; i++) {
        CommandBuffer* commandBuffer = commandBufferFactory->createCommandBuffer();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphore imageAvailableSemaphore;
        vk(vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore));
        
        VkSemaphore renderFinishedSemaphore;
        vk(vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore));
        
        VkFence inFlightFence;
        vk(vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &inFlightFence));

        VulkanFrame frame;
        frame.commandBuffer = commandBuffer;
        frame.imageAvailableSemaphore = imageAvailableSemaphore;
        frame.renderFinishedSemaphore = renderFinishedSemaphore;
        frame.inFlightFence = inFlightFence;

        m_frames.push_back(frame);
    }
}

RenderDevice::~RenderDevice() {
    delete bufferFactory;
    delete commandBufferFactory;
    delete descriptorSetFactory;
    delete imageFactory;
    delete imageSamplerFactory;
    delete shaderFactory;

    vmaDestroyAllocator(m_allocator);

    for (const VulkanFrame& frame : m_frames) {
        vkDestroySemaphore(m_logicalDevice, frame.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_logicalDevice, frame.renderFinishedSemaphore, nullptr);
        vkDestroyFence(m_logicalDevice, frame.inFlightFence, nullptr);
    }
    m_frames.clear();

    vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
    
    renderPassFactory->destroyRenderPass(m_renderPass);
    //vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);

    destroySwapchain();

    vkDestroyDevice(m_logicalDevice, nullptr);

    if (m_useDebug) {
        vulkanDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void RenderDevice::chooseSurfaceConfig() {
    VulkanSurfaceAvailableConfigs surfaceConfigs;

    vk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceConfigs.capabilities));
    
    u32 formatCount;
    vk(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr));
    surfaceConfigs.formats.resize(formatCount);
    vk(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, surfaceConfigs.formats.data()));

    u32 presentModeCount;
    vk(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr));
    surfaceConfigs.presentModes.resize(presentModeCount);
    vk(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, surfaceConfigs.presentModes.data()));

    //  choose format
    //   start with a default
    m_format = surfaceConfigs.formats[0];

    //   try and find the desired one
    //   should put a check to only do this if the desired one is different
    for (VkSurfaceFormatKHR formatPossible : surfaceConfigs.formats) {
        if (formatPossible.format == VK_FORMAT_B8G8R8A8_SRGB && formatPossible.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            m_format = formatPossible;
            break;
        }
    }

    //  choose a presentation mode
    //   start with the default, this is the only one to be guaranteed by the API
    m_present = VK_PRESENT_MODE_FIFO_KHR;

    //   try and find the desired one
    //   should put a check to only do this if the desired one is different
    for (VkPresentModeKHR presentPossible : surfaceConfigs.presentModes) {
        if (presentPossible== VK_PRESENT_MODE_FIFO_KHR) {
            m_present = presentPossible;
            break;
        }
    }

    //  choose the resolution
    int width, height;
	SDL_GL_GetDrawableSize(m_window->m_sdlWindow, &width, &height);

    m_extent.width = SDL_clamp((u32)width, surfaceConfigs.capabilities.minImageExtent.width, surfaceConfigs.capabilities.maxImageExtent.width);
    m_extent.height = SDL_clamp((u32)height, surfaceConfigs.capabilities.minImageExtent.height, surfaceConfigs.capabilities.maxImageExtent.height);

    //  choose the min image count
    m_minImageCount = surfaceConfigs.capabilities.minImageCount + 1;

    //   0 means there is no limit
    if (surfaceConfigs.capabilities.maxImageCount != 0) {
        m_minImageCount = SDL_min(m_minImageCount, surfaceConfigs.capabilities.maxImageCount);
    }

    //  set the transformation
    m_swapChainTransform = surfaceConfigs.capabilities.currentTransform;
}

void RenderDevice::createSwapchain() {
    // For multi-sampling create offscreen color and depth image / attachments

    // imageFactory->createImage2DEmptyMultisample(
    //     m_extent.width,
    //     m_extent.height,
    //     ImageFormat_RGB_8_SRGB,
    //     ImageSampleCount_4
    // );


    VkSwapchainCreateInfoKHR swapChainInfo{};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.surface = m_surface;
    swapChainInfo.minImageCount = m_minImageCount;
    swapChainInfo.imageFormat = m_format.format;
    swapChainInfo.imageColorSpace = m_format.colorSpace;
    swapChainInfo.imageExtent = m_extent;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainInfo.preTransform = m_swapChainTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.presentMode = m_present;
    swapChainInfo.clipped = true;

    if (m_graphicsQueueIndex != m_presentQueueIndex) {
        u32 queueIndices[] = {
            m_graphicsQueueIndex,
            m_presentQueueIndex
        };

        swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainInfo.queueFamilyIndexCount = 2;
        swapChainInfo.pQueueFamilyIndices = queueIndices;
    }

    else {
        swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    vk(vkCreateSwapchainKHR(m_logicalDevice, &swapChainInfo, nullptr, &m_swapChain));

    // could use factories for this

    u32 imageCount;
    vk(vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, nullptr));
    std::vector<VkImage> images(imageCount);
    vk(vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, images.data()));

    // Create swap chain image views

    for (u32 i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_format.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView view;
        vk(vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &view));

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = static_cast<RenderPassVulkan*>(m_renderPass)->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &view;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        VkFramebuffer framebuffer;
        vk(vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &framebuffer));

        VulkanSwapChainImage image;
        image.image = images[i];
        image.view = view;
        image.framebuffer = framebuffer;

        m_images.push_back(image);
    }
}

void RenderDevice::destroySwapchain() {
    for (VulkanSwapChainImage& image : m_images) {
        vkDestroyFramebuffer(m_logicalDevice, image.framebuffer, nullptr);
        vkDestroyImageView(m_logicalDevice, image.view, nullptr);
        // I don't think you destroy the swap chain images?
    }

    vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);

    m_images.clear();
}

void RenderDevice::recreateSwapchain() {
    vk(vkDeviceWaitIdle(m_logicalDevice));

    destroySwapchain();
    chooseSurfaceConfig();
    createSwapchain();
}

FrameSyncInfo RenderDevice::getFrameSyncInfo() {
    return FrameSyncInfo(&m_currentFrameIndex, &m_currentImageIndex, &m_framesInFlight);
}

ImGuiLoop* RenderDevice::newImGuiLoop() {
    // should track this to make sure that you can only create this one time
    // lil hacky code
    static bool once = false;
    if (once == true) {
        throw std::runtime_error("Can only create one ImGuiLoop");
    }
    once = true;

    return new ImGuiLoop(
        m_window,
        m_instance,
        m_physicalDevice,
        m_logicalDevice,
        static_cast<RenderPassVulkan*>(m_renderPass)->renderPass,
        m_commandPool,
        m_graphicsQueue,
        m_graphicsQueueIndex,
        m_minImageCount,
        commandBufferFactory
    );
}

void RenderDevice::updateDescriptorSet(const VkWriteDescriptorSet& write) {
    vkUpdateDescriptorSets(m_logicalDevice, 1, &write, 0, nullptr);
}

bool RenderDevice::waitBeginFrame(VulkanFrameImage* pFrame) {
    VulkanFrame frame = m_frames[m_currentFrameIndex];

    vk(vkWaitForFences(m_logicalDevice, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX));

    VkResult status = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);

    switch (status) {
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR: {
            m_window->waitViewable();
            recreateSwapchain();
            return false;
        }
        case VK_SUCCESS: {
            break;
        }
        default: {
            throw std::runtime_error("Failed to get swap chain image");
        }
    }

    VkCommandBuffer commandBuffer = static_cast<CommandBufferVulkan*>(frame.commandBuffer)->commandBuffer;

    vk(vkResetFences(m_logicalDevice, 1, &frame.inFlightFence));
    vk(vkResetCommandBuffer(commandBuffer, 0));

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vk(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

    pFrame->commandBuffer = frame.commandBuffer;
    pFrame->width = m_extent.width;
    pFrame->height = m_extent.height;
    pFrame->frameIndex = m_currentFrameIndex;
    pFrame->imageIndex = m_currentImageIndex;

    return true;
}

void RenderDevice::submitFrame() {
    VulkanFrame frame = m_frames[m_currentFrameIndex];
    VkCommandBuffer commandBuffer = static_cast<CommandBufferVulkan*>(frame.commandBuffer)->commandBuffer;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
    VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frame.inFlightFence));

    VkSwapchainKHR swapChains[] = { m_swapChain };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_currentImageIndex;

    // could get status
    // when swapchain is out of date
    vkQueuePresentKHR(m_presentQueue, &presentInfo);

    m_currentFrameIndex = (m_currentFrameIndex + 1) % m_framesInFlight;
}

void RenderDevice::beginScreenRenderPass() {
    VulkanFrame frame = m_frames[m_currentFrameIndex];
    VulkanSwapChainImage image = m_images[m_currentImageIndex];
    VkCommandBuffer commandBuffer = static_cast<CommandBufferVulkan*>(frame.commandBuffer)->commandBuffer;

    //frame.commandBuffer->beginRenderPass(image.framebuffer);
    VkRenderPass renderPass = static_cast<RenderPassVulkan*>(m_renderPass)->renderPass;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = image.framebuffer;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = m_extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport pipelineViewport{};
    pipelineViewport.x = 0.0f;
    pipelineViewport.y = 0.0f;
    pipelineViewport.width = (float)m_extent.width;
    pipelineViewport.height = (float)m_extent.height;
    pipelineViewport.minDepth = 0.0f;
    pipelineViewport.maxDepth = 1.0f;

    VkRect2D pipelineScissor{};
    pipelineScissor.offset = { 0, 0 };
    pipelineScissor.extent = m_extent;
    
    vkCmdSetViewport(commandBuffer, 0, 1, &pipelineViewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &pipelineScissor);
}

void RenderDevice::waitUntilIdle() {
    vk(vkDeviceWaitIdle(m_logicalDevice));
}

void RenderDevice::waitUntilGraphicsQueueIdle() {
    vk(vkQueueWaitIdle(m_graphicsQueue));
}