@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

SET GLFW_PATH="C:\VulkanSDK\Libraries\glfw-3.3.9.bin.WIN64"

cmake --build build