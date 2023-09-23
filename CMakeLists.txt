cmake_minimum_required(VERSION 3.12)

set (CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

Project(Game)

add_executable(game
    src/main.cpp

    src/Core/Assets.cpp
    src/Core/Assets.h
    src/Core/Core.cpp
    src/Core/Core.h
    src/Core/Game.cpp
    src/Core/Game.h
    src/Core/Input.cpp
    src/Core/Input.h
    src/Core/Profiler.cpp
    src/Core/Profiler.h

    src/Data/Blocks.cpp
    src/Data/Blocks.h
    src/Data/CubeData.cpp
    src/Data/CubeData.h

    src/Game/Chunk.cpp
    src/Game/Chunk.h
    src/Game/Player.cpp
    src/Game/Player.h
    src/Game/World.cpp
    src/Game/World.h
    src/Game/WorldGeneration.cpp
    src/Game/WorldGeneration.h

    src/Graphics/BatchRenderer.cpp
    src/Graphics/BatchRenderer.h
    src/Graphics/Buffers.cpp
    src/Graphics/Buffers.h
    src/Graphics/Cubemap.cpp
    src/Graphics/Cubemap.h
    src/Graphics/DebugRender.cpp
    src/Graphics/DebugRender.h
    src/Graphics/Shader.cpp
    src/Graphics/Shader.h
    src/Graphics/Texture.cpp
    src/Graphics/Texture.h

    src/Maths/Maths.cpp
    src/Maths/Maths.h
    src/Maths/Types.h

    src/Physics/AABB.cpp
    src/Physics/AABB.h
    src/physics/Frustum.cpp
    src/physics/Frustum.h
    src/physics/VoxelRayCast.cpp
    src/physics/VoxelRayCast.h

    vendor/glad/glad.c

    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_impl_glfw.cpp
    vendor/imgui/imgui_impl_opengl3.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui.cpp

    vendor/stb-src/stb_impl.cpp

    vendor/spdlog-src/async.cpp
    vendor/spdlog-src/cfg.cpp
    vendor/spdlog-src/color_sinks.cpp
    vendor/spdlog-src/file_sinks.cpp
    vendor/spdlog-src/fmt.cpp
    vendor/spdlog-src/spdlog.cpp
    vendor/spdlog-src/stdout_sinks.cpp
)


target_include_directories(game
    PRIVATE vendor
    PRIVATE src
)

target_link_directories(game
    PRIVATE OpenGL
    PRIVATE lib
)

target_link_libraries(game
    PRIVATE glfw3
)

add_compile_definitions(game
    GLFW_INCLUDE_NONE
    PROFILER_ENABLE_ALL
    SPDLOG_COMPILED_LIB
)