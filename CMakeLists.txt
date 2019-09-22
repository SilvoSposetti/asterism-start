cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME asterism)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################
# Project files:

file(GLOB_RECURSE SOURCE_FILES
        include/*.h
        src/*.cpp)

set(PROJECT_INCLUDE_DIR include)
include_directories(${PROJECT_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


####################
# Submodules:

# GLM:
# Path:
set(GLM_DIR ${PROJECT_SOURCE_DIR}/ext/glm)
# Include the glm headers:
include_directories(${GLM_DIR})
# GLM is a header only library, so no need to link with it


# GLFW:
# Path:
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/ext/glfw)
# Options:
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Include the glfw headers:
include_directories(${GLFW_DIR}/include)
# Specify glfw's cmakelists.txt location:
add_subdirectory(${GLFW_DIR})
# Link with GLFW:
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)


# GLSLANG:
# Path:
set(GLSL_DIR ${PROJECT_SOURCE_DIR}/ext/glslang)
# Specify glslang's cmakelists.txt location:
add_subdirectory(${GLSL_DIR})
# Include the glslang headers:
include_directories(${GLFW_DIR}/include)


####################
# Vulkan:
find_package(Vulkan REQUIRED)
set(VULKAN_DIR $ENV{VULKAN_SDK})
STRING(REGEX REPLACE "\\\\" "/" VULKAN_DIR ${VULKAN_DIR})
include_directories(${VULKAN_DIR}/Include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${VULKAN_DIR}/Lib/vulkan-1.lib)