add_executable(rogue_assets_smoke
    profile_smoke.cpp
)

target_link_libraries(rogue_assets_smoke PRIVATE rogue_assets)
target_compile_features(rogue_assets_smoke PRIVATE cxx_std_20)

add_test(NAME rogue_assets_smoke COMMAND rogue_assets_smoke)
