#pragma once
#include <string_view>
namespace rogue::core {
class Engine {
public:
 bool initialize();
 void update(double delta_seconds);
 void shutdown();
 bool is_initialized() const noexcept { return initialized_; }
 std::string_view name() const noexcept { return "Gearsin"; }
private:
 bool initialized_ = false;
};
}
