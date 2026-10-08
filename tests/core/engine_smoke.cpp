#include <cassert>
#include "rogue/core/engine.hpp"
#include "rogue/core/version.hpp"
int main() { rogue::core::Engine e; assert(!e.is_initialized()); assert(e.initialize()); e.update(1.0/60.0); e.shutdown(); assert(!e.is_initialized()); return 0; }
