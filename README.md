# Asterism ![](https://img.shields.io/github/license/SilvoSposetti/asterism.svg) ![](https://img.shields.io/github/languages/code-size/SilvoSposetti/asterism.svg)

A lightweight Vulkan renderer.

This application is developed for 64-bit systems only.


## Getting Started: [Windows]

* Clone with the `--recurse-submodules` flag. [GLFW](https://github.com/glfw/glfw.git), [GLM](https://github.com/g-truc/glm.git), and [GLSLANG](https://github.com/KhronosGroup/glslang.git) are dependencies linked as git submodules.
                  
* Install [MinGW](https://sourceforge.net/projects/mingw-w64/) as compiler for the project.

* Install the latest [VulkanSDK](https://www.lunarg.com/vulkan-sdk/).

* `bin` is the default build directory. [For CLion] Set, for example, the default generation path to the `bin/mingw-debug` and the `bin/mingw-release` folders respectively (Settings -> Build, Execution, Deployment -> CMake -> Generation Path).


## Folder Structure

    .
    ├── 📁 bin                 # Compiled files (gitignored)
    ├── 📁 ext                 # External dependencies (git submodules)
    ├── 📁 include             # Header files
    ├── 📁 shaders             # Shader Code
    ├── 📁 src                 # Source files
    ├── 🖺 CMakeLists.txt      
    ├── 🖺 LICENSE.md
    └── 🖺 README.md


## Built With

* [Vulkan](https://www.khronos.org/vulkan/) - 3D graphics and computing
* [GLFW](https://www.glfw.org/) - Create and manage windows
* [GLM](https://glm.g-truc.net/) - Linear algebra library
* [GLSLANG](https://github.com/KhronosGroup/glslang.git) - Shader compilation
* [CMake](https://cmake.org/) - Build system


## License

This project is licensed under the GNU GPL v3.0 - see [LICENSE](LICENSE.md) for details.


## Acknowledgments
* I used the amazing [Vulkan Tutorial](https://vulkan-tutorial.com/Introduction) made by [Alexander Overvoorde](https://github.com/Overv) to get started with the Vulkan API.


