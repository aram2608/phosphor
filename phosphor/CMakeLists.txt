# Enable FetchContent
include(FetchContent)

# Installing dependencies
# We try to find using whatever package manager is available and fall back to fetch

find_package(raylib 3.0)
if (NOT raylib_FOUND)
    message(STATUS "raylib not found. Fetching from source via FetchContent.")
    FetchContent_Declare(
        raylib_content
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        # Stable version
        GIT_TAG 4.5.0
    )
    FetchContent_MakeAvailable(raylib_content)
    set(RAYLIB_LINK_TARGET raylib)
else()
    message(STATUS "raylib found, fetching glfw3")
    find_package(glfw3)
    set(RAYLIB_LINK_TARGET raylib)
    set(GLFW_LINK_TARGET glfw)
endif()

find_package(cxxopts CONFIG)
if (NOT cxxopts_FOUND)
    message(STATUS "cxxopts not found. Fetching from source via FetchContent.")
    FetchContent_Declare(
        cxxopts_content
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts
        # Stable version
        GIT_TAG v3.3.0
    )
    FetchContent_MakeAvailable(cxxopts_content)
    set(CXXOPTS_LINK_TARGET cxxopts::cxxopts)
else()
    message(STATUS "cxxopts found.")
    set(CXXOPTS_LINK_TARGET cxxopts::cxxopts)
endif()

# Lua is complex to fetch so pray to god that vcpkg or brew can find it
find_package(Lua)
if (NOT LUA_FOUND)
    message(STATUS "Lua interpreter was not found. Scripting module may fail to compile or link.")
    FetchContent_Declare(
        lua
        GIT_REPOSITORY "https://github.com/marovira/lua"
        GIT_TAG 5.4.8
    )
    FetchContent_MakeAvailable(lua)
    set(LUA_INC_DIR ${LUA_INCLUDE_DIR})
    set(LUA_LIBS ${LUA_LIBRARIES})
else()
    message(STATUS "Lua interpreter found.")
    set(LUA_INC_DIR ${LUA_INCLUDE_DIR})
    set(LUA_LIBS ${LUA_LIBRARIES})
endif()

find_package(sol2 CONFIG)
if (NOT sol2_FOUND)
    message(STATUS "sol2 not found. Fetching from source via FetchContent.")
    FetchContent_Declare(
        sol2_content
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        # Stable version
        GIT_TAG v3.0.2
    )
    FetchContent_MakeAvailable(sol2_content)
    set(SOL2_INCLUDE_DIR ${sol2_content_SOURCE_DIR}/include)
else()
    set(SOL2_INCLUDE_DIR ${sol2_INCLUDE_DIRS})
endif()

add_library(phosphlib
    phosphlib/editor/editor.cpp
    phosphlib/gap_buffer/gap_buffer.cpp
    phosphlib/ui/ui.cpp
    phosphlib/scripting/scripting.cpp
)

target_include_directories(phosphlib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/phosphlib/
    ${SOL2_INCLUDE_DIR}
    ${LUA_INC_DIR}
)

target_link_libraries(phosphlib
    PUBLIC
    ${RAYLIB_LINK_TARGET}
    ${GLFW_LINK_TARGET}
    ${CXXOPTS_LINK_TARGET}
    ${LUA_LIBS}
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