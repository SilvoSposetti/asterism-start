# Asterism ![](https://img.shields.io/github/license/SilvoSposetti/asterism.svg) ![](https://img.shields.io/github/repo-size/SilvoSposetti/asterism.svg) ![](https://img.shields.io/github/languages/code-size/SilvoSposetti/asterism.svg)

A lightweight Vulkan renderer.

This application is developed for 64-bit systems only.

## Getting Started: [Windows]

* Install [MinGW](https://sourceforge.net/projects/mingw-w64/).

* Install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).

* The precompiled binaries and include headers for [GLFW](https://www.glfw.org/download.html) are already bundled in the repo, under `ext/glfw`.

* [For the CLion editor]. Open the existing CMAKE project in the root folder, then set the default generation path to the `bin/mingw-debug` and the `bin/mingw-release` folders respectively (Settings -> Build, Execution, Deployment -> CMake -> Generation Path).


## Built With

* [Vulkan](https://www.khronos.org/vulkan/) - 3D graphics and computing
* [GLFW](https://www.glfw.org/) - Create and manage windows
* [CMake](https://cmake.org/) - Build system


## License

This project is licensed under the GNU GPL v3.0 - see [LICENSE](LICENSE.md) for details.


## Acknowledgments
* I used the amazing [Vulkan Tutorial](https://vulkan-tutorial.com/Introduction) made by [Alexander Overvoorde](https://github.com/Overv) to get started with the Vulkan API.


