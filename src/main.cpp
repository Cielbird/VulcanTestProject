#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>

const uint32_t WIDTH = 600;
const uint32_t HEIGHT = 800;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    /// @brief Checks if all the required queue families are set
    /// @return True if all required queue family indices are set
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    void initWindow() {
        glfwInit();
        // glfw by default expects tu use OpenGL, which is not the case.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "My Window - Vulcan", nullptr, nullptr);
    }

    /// @brief pisscock
    void initVulkan() {
        createInstance();
        pickPhysicalDevice();
    }

    /// @brief Runs as long as the window is open. Main loop.
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    /// @brief Cleans up anything before closing
    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    /// @brief Creates a Vulkan instance
    void createInstance(){
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        // tells the vulcan driver which extentions and validation layers the program needs
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.enabledLayerCount = 0;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> requiredExtensions;
        for(uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtensions.emplace_back(glfwExtensions[i]);
        }
        // from 1.3.216 Vulkan SDK, VK_KHR_PORTABILITY_subset extension is mandatory.
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("extensions requested, but not available!");
        }
        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        if (enableValidationLayers)
            std::cout << "Debugging" << std::endl;
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    /// @brief Checks if the required extensions are available. Prints info if otherwise
    /// @param requiredExtensions Vector of required extension names
    /// @return False if any extensions are missing
    bool checkReqExtensions(const std::vector<const char*>& requiredExtensions) {
        // get available extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availExtensions (extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availExtensions.data());
        bool areAnyMissing = false;
        for (const char* requiredExtension : requiredExtensions){
            // check if required extension is available
            bool extFound = false;
            for (VkExtensionProperties availExtension : availExtensions){
                if (strcmp(requiredExtension, availExtension.extensionName) == 0) {
                    extFound = true;
                    break;
                }
            }
            if (!extFound){
                areAnyMissing = true;
                std::cout << "Missing required extension: " << requiredExtension << std::endl;
            }
        }
        return !areAnyMissing;
    }

    /// @brief Checks if the required validation layers are available. Prints info if otherwise
    /// @param requiredExtensions Vector of required layer names
    /// @return False if any layers are missing
    bool checkValidationLayerSupport() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> layers (layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
        bool areAnyMissing = false;
        for (const char* requiredLayer : validationLayers){
            // check if required validation layer is available
            bool layerFound = false;
            for (VkLayerProperties currentLayer : layers){
                if (strcmp(requiredLayer, currentLayer.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound){
                areAnyMissing = true;
                std::cout << "Missing required validation layer: " << requiredLayer << std::endl;
            }
        }
        return !areAnyMissing;
    }

    /// @brief Selects a device (GPU) for use. Checks for device suitability.
    void pickPhysicalDevice() {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0){
            throw std::runtime_error("Unable to find any GPU devices with Vulcan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        // score and pick best GPU
        VkPhysicalDevice bestPhysicalDevice;
        uint32_t bestScore = 0; 
        for (const VkPhysicalDevice& device : devices) {
            uint32_t score = scorePhysicalDeviceSuitability(device);
            if (score == 0) continue; // score of 0 means unsuitable GPU
            if (score > bestScore){
                bestPhysicalDevice = device;
                bestScore = score;
            }
        }

        if (bestPhysicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable GPU!");
        
    }

    /// @brief Scores the suitability of the GPU for the application to chose the best for the job.
    /// @param device The GPU we are interested in checking
    /// @return An int for the score. 0 means unusable, highest score is best.
    uint32_t scorePhysicalDeviceSuitability (const VkPhysicalDevice& device) {
        // get properties and features
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(device, &properties);
        vkGetPhysicalDeviceFeatures(device, &features);
        // get phyiscal device queue families (indices of the family of each type we need)
        QueueFamilyIndices queueFamilyIndices;
        getQueueFamilyIndices(device, queueFamilyIndices);

        // baseline requirements
        if (!queueFamilyIndices.isComplete())
            return 0;

        uint32_t score = 0;
        // discrete gpus are much better
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 100;
        // Maximum possible size of textures affects graphics quality
        score += properties.limits.maxImageDimension2D;
        return score;
    }

    void getQueueFamilyIndices(const VkPhysicalDevice& device, QueueFamilyIndices& queueFamilies){
        uint32_t famCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, nullptr);
        std::vector<VkQueueFamilyProperties> fams(famCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, fams.data());

        for(int i=0; i<famCount; ++i)
        {
            if ((fams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                queueFamilies.graphicsFamily = i;
                continue;
            if (queueFamilies.isComplete())
                break;
        }
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}