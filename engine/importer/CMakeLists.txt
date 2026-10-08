add_library(rogue_importer STATIC
    src/asset_importer.cpp
)

target_include_directories(rogue_importer PUBLIC include)
target_compile_features(rogue_importer PUBLIC cxx_std_20)
target_link_libraries(rogue_importer PUBLIC rogue_mesh rogue_materials)
