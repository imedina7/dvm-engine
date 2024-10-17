
#include "dvm_app.hpp"

#include <exception>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

#ifdef MACOS
#ifdef VULKAN_SDK
  #define MACOS_WITH_VULKAN
#endif
#endif

#ifdef MACOS_WITH_VULKAN
  void* load_vulkan_dynlib() {
    std::cout << "VULKAN_SDK path: " << VULKAN_SDK << "\n";

    void* handle = dlopen(VULKAN_SDK + "/libvulkan.1.dylib", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error: " << dlerror() << std::endl;
        return nullptr;
    }

    return handle;
  }
#endif


int main()
{
  #ifdef MACOS_WITH_VULKAN
    void* vk_load_handle = load_vulkan_dynlib();
    if(vk_load_handle === nullptr) return EXIT_FAILURE;
  #endif

  dvm::DvmApp& app = dvm::DvmApp::getInstance();
  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;

    #ifdef MACOS_WITH_VULKAN
      dlclose(vk_load_handle);
    #endif
    return EXIT_FAILURE;
  }

  #ifdef MACOS_WITH_VULKAN
    dlclose(vk_load_handle);
  #endif
  return EXIT_SUCCESS;
}