
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

set(INSTALL_TARGETS PortAudio)

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
