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
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
  set(Vulkan_FOUND "True")
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

if(NOT LIBSNDFILE_PATH)
  message(STATUS "LIBSNDFILE_PATH not specified in .env.cmake, using vendor/libsndfile-1.2.2-win64")
  set(LIBSNDFILE_PATH vendor/libsndfile-1.2.2-win64)
endif()

add_library(imgui STATIC
  ${IMGUI_PATH}/imgui.cpp
  ${IMGUI_PATH}/imgui.h
  ${IMGUI_PATH}/imstb_rectpack.h
  ${IMGUI_PATH}/imstb_textedit.h
  ${IMGUI_PATH}/imstb_truetype.h
  ${IMGUI_PATH}/imgui_demo.cpp
  ${IMGUI_PATH}/imgui_draw.cpp
  ${IMGUI_PATH}/imgui_internal.h
  ${IMGUI_PATH}/imgui_tables.cpp
  ${IMGUI_PATH}/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC
  $<BUILD_INTERFACE:${ROOT}>
  $<INSTALL_INTERFACE:include>)


if(PORTAUDIO_ENABLE)
  if(NOT PORTAUDIO_PATH)
    message(STATUS "PORTAUDIO_PATH not specified in .env.cmake, using vendor/portaudio")
    set(PORTAUDIO_PATH vendor/portaudio)
  endif()

  add_library(PortAudio STATIC ${PORTAUDIO_PATH}/src/common/pa_allocation.c
    ${PORTAUDIO_PATH}/src/common/pa_allocation.h
    ${PORTAUDIO_PATH}/src/common/pa_converters.c
    ${PORTAUDIO_PATH}/src/common/pa_converters.h
    ${PORTAUDIO_PATH}/src/common/pa_cpuload.c
    ${PORTAUDIO_PATH}/src/common/pa_cpuload.h
    ${PORTAUDIO_PATH}/src/common/pa_debugprint.c
    ${PORTAUDIO_PATH}/src/common/pa_debugprint.h
    ${PORTAUDIO_PATH}/src/common/pa_dither.c
    ${PORTAUDIO_PATH}/src/common/pa_dither.h
    ${PORTAUDIO_PATH}/src/common/pa_endianness.h
    ${PORTAUDIO_PATH}/src/common/pa_front.c
    ${PORTAUDIO_PATH}/src/common/pa_hostapi.h
    ${PORTAUDIO_PATH}/src/common/pa_memorybarrier.h
    ${PORTAUDIO_PATH}/src/common/pa_process.c
    ${PORTAUDIO_PATH}/src/common/pa_process.h
    ${PORTAUDIO_PATH}/src/common/pa_ringbuffer.c
    ${PORTAUDIO_PATH}/src/common/pa_ringbuffer.h
    ${PORTAUDIO_PATH}/src/common/pa_stream.c
    ${PORTAUDIO_PATH}/src/common/pa_stream.h
    ${PORTAUDIO_PATH}/src/common/pa_trace.c
    ${PORTAUDIO_PATH}/src/common/pa_trace.h
    ${PORTAUDIO_PATH}/src/common/pa_types.h
    ${PORTAUDIO_PATH}/src/common/pa_util.h
  )
  if(UNIX)
    target_compile_options(PortAudio PRIVATE -fPIC)
  endif()
endif()

if(PORTAUDIO_ENABLE)
  set(INSTALL_TARGETS imgui
    PortAudio)

  if(WIN32)
    target_sources(PortAudio PRIVATE
      ${PORTAUDIO_PATH}/src/os/win/pa_win_coinitialize.c
      ${PORTAUDIO_PATH}/src/os/win/pa_win_coinitialize.h
      ${PORTAUDIO_PATH}/src/os/win/pa_win_hostapis.c
      ${PORTAUDIO_PATH}/src/os/win/pa_win_util.c
      ${PORTAUDIO_PATH}/src/os/win/pa_win_util.h
      ${PORTAUDIO_PATH}/src/os/win/pa_win_version.c
      ${PORTAUDIO_PATH}/src/os/win/pa_win_version.h
      ${PORTAUDIO_PATH}/src/os/win/pa_win_waveformat.c
      ${PORTAUDIO_PATH}/src/os/win/pa_win_wdmks_utils.h
      ${PORTAUDIO_PATH}/src/os/win/pa_x86_plain_converters.h
    )
    target_include_directories(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/common ${PORTAUDIO_PATH}/include ${PORTAUDIO_PATH}/src/os/win)
    set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_win_waveformat.h)
    target_link_libraries(PortAudio PRIVATE winmm)

    if(MSVC)
      target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/os/win/pa_x86_plain_converters.c)
    else()
      target_compile_definitions(PortAudio PRIVATE _WIN32_WINNT=0x0501 WINVER=0x0501)
      set(DEF_EXCLUDE_X86_PLAIN_CONVERTERS ";")
    endif()

    target_compile_definitions(PortAudio PRIVATE _CRT_SECURE_NO_WARNINGS)

    option(PA_USE_ASIO "Enable support for ASIO" OFF)
    if(PA_USE_ASIO)
      find_package(ASIO)
      # Automatically download the ASIO SDK ZIP if it is not found. The ASIO SDK license
      # allows for downloading it from Steinberg and using it without charge, but it is
      # not allowed to be redistributed.
      #
      # The file(ARCHIVE_EXTRACT) command needed to extract the ZIP archive was added in
      # CMake 3.18, so do not bother downloading the ZIP archive for older CMake versions.
      # Instead, FindASIO.cmake directs the user to manually extract the ZIP file to
      # CMAKE_PREFIX_PATH or CMAKE_CURRENT_BINARY_DIR.
      if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18 AND NOT TARGET ASIO::host)
        if(NOT ASIO_SDK_ZIP_PATH)
          set(ASIO_SDK_ZIP_PATH "${CMAKE_CURRENT_BINARY_DIR}/asiosdk.zip")
        endif()
        message(STATUS "Downloading ASIO SDK... ${ASIO_SDK_ZIP_PATH}")
        file(DOWNLOAD "https://www.steinberg.net/asiosdk"
          "${ASIO_SDK_ZIP_PATH}"
          STATUS ASIO_DOWNLOAD_STATUS
          SHOW_PROGRESS
        )
        if("${ASIO_DOWNLOAD_STATUS}" EQUAL 0)
          find_package(ASIO)
        else()
          list(GET "${ASIO_DOWNLOAD_STATUS}" 1 DOWNLOAD_ERROR)
          message(FATAL_ERROR "Error downloading ASIO SDK: ${DOWNLOAD_ERROR} "
            "Reconfigure CMake with -DPA_USE_ASIO=OFF to build without ASIO. "
            "Alternatively, download the ZIP from https://www.steinberg.net/asiosdk "
            "and put it in ${CMAKE_PREFIX_PATH} or ${CMAKE_CURRENT_BINARY_DIR}"
          )
        endif()
      endif()
    endif()
    if(PA_USE_ASIO AND TARGET ASIO::host)
      target_link_libraries(PortAudio PRIVATE "$<BUILD_INTERFACE:ASIO::host>")
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_asio.h)
      target_compile_definitions(PortAudio PUBLIC PA_USE_ASIO=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_ASIO=1")
      target_sources(PortAudio PRIVATE
        ${PORTAUDIO_PATH}/src/hostapi/asio/pa_asio.cpp
        ${PORTAUDIO_PATH}/src/hostapi/asio/iasiothiscallresolver.cpp
        ${PORTAUDIO_PATH}/src/hostapi/asio/iasiothiscallresolver.h
      )
    else()
      set(DEF_EXCLUDE_ASIO_SYMBOLS ";")
    endif()

    option(PA_USE_DS "Enable support for DirectSound" ON)
    if(PA_USE_DS)
      target_sources(PortAudio PRIVATE
        ${PORTAUDIO_PATH}/src/hostapi/dsound/pa_win_ds.c
        ${PORTAUDIO_PATH}/src/hostapi/dsound/pa_win_ds_dynlink.c
        ${PORTAUDIO_PATH}/src/hostapi/dsound/pa_win_ds_dynlink.h
      )
      target_include_directories(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/common ${PORTAUDIO_PATH}/include ${PORTAUDIO_PATH}/src/hostapi/dsound)
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_win_ds.h)
      target_compile_definitions(PortAudio PUBLIC PA_USE_DS=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_DS=1")
      target_link_libraries(PortAudio PRIVATE dsound)
      if(NOT MINGW)
        target_compile_definitions(PortAudio PRIVATE PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE)
      endif()
    endif()

    option(PA_USE_WMME "Enable support for WMME" ON)
    if(PA_USE_WMME)
      target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/hostapi/wmme/pa_win_wmme.c)
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_win_wmme.h)
      target_compile_definitions(PortAudio PUBLIC PA_USE_WMME=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_WMME=1")
      target_link_libraries(PortAudio PRIVATE ole32 uuid)
    else()
      set(DEF_EXCLUDE_WMME_SYMBOLS ";")
    endif()

    option(PA_USE_WASAPI "Enable support for WASAPI" ON)
    if(PA_USE_WASAPI)
      target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/hostapi/wasapi/pa_win_wasapi.c)
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_win_wasapi.h)
      target_compile_definitions(PortAudio PUBLIC PA_USE_WASAPI=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_WASAPI=1")
      target_link_libraries(PortAudio PRIVATE ole32 uuid)
    else()
      set(DEF_EXCLUDE_WASAPI_SYMBOLS ";")
    endif()

    option(PA_USE_WDMKS "Enable support for WDMKS" ON)
    if(PA_USE_WDMKS)
      target_sources(PortAudio PRIVATE
        ${PORTAUDIO_PATH}/src/os/win/pa_win_wdmks_utils.c
        ${PORTAUDIO_PATH}/src/hostapi/wdmks/pa_win_wdmks.c
      )
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_win_wdmks.h)
      target_compile_definitions(PortAudio PUBLIC PA_USE_WDMKS=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_WDMKS=1")
      target_link_libraries(PortAudio PRIVATE setupapi ole32 uuid)
    endif()

    option(PA_USE_WDMKS_DEVICE_INFO "Use WDM/KS API for device info" ON)
    if(PA_USE_WDMKS_DEVICE_INFO)
      target_compile_definitions(PortAudio PRIVATE PAWIN_USE_WDMKS_DEVICE_INFO)
    endif()

    if(PA_BUILD_SHARED_LIBS)
      configure_file(cmake/portaudio.def.in "${CMAKE_CURRENT_BINARY_DIR}/portaudio.def" @ONLY)
      target_sources(PortAudio PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/portaudio.def")
    endif()
  elseif(UNIX)
    target_sources(PortAudio PRIVATE
      ${PORTAUDIO_PATH}/src/os/unix/pa_unix_hostapis.c
      ${PORTAUDIO_PATH}/src/os/unix/pa_unix_util.c
      ${PORTAUDIO_PATH}/src/os/unix/pa_unix_util.h
    )
    target_include_directories(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/common ${PORTAUDIO_PATH}/include ${PORTAUDIO_PATH}/src/os/unix)
    target_link_libraries(PortAudio PRIVATE m)
    set(PKGCONFIG_LDFLAGS_PRIVATE "${PKGCONFIG_LDFLAGS_PUBLIC} -lm -lpthread")
    set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -pthread")

    if(APPLE)
      set(CMAKE_MACOSX_RPATH 1)
      target_sources(PortAudio PRIVATE
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core.c
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core_blocking.c
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core_blocking.h
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core_internal.h
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core_utilities.c
        ${PORTAUDIO_PATH}/src/hostapi/coreaudio/pa_mac_core_utilities.h
      )
      target_include_directories(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/common ${PORTAUDIO_PATH}/include ${PORTAUDIO_PATH}/src/hostapi/coreaudio)
      set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_mac_core.h)

      target_link_libraries(PortAudio
        PRIVATE
        -Wl,-framework,CoreAudio
        -Wl,-framework,AudioToolbox
        -Wl,-framework,AudioUnit
        -Wl,-framework,CoreFoundation
        -Wl,-framework,CoreServices
      )
      target_compile_definitions(PortAudio PUBLIC PA_USE_COREAUDIO=1)
      set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_COREAUDIO=1")

      # Use C11 so that we can make use of atomic library and avoid deprecation errors.
      set_property(TARGET PortAudio PROPERTY C_STANDARD 11)

      set(PKGCONFIG_LDFLAGS_PRIVATE
        "${PKGCONFIG_LDFLAGS_PRIVATE} -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreFoundation -framework CoreServices")
    else()
      include(CMakeDependentOption)
      # Some BSDs have a reimplementation of alsalib, so do not explicitly check for Linux.
      find_package(ALSA)
      cmake_dependent_option(PA_USE_ALSA "Enable support for ALSA" ON ALSA_FOUND OFF)
      if(PA_USE_ALSA)
        target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/hostapi/alsa/pa_linux_alsa.c)
        set(PORTAUDIO_PUBLIC_HEADERS "${PORTAUDIO_PUBLIC_HEADERS}" ${PORTAUDIO_PATH}/include/pa_linux_alsa.h)
        target_compile_definitions(PortAudio PUBLIC PA_USE_ALSA=1)
        set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_ALSA=1")

        option(PA_ALSA_DYNAMIC "Enable dynamically loading libasound with dlopen using PaAlsa_SetLibraryPathName" OFF)
        if(PA_ALSA_DYNAMIC)
          target_compile_definitions(PortAudio PRIVATE PA_ALSA_DYNAMIC)
          target_link_libraries(PortAudio PRIVATE "${CMAKE_DL_LIBS}")
          set(PKGCONFIG_LDFLAGS_PRIVATE "${PKGCONFIG_LDFLAGS_PRIVATE} -l${CMAKE_DL_LIBS}")
        else()
          target_link_libraries(PortAudio PRIVATE "${ALSA_LIBRARIES}")
          set(PKGCONFIG_REQUIRES_PRIVATE "${PKGCONFIG_REQUIRES_PRIVATE} alsa")
        endif()
      endif()

      # OSS is intentionally off by default to avoid confusing users of PortAudio
      # applications. OSS builds but there are no devices available on modern
      # Linux systems.
      find_package(OSS)
      cmake_dependent_option(PA_USE_OSS "Enable support for OSS" OFF "OSS_FOUND" OFF)
      if(PA_USE_OSS)
        target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/hostapi/oss/pa_unix_oss.c)
        target_compile_definitions(PortAudio PUBLIC PA_USE_OSS=1)
        set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_OSS=1")
        target_link_libraries(PortAudio PRIVATE OSS::oss)
        # The FindOSS.cmake module does not need to be installed like the JACK modules because it
        # does not link any library; it only adds an include directory and compile definition.
      endif()

      check_include_file(sys/audioio.h HAVE_SYS_AUDIOIO_H)
      cmake_dependent_option(AUDIOIO "Enable support for Solaris/NetBSD audio" ON "HAVE_SYS_AUDIOIO_H" AUDIOIO)
      if(AUDIOIO AND HAVE_SYS_AUDIOIO_H)
        target_sources(PortAudio PRIVATE ${PORTAUDIO_PATH}/src/hostapi/audioio/pa_unix_audioio.c)
        target_compile_definitions(PortAudio PUBLIC PA_USE_AUDIOIO=1)
        set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_AUDIOIO=1")
      endif()

      find_package(PulseAudio)
      cmake_dependent_option(PA_USE_PULSEAUDIO "Enable support for PulseAudio general purpose sound server" ON PulseAudio_FOUND OFF)
      if(PA_USE_PULSEAUDIO)
        target_link_libraries(PortAudio PRIVATE PulseAudio::PulseAudio)
        target_sources(PortAudio PRIVATE
          ${PORTAUDIO_PATH}/src/hostapi/pulseaudio/pa_linux_pulseaudio_block.c
          ${PORTAUDIO_PATH}/src/hostapi/pulseaudio/pa_linux_pulseaudio.c
          ${PORTAUDIO_PATH}/src/hostapi/pulseaudio/pa_linux_pulseaudio_cb.c)

        target_compile_definitions(PortAudio PUBLIC PA_USE_PULSEAUDIO=1)
        set(PKGCONFIG_CFLAGS "${PKGCONFIG_CFLAGS} -DPA_USE_PULSEAUDIO=1")
        set(PKGCONFIG_REQUIRES_PRIVATE "${PKGCONFIG_REQUIRES_PRIVATE} libpulse")

        # needed for PortAudioConfig.cmake so `find_package(PortAudio)` works in downstream projects
        install(FILES cmake/modules/FindPulseAudio.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/portaudio/modules")
      endif()

    endif()
  endif()
else()
  set(INSTALL_TARGETS imgui)
endif()

set(IMGUI_HEADERS ${IMGUI_PATH}/imgui.h ${IMGUI_PATH}/imconfig.h)

install(TARGETS ${INSTALL_TARGETS}
  EXPORT imgui-targets DESTINATION lib)
install(EXPORT imgui-targets
  FILE imgui-config.cmake
  NAMESPACE imgui::
  DESTINATION lib/cmake/imgui)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(imgui PUBLIC )
target_link_libraries(${PROJECT_NAME} imgui)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_BUILD_TYPE Debug)

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

  target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1 imgui PortAudio sndfile)
elseif(UNIX)
  message(STATUS "CREATING BUILD FOR UNIX")

  if(APPLE)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
  
  target_include_directories(${PROJECT_NAME} PUBLIC
    /usr/include
    ${PROJECT_SOURCE_DIR}/src
    ${TINYOBJ_PATH}
    ${IMGUI_PATH}
    ${PORTAUDIO_PATH}/include
    ${PORTAUDIO_PATH}/src/common
  )
  target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
    lib/cmake/imgui)

  target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES} portaudio sndfile)
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
