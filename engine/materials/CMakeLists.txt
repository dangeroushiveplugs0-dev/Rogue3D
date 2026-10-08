add_library(rogue_materials STATIC
    src/pbr_material.cpp
)

target_include_directories(rogue_materials PUBLIC include)
target_compile_features(rogue_materials PUBLIC cxx_std_20)
