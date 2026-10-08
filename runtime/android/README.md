# Android Vulkan runtime adapter

This layer owns the Android-specific ANativeWindow to VkSurfaceKHR bridge.

Ownership remains separated:
- Android owns the native window lifecycle.
- VulkanAndroidSurface owns the Vulkan surface.
- VulkanDevice, VulkanSwapchain, and VulkanFrameCoordinator remain platform-neutral.

The adapter does not create an Android Activity or UI, so it can be hosted by Kotlin/Compose or a native activity.
