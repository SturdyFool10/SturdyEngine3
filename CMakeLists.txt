cmake_minimum_required(VERSION 3.30)
project(SturdyEngine3 LANGUAGES CXX)

set(SHARE_DIR F:/Projects/SturdyEngine3/vcpkg_installed/x64-windows-static/share)
set(glfw3_DIR ${SHARE_DIR}/glfw3)
set(glm_DIR ${SHARE_DIR}/glm)
set(OpenAL_DIR ${SHARE_DIR}/openal-soft)


set(CMAKE_CXX_STANDARD 26)
# Application Specific Deps go here, this allows for easy linking of libraries, the Core and its deps are automatically linked, but you sure can add more here!

set(Runtime_INCLUDE_DIRS
        # add any directories where *.h files are located, this includes them in runtime executable
)

set(Runtime_LIB_SEARCH_DIRS
        # add any directories where *.lib, *.a or *.so files are located, this links them to runtime executable
)

set(Editor_INCLUDE_DIRS
        # add any directories where *.h files are located, this includes them in Editor executable
)

set(Editor_LIB_SEARCH_DIRS
        # add any directories where *.lib, *.a or *.so files are located, this links them to Editor executable
)
# Get the VULKAN_SDK environment variable
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set.")
endif()
set(VULKAN_SDK $ENV{VULKAN_SDK})
# Function to gather source files
function(gather_source_files target_name target_dir)
    file(GLOB_RECURSE ${target_name}_SOURCE_FILES ${CMAKE_SOURCE_DIR}/${target_dir}/*.cpp ${CMAKE_SOURCE_DIR}/${target_dir}/*.h)
    set(${target_name}_SOURCE_FILES ${${target_name}_SOURCE_FILES} PARENT_SCOPE)
endfunction()

# Function to set include and library directories based on platform
    if(WIN32)
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
        set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-macos/include ${VULKAN_SDK}/Include ${CMAKE_SOURCE_DIR}/Engine)
        set(LIB_SEARCH_DIRS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-macos/lib ${VULKAN_SDK}/lib)
    elseif(UNIX)
        set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/include ${VULKAN_SDK}/Include ${CMAKE_SOURCE_DIR}/Engine)
        set(LIB_SEARCH_DIRS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/lib ${VULKAN_SDK}/lib)
    endif()

# Function to link libraries automatically
function(link_libraries_automatically target_name)
    foreach(lib_file ${CORE_LIB_FILES})
        message(STATUS "Linking library ${lib_file} to ${target_name}.")
        target_link_libraries(${target_name} PRIVATE ${lib_file})
    endforeach()
    if(DEFINED ${target_name}_LIB_SEARCH_DIRS)
        message(STATUS "Linking libraries to ${target_name}.")
        foreach(lib_dir ${${target_name}_LIB_SEARCH_DIRS})
            message(STATUS "Searching for libraries in ${lib_dir}.")
            if(EXISTS ${lib_dir})
                file(GLOB LIB_FILES "${lib_dir}/*")
                foreach(lib_file ${LIB_FILES})
                    if(lib_file MATCHES ".*\\.(so|a|lib)$")
                        message(STATUS "Linking library ${lib_file} to ${target_name}.")
                        target_link_libraries(${target_name} PRIVATE ${lib_file})
                    endif()
                endforeach()
            else()
                message(WARNING "Library directory ${lib_dir} does not exist.")
            endif()
        endforeach()
    endif()
endfunction()

# Gather core library files
set(CORE_LIB_FILES)
foreach(lib_dir ${LIB_SEARCH_DIRS})
    message(STATUS "Searching for libraries in ${lib_dir}.")
    if(EXISTS ${lib_dir})
        file(GLOB LIB_FILES "${lib_dir}/*")
        foreach(lib_file ${LIB_FILES})

            if(lib_file MATCHES ".*\\.(so|a|lib)$")
                message(STATUS "Found library ${lib_file}.")
                list(APPEND CORE_LIB_FILES ${lib_file})
            endif()
        endforeach()
    else()
        message(STATUS "Library directory ${lib_dir} does not exist.")
    endif()
endforeach()
# Core library
gather_source_files(Core "Engine/Core")
add_library(Core ${Core_SOURCE_FILES})
target_include_directories(Core PUBLIC ${INCLUDE_DIRS})
link_libraries_automatically(Core)

# Editor executable
gather_source_files(Editor "src/Editor")
add_executable(Editor ${Editor_SOURCE_FILES})
target_link_libraries(Editor PRIVATE Core)
target_include_directories(Editor PRIVATE ${INCLUDE_DIRS})
target_include_directories(Editor PRIVATE ${Editor_INCLUDE_DIRS})
link_libraries_automatically(Editor)

# Runtime executable
gather_source_files(Runtime "src/Runtime")
add_executable(Runtime ${Runtime_SOURCE_FILES})
target_link_libraries(Runtime PRIVATE Core)
target_include_directories(Runtime PRIVATE ${INCLUDE_DIRS})
target_include_directories(Runtime PRIVATE ${Runtime_INCLUDE_DIRS})
link_libraries_automatically(Runtime)
message(STATUS "Project_dir: ${PROJECT_SOURCE_DIR}")