find_package(raylib 3.0 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

add_library(phosphlib
    phosphlib/editor/editor.cpp
    phosphlib/gap_buffer/gap_buffer.cpp
    phosphlib/ui/ui.cpp
)

target_include_directories(phosphlib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/phosphlib/
)

target_link_libraries(phosphlib
    PUBLIC
    raylib
    glfw
    cxxopts::cxxopts
)

target_compile_options(phosphlib
    PRIVATE
    -Wall 
    -Wextra 
    -Wpedantic 
    -Wconversion
    -Wpessimizing-move
    -Wredundant-move
    -Wmove
    -Wrange-loop-construct
)

add_executable(phosphor
    app/main.cpp
)

target_link_libraries(phosphor
    PRIVATE
    phosphlib
)

if(APPLE)
    target_link_libraries(phosphor
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework OpenGL"
    )
endif()