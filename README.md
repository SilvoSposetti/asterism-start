#Asterism
![](https://img.shields.io/github/license/SilvoSposetti/asterism.svg)
![](https://img.shields.io/github/repo-size/SilvoSposetti/asterism.svg)
![](https://img.shields.io/github/languages/code-size/SilvoSposetti/asterism.svg)

A lightweight Vulkan renderer.

##Getting Started: [Windows and Linux]

* Install [Cygwin](https://cygwin.com/install.html) with packages `gcc-g++`, `make`, `GDB`, and `cmake`.
I ended up using the bundled CMake.
* Install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
* Pick the editor you prefer, I went with CLion. Open the existing CMAKE project in the root folder, then set the default generation path to the `bin` folder (in CLion: Settings -> Build, Execution, Deployment -> CMake -> Generation Path)
* If everything went smoothly, it should build and run.

## Built With

* [Vulkan](https://www.khronos.org/vulkan/) - 3D graphics and computing
* [Cygwin](https://www.cygwin.com/) - Compiler & Debugger
* [CMake](https://cmake.org/) - Build System

## License

This project is licensed under the GNU GPL v3.0 - see the [LICENSE](LICENSE.md) file for details

## Acknowledgments

* I'm grateful to Arseny Kapoulkine ([zeux](https://github.com/zeux)) for his streams on [YouTube](https://www.youtube.com/playlist?list=PL0JVLUVCkk-l7CWCn3-cdftR0oajugYvd) about building a Vulkan renderer.

