add_library(rogue_rendering STATIC
    src/null_renderer.cpp
    src/morph_buffer.cpp
)

target_include_directories(rogue_rendering PUBLIC include)
target_compile_features(rogue_rendering PUBLIC cxx_std_20)
target_link_libraries(rogue_rendering PUBLIC rogue_core)
