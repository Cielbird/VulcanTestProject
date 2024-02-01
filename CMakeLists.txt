# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(VulcanTestProject VERSION 1.0
                          DESCRIPTION "Very nice project where I explore Vulcan"
                          LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Source files
set(SOURCES
    src/main.cpp
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# See these instructions: https://vulkan-tutorial.com/Development_environment#page_MacOS
set(MY_VULKAN_SDK_PATH "/Users/guilhemane/VulkanSDK/1.3.239.0")
# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${MY_VULKAN_SDK_PATH}/macOS/include)

# Link against libraries
target_link_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE libglfw.3.3.dylib)
target_link_directories(${PROJECT_NAME} PRIVATE ${MY_VULKAN_SDK_PATH}/macOS/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE libvulkan.1.3.239.dylib)
target_link_libraries(${PROJECT_NAME} PRIVATE libvulkan.1.dylib)

# Set environment variables for Vulcan
set(ENV{VK_ICD_FILENAMES} ${MY_VULKAN_SDK_PATH}/macOS/share/vulkan/icd.d/MoltenVK_icd.json)
set(ENV{VK_LAYER_PATH} ${MY_VULKAN_SDK_PATH}/macOS/share/vulkan/explicit_layer.d)



# Specify additional libraries if needed
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)
# Output directory for the built executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
