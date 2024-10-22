
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
  
set(INSTALL_TARGETS ${INSTALL_TARGETS} imgui)

set(IMGUI_HEADERS ${IMGUI_PATH}/imgui.h ${IMGUI_PATH}/imconfig.h)

install(TARGETS ${INSTALL_TARGETS}
  EXPORT imgui-targets DESTINATION lib)
install(EXPORT imgui-targets
  FILE imgui-config.cmake
  NAMESPACE imgui::
  DESTINATION lib/cmake/imgui)
  
target_include_directories(imgui PUBLIC )
