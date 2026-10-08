#pragma once

namespace rogue::rendering::vulkan {

// Vulkan morph evaluation interface. The implementation owns descriptor state
// and dispatches the compute evaluator against caller-provided position buffers.
class VulkanMorphEvaluator;

}
