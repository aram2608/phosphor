find_package(raylib 3.0 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)

add_library(phosphlib
    phosphlib/editor/editor.cpp
    phosphlib/gap_buffer/gap_buffer.cpp
    phosphlib/ui/ui.cpp
    phosphlib/scripting/scripting.cpp
)

target_include_directories(phosphlib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/phosphlib/
    ${sol2_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
)

target_link_libraries(phosphlib
    PUBLIC
    raylib
    glfw
    cxxopts::cxxopts
    ${LUA_LIBRARIES}
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