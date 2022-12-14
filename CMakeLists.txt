cmake_minimum_required(VERSION 3.14...3.25)
project(PSXEmulator VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION" )
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# No in-tree builds
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

# Check Git submodules.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.gitmodules")
    execute_process (COMMAND git submodule update --init
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set (IMGUI_DIR ../imgui) # Set Imgui dir for imgui-sfml to work

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
elseif(APPLE)
    set(IMGUI_SFML_FIND_SFML OFF) # Make imgui-sfml work with brew SFML installs
endif()
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

if(NOT SFML_FOUND)
  message(FATAL_ERROR "SFML couldn't be located!")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/GUI/)
include_directories (${SFML_INCLUDE_DIR})
include_directories(third-party/)
include_directories(third-party/imgui/)
include_directories(third-party/imgui-sfml/)
include_directories(third-party/fmt/include)
include_directories(third-party/Dolphin)
include_directories(third-party/tinyfiledialogs)
include_directories(third-party/imgui-club/imgui_memory_editor)
include_directories(third-party/json)
include_directories(third-party/sha1)
include_directories(third-party/mio/single_include)
include_directories(third-party/SaveFile/include)

# Capstone Library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
set(CAPSTONE_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(CAPSTONE_ARCHITECTURE_DEFAULT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_MIPS_SUPPORT ON CACHE BOOL "MIPS" FORCE)

add_subdirectory(third-party/capstone)
include_directories(third-party/capstone/include)


add_subdirectory(third-party/imgui-sfml)
add_subdirectory(third-party/calib)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/emulator.cpp
    src/GUI/gui.cpp

    third-party/imgui/imgui_draw.cpp
    third-party/imgui/imgui_demo.cpp
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/imgui.cpp
    third-party/imgui-sfml/imgui-SFML.cpp
    third-party/tinyfiledialogs/tinyfiledialogs.c
    third-party/sha1/sha1.cpp
    third-party/fmt/src/os.cc
    third-party/fmt/src/format.cc src/mem.cpp src/cpu.cpp src/GUI/disassembly.cpp
    src/instructions.cpp src/gte_instructions.cpp src/GUI/regviewer.cpp src/GUI/logger.cpp
    src/GUI/debuginfo.cpp src/GUI/memviewer.cpp)

# set_property(TARGET MyEmulator PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) # Enable LTO
find_package(OpenGL REQUIRED)

if(WIN32)
    target_link_libraries (${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window Imm32 glu32 ${OPENGL_LIBRARY} calib capstone-static)
else()
    target_link_libraries (${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window ${OPENGL_LIBRARY} calib capstone-static)
endif()
