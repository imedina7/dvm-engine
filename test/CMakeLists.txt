# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(dvm-engineTests LANGUAGES CXX)

# ---- Tests ----

add_executable(dvm-engine_test source/dvm-engine_test.cpp)
target_link_libraries(dvm-engine_test PRIVATE dvm-engine_lib)
target_compile_features(dvm-engine_test PRIVATE cxx_std_17)

add_test(NAME dvm-engine_test COMMAND dvm-engine_test)

# ---- End-of-file commands ----

add_folders(Test)
