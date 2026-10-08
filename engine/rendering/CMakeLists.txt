option(ROGUE_ENABLE_VULKAN "Build the optional Vulkan rendering backend" OFF)
add_library(rogue_rendering STATIC src/null_renderer.cpp src/morph_buffer.cpp)
target_include_directories(rogue_rendering PUBLIC include)
target_compile_features(rogue_rendering PUBLIC cxx_std_20)
target_link_libraries(rogue_rendering PUBLIC rogue_core)
if(ROGUE_ENABLE_VULKAN)
 find_package(Vulkan REQUIRED)
 add_library(rogue_rendering_vulkan STATIC
  vulkan/vulkan_morph_buffer.cpp vulkan/vulkan_morph_evaluator.cpp vulkan/vulkan_upload_buffer.cpp
  vulkan/vulkan_device_buffer.cpp vulkan/vulkan_buffer_copy.cpp vulkan/vulkan_morph_packing.cpp
  vulkan/vulkan_mesh_buffer.cpp vulkan/vulkan_mesh_pipeline.cpp vulkan/vulkan_shader_module.cpp
  vulkan/vulkan_runtime.cpp vulkan/vulkan_swapchain.cpp vulkan/vulkan_frame.cpp vulkan/vulkan_device.cpp
  vulkan/vulkan_frame_coordinator.cpp)
 target_include_directories(rogue_rendering_vulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
 target_compile_features(rogue_rendering_vulkan PUBLIC cxx_std_20)
 target_compile_definitions(rogue_rendering_vulkan PUBLIC ROGUE_ENABLE_VULKAN=1)
 target_link_libraries(rogue_rendering_vulkan PUBLIC rogue_rendering Vulkan::Vulkan)
endif()