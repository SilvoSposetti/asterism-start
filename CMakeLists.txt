cmake_minimum_required(VERSION 3.14)

####################
# Project properties:
set(PROJECT_NAME asterism)
project(${PROJECT_NAME} VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


####################
# Project files:

file(GLOB_RECURSE SOURCE_FILES
        include/*.h
        src/*.cpp)

set(PROJECT_INCLUDE_DIR include)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIR})


####################
# Submodules:

# GLM:
# Path:
set(GLM_DIR ${PROJECT_SOURCE_DIR}/ext/glm)
# Include the glm headers:
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_DIR})
# GLM is a header only library, so no need to link with it


# GLFW:
# Path:
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/ext/glfw)
# Options:
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Include the glfw headers:
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_DIR}/include)
# Specify glfw's CMakeLists.txt location:
add_subdirectory(${GLFW_DIR})
# Link with GLFW:
#add_library(glfw ${GLFW_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)


# GLSLANG:
# Path:
set(GLSL_DIR ${PROJECT_SOURCE_DIR}/ext/glslang)
# Include the glslang headers:
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_DIR}/include)
# Specify glslang's CMakeLists.txt location:
add_subdirectory(${GLSL_DIR})
# Link with GLSLANG and SPIRV
target_link_libraries(${PROJECT_NAME} PUBLIC glslang SPIRV)


####################
# Vulkan:
set(VULKAN_DIR $ENV{VULKAN_SDK})
STRING(REGEX REPLACE "\\\\" "/" VULKAN_DIR ${VULKAN_DIR})
include_directories(${VULKAN_DIR}/Include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${VULKAN_DIR}/Lib/vulkan-1.lib)

####################
# Definitions:
add_definitions(-DSOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")


