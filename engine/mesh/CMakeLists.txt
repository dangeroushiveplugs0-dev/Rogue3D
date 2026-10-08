add_library(rogue_mesh STATIC
    src/mesh.cpp
)

target_include_directories(rogue_mesh PUBLIC include)
target_compile_features(rogue_mesh PUBLIC cxx_std_20)
