# Asterism ![](https://img.shields.io/github/license/SilvoSposetti/asterism.svg) ![](https://img.shields.io/github/repo-size/SilvoSposetti/asterism.svg) ![](https://img.shields.io/github/languages/code-size/SilvoSposetti/asterism.svg)

A lightweight Vulkan renderer.


## Getting Started: [Windows]

* Install the [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2017)

* Install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)

* The precompiled binaries and include headers for [GLFW](https://www.glfw.org/download.html) are already bundled in the repo, under `ext/glfw`

* [For the CLion editor]. Open the existing CMAKE project in the root folder, then set the default generation path to the `bin` folder (Settings -> Build, Execution, Deployment -> CMake -> Generation Path). Then under (Settings -> Build, Execution, Deployment -> Toolchains select the VS19 tools and amd64 as architecture.


## Built With

* [Vulkan](https://www.khronos.org/vulkan/) - 3D graphics and computing
* [GLFW](https://www.glfw.org/) - Create and manage windows
* [CMake](https://cmake.org/) - Build system


## License

This project is licensed under the GNU GPL v3.0 - see the [LICENSE](LICENSE.md) file for details.


## Acknowledgments

* I'm grateful to Arseny Kapoulkine ([zeux](https://github.com/zeux)) for his streams on [YouTube](https://www.youtube.com/playlist?list=PL0JVLUVCkk-l7CWCn3-cdftR0oajugYvd) about building a Vulkan renderer. See also his [repo](https://github.com/zeux/niagara).

