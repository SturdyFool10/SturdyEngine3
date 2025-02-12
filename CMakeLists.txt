cmake_minimum_required(VERSION 3.30)
project(SturdyEngine3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Gather all source files for Core
file(GLOB_RECURSE CORE_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/Engine/Core/*.cpp
        ${CMAKE_SOURCE_DIR}/Engine/Core/*.h
)

# Create the Core library target
add_library(Core ${CORE_SOURCE_FILES})

# Get the VULKAN_SDK environment variable
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set.")
endif()
set(VULKAN_SDK $ENV{VULKAN_SDK})

# Define library search directories based on the platform
if(WIN32)
    message(STATUS "Linking libraries for Windows")
    set(INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows-static/include
            ${VULKAN_SDK}/Include
            ${CMAKE_SOURCE_DIR}/Engine
    )
    set(LIB_SEARCH_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows-static/lib
            ${VULKAN_SDK}/Lib
    )
elseif(APPLE)
    message(STATUS "Linking libraries for macOS")
    set(INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-macos/include
            ${VULKAN_SDK}/include
            ${CMAKE_SOURCE_DIR}/Engine
    )
    set(LIB_SEARCH_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-macos/lib
            ${VULKAN_SDK}/lib
    )
elseif(UNIX)
    message(STATUS "Linking libraries for Linux")
    set(INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/include
            ${VULKAN_SDK}/include
            ${CMAKE_SOURCE_DIR}/Engine
    )
    set(LIB_SEARCH_DIRS
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/lib
            ${VULKAN_SDK}/lib
    )
endif()

target_include_directories(Core PRIVATE ${INCLUDE_DIRS})

# Gather all source files for Editor
file(GLOB_RECURSE EDITOR_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/Editor/*.cpp
        ${CMAKE_SOURCE_DIR}/src/Editor/*.h
)

set(DOWNSTREAM_INCLUDE_DIRS ${INCLUDE_DIRS})
list(APPEND DOWNSTREAM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Engine)

# Create the Editor executable target
add_executable(Editor ${EDITOR_SOURCE_FILES})
target_link_libraries(Editor PRIVATE Core)
target_include_directories(Editor PRIVATE ${DOWNSTREAM_INCLUDE_DIRS})

# Gather all source files for Runtime
file(GLOB_RECURSE RUNTIME_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/Runtime/*.cpp
        ${CMAKE_SOURCE_DIR}/src/Runtime/*.h
)

# Create the Runtime executable target
add_executable(Runtime ${RUNTIME_SOURCE_FILES})
target_link_libraries(Runtime PRIVATE Core)
target_include_directories(Runtime PRIVATE ${DOWNSTREAM_INCLUDE_DIRS})

# Automatically link against every library file found in the search directories for Core, Editor, and Runtime
foreach(lib_dir ${LIB_SEARCH_DIRS})
    if(EXISTS ${lib_dir})
        # Gather all files in the library directory
        file(GLOB LIB_FILES "${lib_dir}/*")
        foreach(lib_file ${LIB_FILES})
            # Check for common library extensions: .so (Linux shared), .a (Linux static), .lib (Windows)
            if(lib_file MATCHES ".*\\.(so|a|lib)$")
                message(STATUS "Linking library: ${lib_file}")
                target_link_libraries(Core PRIVATE ${lib_file})
                target_link_libraries(Editor PRIVATE ${lib_file})
                target_link_libraries(Runtime PRIVATE ${lib_file})
            endif()
        endforeach()
    else()
        message(WARNING "Library directory ${lib_dir} does not exist.")
    endif()
endforeach()
