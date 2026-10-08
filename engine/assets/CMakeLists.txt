add_library(rogue_assets STATIC src/rb_container.cpp)
target_include_directories(rogue_assets PUBLIC include)
target_compile_features(rogue_assets PUBLIC cxx_std_20)
