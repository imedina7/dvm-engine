# Clone the repo

```bash
git clone --recursive https://github.com/imedina7/dvm-engine.git
```

# Dependencies
Most dependencies are included as submodules,
but there are some that are expected to be installed in the system.

- Vulkan SDK
- GLFW
- GLM
- libsndfile (optional)
- Portaudio (optional)

# Building with CMake

## Environment

Set up a `.env.cmake` file in the root of the project with the following:

```CMake
set(GLFW_PATH <path_to_glfw>)
set(GLM_PATH <path_to_glm>)
set(VULKAN_SDK_PATH <path_to_vulkan_sdk>)
set(PORTAUDIO_ENABLE false) # set to true if you want audio support
set(AUDIO_ENGINE false) # set to true if you want audio support
```

## Build

This project doesn't require any special command-line flags to build to keep
things simple.

Here are the steps for building in release mode with a single-configuration
generator, like the Unix Makefiles one:

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

Here are the steps for building in release mode with a multi-configuration
generator, like the Visual Studio ones:

```sh
cmake -S . -B build
cmake --build build --config Release
```

### Building with MSVC

Note that MSVC by default is not standards compliant and you need to pass some
flags to make it behave properly. See the `flags-windows` preset in the
[CMakePresets.json](CMakePresets.json) file for the flags and with what
variable to provide them to CMake during configuration.

### Building on Apple Silicon

CMake supports building on Apple Silicon properly since 3.20.1. Make sure you
have the [latest version][1] installed.

## Install

This project doesn't require any special command-line flags to install to keep
things simple. As a prerequisite, the project has to be built with the above
commands already.

The below commands require at least CMake 3.15 to run, because that is the
version in which [Install a Project][2] was added.

Here is the command for installing the release mode artifacts with a
single-configuration generator, like the Unix Makefiles one:

```sh
cmake --install build
```

Here is the command for installing the release mode artifacts with a
multi-configuration generator, like the Visual Studio ones:

```sh
cmake --install build --config Release
```

[1]: https://cmake.org/download/
[2]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
