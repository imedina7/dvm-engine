include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)

set(NAME DvmEngine)

message(STATUS "using ${CMAKE_GENERATOR}")
if(CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
  if(NOT MINGW_PATH)
    message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
  endif()
  set(USE_MINGW "True")
  set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
  set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)
endif()

project(${NAME} VERSION 0.23.0)

# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if(DEFINED VULKAN_SDK_PATH)
  if(APPLE)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/libexec/include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
  elseif(WIN32)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
  else()
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
  endif()
else()
  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if(NOT Vulkan_FOUND)
  message(FATAL_ERROR "Could not find Vulkan library!")
else()
  message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()


# 2. Set GLFW_PATH in .env.cmake to target specific glfw
if(DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  if(MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2022") # 2.1 Update lib-vc2019 to use same version as your visual studio
  elseif(CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
  else()
    set(GLFW_LIB "${GLFW_PATH}/lib") # 2.1 make sure matches glfw subdirectory
  endif()
else()
  find_package(glfw3 3.3 REQUIRED)
  set(GLFW_LIB glfw)
  message(STATUS "Found GLFW")
endif()
if(NOT GLFW_LIB)
  message(FATAL_ERROR "Could not find glfw library!")
else()
  message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

include_directories(vendor)

# If TINYOBJ_PATH not specified in .env.cmake, try fetching from git repo
if(NOT TINYOBJ_PATH)
  message(STATUS "TINYOBJ_PATH not specified in .env.cmake, using vendor/tinyobjloader")
  set(TINYOBJ_PATH vendor/tinyobjloader)
endif()

if(NOT IMGUI_PATH)
  message(STATUS "IMGUI_PATH not specified in .env.cmake, using vendor/imgui")
  set(IMGUI_PATH vendor/imgui)
endif()

include(cmake/imgui.cmake)


if(NOT LIBSNDFILE_PATH)
  message(STATUS "LIBSNDFILE_PATH not specified in .env.cmake, using vendor/libsndfile-1.2.2-win64")
  set(LIBSNDFILE_PATH vendor/libsndfile-1.2.2-win64)
endif()

if(NOT PORTAUDIO_PATH)
  message(STATUS "PORTAUDIO_PATH not specified in .env.cmake, using vendor/portaudio")
  set(PORTAUDIO_PATH vendor/portaudio)
endif()

if(AUDIO_ENGINE AND PORTAUDIO_ENABLE)
  include(cmake/portaudio.cmake)
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_BUILD_TYPE Debug)


target_link_libraries(${PROJECT_NAME} imgui)

if(WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")

  if(USE_MINGW)
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/lib
    )
  endif()

  if(AUDIO_ENGINE)
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
      ${Vulkan_INCLUDE_DIRS}
      ${TINYOBJ_PATH}
      ${GLFW_INCLUDE_DIRS}
      ${GLM_PATH}
      ${IMGUI_PATH}
      ${PORTAUDIO_PATH}/include
      ${LIBSNDFILE_PATH}/include
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
      ${Vulkan_LIBRARIES}
      ${GLFW_LIB}
      lib/cmake/imgui
      ${LIBSNDFILE_PATH}/lib
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAUDIO=1")

    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1 imgui PortAudio sndfile)
  else()
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
      ${Vulkan_INCLUDE_DIRS}
      ${TINYOBJ_PATH}
      ${GLFW_INCLUDE_DIRS}
      ${GLM_PATH}
      ${IMGUI_PATH}
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
      ${Vulkan_LIBRARIES}
      ${GLFW_LIB}
      lib/cmake/imgui
    )

    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1 imgui PortAudio sndfile)

  endif()
elseif(UNIX)
  message(STATUS "CREATING BUILD FOR UNIX")

  if(APPLE)
    message(STATUS "CREATING BUILD FOR APPLE MACOS")
    set(CMAKE_CXX_COMPILER clang++)

    if(AUDIO_ENGINE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DMACOS=1 -DAUDIO=1")
      target_include_directories(${PROJECT_NAME} PUBLIC
        /usr/include
        /opt/homebrew/include
        ${PROJECT_SOURCE_DIR}/src
        ${TINYOBJ_PATH}
        ${IMGUI_PATH}
        ${PORTAUDIO_PATH}/include
        ${PORTAUDIO_PATH}/src/common
        ${Vulkan_INCLUDE_DIRS}
        )
      target_link_directories(${PROJECT_NAME} PUBLIC
        ${GLFW_LIB}
        ${Vulkan_LIBRARIES}
        ${PORTAUDIO_PATH}/lib
        ${LIBSNDFILE_PATH}/lib
        build
      )

      target_link_libraries(${PROJECT_NAME}
        glfw
        portaudio
        imgui
        sndfile
        MoltenVK
        "-framework AVFoundation"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework Foundation"
        "-framework AppKit"
        "-framework IOKit"
        "-framework IOSurface"
        "-framework MediaPlayer"
        "-framework QuartzCore"
      )
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DMACOS=1")
      target_include_directories(${PROJECT_NAME} PUBLIC
        /usr/include
        /opt/homebrew/include
        ${PROJECT_SOURCE_DIR}/src
        ${TINYOBJ_PATH}
        ${IMGUI_PATH}
        ${Vulkan_INCLUDE_DIRS}
      )
      target_link_directories(${PROJECT_NAME} PUBLIC
        ${GLFW_LIB}
        ${Vulkan_LIBRARIES}
        build
      )
      
      target_link_libraries(${PROJECT_NAME}
        glfw3
        imgui
        MoltenVK
        "-framework AVFoundation"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework Foundation"
        "-framework AppKit"
        "-framework IOKit"
        "-framework IOSurface"
        "-framework MediaPlayer"
        "-framework QuartzCore"
      )
      
    endif()
  else()

  target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
    lib/cmake/imgui)
  
  if(AUDIO_ENGINE)
    target_include_directories(${PROJECT_NAME} PUBLIC
      /usr/include
      ${PROJECT_SOURCE_DIR}/src
      ${TINYOBJ_PATH}
      ${IMGUI_PATH}
      ${PORTAUDIO_PATH}/include
      ${PORTAUDIO_PATH}/src/common
    )
    
      target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES} portaudio sndfile)
    else()
      target_include_directories(${PROJECT_NAME} PUBLIC
        /usr/include
        ${PROJECT_SOURCE_DIR}/src
        ${TINYOBJ_PATH}
        ${IMGUI_PATH}
      )
      target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES})
    endif()
  endif()
  else()
  message(FATAL_ERROR "Unsupported Platform")
endif()

# Google Test library
if(UNIT_TESTS)
  find_package(GTest REQUIRED)
  include(CTest)
  include(FetchContent)
  enable_testing()
  add_subdirectory(test)
  FetchContent_Declare(
    googletest
    # Specify the commit you depend on and update it regularly.
    URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

endif()


############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
  /usr/bin
  /usr/local/bin
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/
  $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
  Shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)
