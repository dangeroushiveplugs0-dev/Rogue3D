add_library(rogue_scene STATIC
    src/scene.cpp
)

target_include_directories(rogue_scene PUBLIC include)
target_compile_features(rogue_scene PUBLIC cxx_std_20)
target_link_libraries(rogue_scene PUBLIC rogue_core)
