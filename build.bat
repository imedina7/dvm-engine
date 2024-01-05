@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

SET GLFW_PATH="C:\VulkanSDK\Libraries\glfw-3.3.9.bin.WIN64"

SET includes=/Isrc /I%VULKAN_SDK%/Include /Isrc /I%GLFW_PATH%/include
SET links=/link /LIBPATH:%VULKAN_SDK%/Lib /LIBPATH:%GLFW_PATH%/lib-vc2022 vulkan-1.lib glfw3_mt.lib
SET defines=/D DEBUG

echo "Building main..."

cl/EHsc %includes% %defines% src/main.cpp %links%