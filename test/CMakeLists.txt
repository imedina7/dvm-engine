# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

# ---- Tests ----
if(BUILD_TESTING)
  add_executable(dvm-engine_test my_test.cpp)
  target_link_libraries(dvm-engine_test PRIVATE GTest::gtest_main imgui)
  target_compile_features(dvm-engine_test PRIVATE cxx_std_17)
  target_include_directories(dvm-engine_test PUBLIC ../${IMGUI_PATH})
  include(GoogleTest)
  gtest_discover_tests(dvm-engine_test)

endif()
