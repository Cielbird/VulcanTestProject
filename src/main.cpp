#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 600;
const uint32_t HEIGHT = 800;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
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