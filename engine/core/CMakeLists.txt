add_library(rogue_core STATIC src/engine.cpp)
target_include_directories(rogue_core PUBLIC include)
target_compile_features(rogue_core PUBLIC cxx_std_20)
