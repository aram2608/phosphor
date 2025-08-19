cmake_minimum_required (VERSION 3.11)
project(phosphor
    LANGUAGES CXX
    VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages and config files
find_package(raylib 3.0 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(ftxui REQUIRED)

# Add executables (.cpp) files
add_executable(${PROJECT_NAME} phosphor/src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/phosphor/include)

target_link_libraries(${PROJECT_NAME}
    raylib 
    glfw
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()