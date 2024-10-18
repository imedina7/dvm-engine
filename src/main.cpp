
#include "dvm_app.hpp"

#include <exception>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
  dvm::DvmApp& app = dvm::DvmApp::getInstance();
  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}