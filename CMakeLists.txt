cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME asterism)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES src/asterism.cpp)
add_executable(asterism ${SOURCE_FILES})

########################################################################################################################
# GLFW:

# Include the header:
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/ext/glfw/include)
include_directories(${INCLUDE_DIRS})

# Link static library:
set(GLFW3_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/ext/glfw/lib-mingw-w64)
add_library(glfw3 STATIC IMPORTED)
set_property(TARGET glfw3 PROPERTY IMPORTED_LOCATION ${GLFW3_LIBRARY_DIR}/libglfw3.a)
target_link_libraries(${PROJECT_NAME} glfw3)

########################################################################################################################\
# Vulkan:
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

########################################################################################################################\
# Compile Definitions:
if (WIN32) # On Windows:
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif(WIN32)
if(UNIX) # On Linux:
#don't know yet, ToDo: Fill-in
endif(UNIX)