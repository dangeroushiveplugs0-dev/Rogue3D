option(ROGUE_ENABLE_ANDROID_VULKAN "Build the Android Vulkan surface adapter" OFF)
if(ROGUE_ENABLE_ANDROID_VULKAN)
 add_subdirectory(android)
endif()
