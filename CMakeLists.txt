cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME asterism)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES src/asterism.cpp)
add_executable(asterism ${SOURCE_FILES})


# GLFW:
# Include the header:
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/ext/glfw/include)
include_directories(${INCLUDE_DIRS})
# Link static library:
set(GLFW3_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/ext/glfw/lib-vc2019)

add_library(glfw3 STATIC IMPORTED)
set_property(TARGET glfw3 PROPERTY IMPORTED_LOCATION ${GLFW3_LIBRARY_DIR}/glfw3.lib)

target_link_libraries(${PROJECT_NAME} glfw3)